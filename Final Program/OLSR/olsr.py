from socket import*
import time
from threading import Thread
import datetime
import sys
import json
import heapq

data = []
mrp = ""
router_graph = {}
off = 3
off_time = 1
send_time = 1
neighbor_count = 0
get_recent_time = {}
this_router = sys.argv[1]
seq_num = 0
seq_num_new = 0
sent_msg = ""
router_ports = {
    1000: "A",
    1001: "B",
    1002: "C",
    1003: "D",
    1004: "E",
    1005: "F",
    }
myServer = socket(AF_INET, SOCK_DGRAM)

def startServer():
    # Starts the server on localhost
    myServer.bind(('', int(sys.argv[2])))

def check_connection_state():
    # checks to see if the router is on
    global seq_num, sent_msg
    new = {}
    new['data'] = [x for x in data if x[3] == True]
    new['seq'] = seq_num
    new['time'] = datetime.datetime.now()
    sent_msg = this_router+"*3"+"*"+str(new)
    seq_num += 1
    router_graph[this_router] = new

def check_msgs(msg, sender, my_msg, option):
    # checks to see if recieved message is or isnt in router graph, and update accordingly
    global get_recent_time
    if(msg[0] != this_router):
        current_msg = eval(msg[2])
        checkIf = False

        temp = 0
        new_temp = 0
        if (msg[0] in router_graph):
            temp = int(msg[4])
            new_temp = int(get_recent_time[msg[0]]['seq'])

        if(msg[0] not in router_graph):
            get_recent_time[msg[0]] = {}
            get_recent_time[msg[0]]['time'] = datetime.datetime.now()
            get_recent_time[msg[0]]['seq'] = msg[4]

        elif(temp > new_temp):
            get_recent_time[msg[0]]['time'] = datetime.datetime.now()
            get_recent_time[msg[0]]['seq'] = msg[4]
            checkIf = True

        if(msg[0] not in router_graph):
            router_graph[msg[0]] = current_msg

        elif(router_graph[msg[0]]['seq'] != current_msg['seq']):
            router_graph[msg[0]] = current_msg
        
        if option == "1":
            if checkIf:
                for y in range(neighbor_count):
                    if(int(data[y][2]) != sender):
                        try:
                            if current_msg['data'][0][5] == this_router:
                                myServer.sendto(my_msg, ("localhost", int(data[y][2])))
                        except:
                            pass
        if option == "2":
            if checkIf:
                for y in range(neighbor_count):
                    if(int(data[y][2]) != sender):
                        try:
                            if current_msg['data'][0][5] == this_router:
                                recvrouter = int(data[y][2])
                                print("sending information from port ", router_ports[sender], " to port ", router_ports[recvrouter])
                                myServer.sendto(my_msg, ("localhost", int(data[y][2])))
                        except:
                            pass
def get_rtable():
    #gets router table for specified router (ex. loads configA and grabs all of its content if user put A in the command line)
    global data, neighbor_count, mrp
    new_file = f'{this_router}_config.json'
    with open(new_file, 'r') as myfile1:
        data1=myfile1.read()
    obj = json.loads(data1)
    neighbor_count = (int(obj[f'{this_router}']['numofneighbors']))
    mrp = (str(obj[f'{this_router}']['MRP']))
    for x in range(neighbor_count):
        router = (str(obj[f'{this_router}']['routeTable'][x]['rname']))
        distance = (str(obj[f'{this_router}']['routeTable'][x]['dist']))
        port_number = (str(obj[f'{this_router}']['routeTable'][x]['port']))
        new_list = [router, distance, port_number]
        data.append(new_list)

    index = 0
    for y in data:
        data[index].insert(3, False)
        data[index].insert(4, datetime.datetime.now())
        data[index].insert(5, mrp)
        index += 1

    check_connection_state()

def router_state():
    # checks to see the current state of the router (used mainly to signal the program to turn on said router)
    for y in range(neighbor_count):
        packet = data[y][2] + "*1"
        packet = packet.encode('utf-8')
        myServer.sendto(packet, ("localhost", int(data[y][2])))

def check_router_state(option):
    # checks to see what value is received from the UDP connection. If the second part of said message is 1, it begins the process of waking up that router. If the option is 2, it successfully wakes up the router. 
    # if the option is 3, it handles the messages from that router and tries to add the data to the current router graph
    global myServer, data
    while True:
        try:
            msg, addr = myServer.recvfrom(1024)
            my_msg = msg
            msg = msg.decode('utf-8').split('*')
            if(msg[1] == '1'):
                packet = msg[0] + "*2"
                packet = packet.encode('utf-8')
                myServer.sendto(packet, addr)
            elif(msg[1] == '2'):
                for x in range(neighbor_count):
                    if(data[x][2] == msg[0]):
                        data[x][4] = datetime.datetime.now()
                        if(data[x][3] == False):
                            data[x][3] = True
                            check_connection_state()
                        break;
            elif(msg[1] == '3'):
                new_thread = Thread(target=check_msgs, args=(msg, addr[1], my_msg, option))
                new_thread.daemon = True
                new_thread.start()
        except:
            pass

