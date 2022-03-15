from socket import *
import os
import sys
import struct
import time
import select
import binascii

ICMP_ECHO_REQUEST = 8
pingRTT = []
sentPackets = 0
receivedPackets = 0

def checksum(string):
    string = bytearray(string) #may not need???? (works with or without it)
    csum = 0
    countTo = (len(string) // 2) * 2
    count = 0

    while count < countTo:
        thisVal = string[count+1] * 256 + string[count]
        csum = csum + thisVal
        csum = csum & 0xffffffff
        count = count + 2

    if countTo < len(string):
        csum = csum + string[len(string) - 1]
        csum = csum & 0xffffffff

    csum = (csum >> 16) + (csum & 0xffff)
    csum = csum + (csum >> 16)
    answer = ~csum
    answer = answer & 0xffff

    answer = answer >> 8 | (answer << 8 & 0xff00)
    return answer

def recieveOnePing(mySocket, ID, timeout, destAddr):
    global receivedPackets
    timeLeft = timeout
    while 1:
        startedSelect = time.time()
        whatReady = select.select([mySocket], [], [], timeLeft)
        howLongInSelect = (time.time() - startedSelect)
        if whatReady[0] == []:
            return "Request timed out."

        timeRecieved = time.time()
        recPacket, addr = mySocket.recvfrom(1024)

        #fill in start
        icmpHeader = recPacket[20:28]
        icmptype, code, mychecksum, packetID, sequence = struct.unpack("bbHHh", icmpHeader) #unpack header in proper format
        if code != 0:
            return "Did not get code 0 (expected)"
        if icmptype != 0:
            return "Did not get type 0 (expected)"
        if code == 0 and icmptype == 0 and packetID == ID:
            bytesDouble = struct.calcsize("d")
            timeSent = struct.unpack("d", recPacket[28:28 + bytesDouble])[0]
            receivedPackets += 1
            pingRTT.append(timeRecieved - timeSent)
            return timeRecieved - timeSent
        #fill in end

        timeLeft = timeLeft - howLongInSelect
        if timeLeft <= 0:
            return "Request timed out."

def sendOnePing(mySocket, destAddr, ID):
    global sentPackets
    #header is type (8), code (8), checksum (16), id (16), sequence (16)
    myChecksum = 0

    #make a dummy header with a 0 checksum
    #struct --- interpret strings as packed binary data

    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, 1)
    data = struct.pack("d", time.time())

    #calculate the checksum on the data and the dummy header

    myChecksum = checksum(header + data)

    #get the right checksum, and put it in the header

    if sys.platform == 'darwin':
        #convert 16-bit integers from host to network byte order
        myChecksum = htons(myChecksum) & 0xffff
    else:
        myChecksum = htons(myChecksum)

    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, 1)
    packet = header + data

    mySocket.sendto(packet, (destAddr, 1)) 
    sentPackets += 1
    #increment the amount of sent packets

def doOnePing(destAddr, timeout):
    icmp = getprotobyname("icmp")

    # SOCK_RAW is a powerful socket type. For more details: http://sockraw.org/papers/sock_raw

    mySocket = socket(AF_INET, SOCK_RAW, icmp)

    myID = os.getpid() & 0xFFFF #return the current process i

    sendOnePing(mySocket, destAddr, myID)
    delay = recieveOnePing(mySocket, myID, timeout, destAddr)
    mySocket.close()

    return delay


def ping(host, timeout=1, count=10):

    # timeout=1 means: If one second goes by without a reply from the server,
    # the client assumes that either the client's ping or the server's pong is lost
    # count=10 means: Do the loop iteration 10 times per host to get consistent results

    dest = gethostbyname(host)
    print("Pinging " + dest + " using Python:")
    print("")

    # Send ping requests to a server separated by approximately one second

    for i in range(count):
        delay = doOnePing(dest, timeout)
        print("RTT:", delay)
        print("Max RTT:", (max(pingRTT) if len(pingRTT) > 0 else 0))
        print("Min RTT:", (min(pingRTT) if len(pingRTT) > 0 else 0))
        print("Avg RTT:", (sum(pingRTT)/len(pingRTT) if len(pingRTT) > 0 else 0))
        print("Packet Loss Rate:", ((sentPackets - receivedPackets)/sentPackets if receivedPackets > 0 else 0))
        print("")
        time.sleep(1) #one second
    return delay

#ping("127.0.0.1")
#print("-----------------------------------------------------")
#ping("www.tropenhaus.com.br") #pinging website in South America
#print("-----------------------------------------------------")
#ping("masrawy.com") #pinging website in Africa
#print("-----------------------------------------------------")
#ping("tu.berlin") #pinging website in Europe
#print("-----------------------------------------------------")
ping("www.aiit.or.kr") #pinging website in Asia