def get_off_routers():
    # sets router state to false (off) if the router has been unresponsive for more than 3 seconds
    while True:
        for x in range(neighbor_count):
            if((datetime.datetime.now() - data[x][4]).seconds > off and data[x][3] == True):
               data[x][3] = False
               data[x][4] = datetime.datetime.now()
               check_connection_state()

            
        time.sleep(1)

def get_expired_routers():
    # pops the router out of the router graph if it has been turned off by the above function
    while True:
        try:
            for y in get_recent_time:
                if((datetime.datetime.now() - get_recent_time[y]['time']).seconds > off_time):
                    router_graph.pop(y, None)
                    get_recent_time.pop(y, None)
        except:
            pass
        time.sleep(1)

def generate_messages(option):
    # sends the initial messages from the router (ex. A sends all of its contents to B C and D)
    global seq_num_new
    if option == "1":
        while True:
            for y in range(neighbor_count):
                myServer.sendto((sent_msg+"*"+this_router+"*"+str(seq_num_new)).encode('utf-8'), ('localhost', int(data[y][2])))
            seq_num_new += 1
            time.sleep(1)
    if option == "2":
        while True:
            for y in range(neighbor_count):
                recvrouter = int(data[y][2])
                print("sending information from port ", this_router, " to port ", router_ports[recvrouter])
                myServer.sendto((sent_msg+"*"+this_router+"*"+str(seq_num_new)).encode('utf-8'), ('localhost', int(data[y][2])))
            seq_num_new += 1
            time.sleep(1)

def my_dijkstra(graph, this_router, new_router):
    # dijkstra algorithm for least cost path
    D = {router: float('inf') for router in graph}
    D[this_router] = 0
    N = {}

    pqueue = [(0, this_router)]
    while len(pqueue) > 0:
        cur_dist, cur_router = heapq.heappop(pqueue)
        if cur_router == new_router: break
        if cur_dist > D[cur_router]:
            continue
        for neigh, weight in graph[cur_router].items():
            new_dist = cur_dist + weight

            if new_dist < D[neigh]:
                D[neigh] = new_dist
                N[neigh] = cur_router
                heapq.heappush(pqueue, (new_dist, neigh))

    return (D, N)



def get_dijkstra(option):
    # returns the value of dijkstras algorithm if the option selected was 1, but if option 2 was selected do not print the results
    while True:
        try:
            this_graph = router_graph
            new_graph = {}
            for x in router_graph:
                new_graph[x] = {}
                for y in this_graph[x]["data"]:
                    new_graph[x][y[0]] = int(y[1])
            print("--------------------------------")
            print("Router ", this_router)
            if option == "1":
                for y in router_graph:
                    dist, path = my_dijkstra(new_graph, this_router, y)
                    fullPath = []
                    tCost = dist[y]
                    print("Least cost path to ", y, " is ", end = "")
                    while True:
                        fullPath.append(y)
                        if y == this_router: break
                        y = path[y]
                    fullPath.reverse()
                    for z in range(len(fullPath)):
                        print(fullPath[z], end = "")

                    print(" and the cost is: ", tCost)

        except Exception as e:
            print("Lost vital connection to ", e)
            pass
        time.sleep(2);

def main():
    print("Welcome to the Optimized Link State Routing Protocol (OLSR), please type either 1 or 2, from the options below")
    print("Option 1: Show Results of Dijkstras Algorithm")
    print("Option 2: Show sent and recieved information")
    options = ["1", "2"]
    breakval = input("Input selected option #: ")
    if breakval not in options:
        print("Not a valid option")
    else:
        startServer()
        get_rtable()
        router = Thread(target=check_router_state, args=[breakval])
        router.daemon = True
        router.start()
        off_routers = Thread(target=get_off_routers)
        off_routers.daemon = True
        off_routers.start()
        gen_msgs = Thread(target=generate_messages, args=[breakval])
        gen_msgs.daemon = True
        gen_msgs.start()
        expired_routers = Thread(target=get_expired_routers)
        expired_routers.daemon = True
        expired_routers.start()
        dijkstra = Thread(target=get_dijkstra, args=[breakval])
        dijkstra.daemon = True
        dijkstra.start()

        while True:
            time.sleep(send_time)
            router_state()

main()


