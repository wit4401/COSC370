#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };


extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;

int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table
{
  int costs[4][4];
} dt1;

struct rtpkt packet1;
/* students to write the following two routines, and maybe some others */

rtinit1()
{
printf("\n rtinit1 at time %f \n", clocktime);
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      dt1.costs[i][j] = 9999;
    }
  }

  dt1.costs[0][0] = 1;
  dt1.costs[1][1] = 0;
  dt1.costs[2][2] = 1;
  dt1.costs[3][3] = 9999;

  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      packet1.mincost[i] = dt1.costs[i][j];
    }
  }
  packet1.sourceid = 1;
  packet1.destid = 0;
  tolayer2(packet1);
  packet1.destid = 2;
  tolayer2(packet1);
  printdt1(&dt1);
}


rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;

{
int l = 0;
  printf("\n rtupdate1 at time %d \n", clocktime);
  int srcpkt = rcvdpkt->sourceid;
  printf("\n packet rcvd at node 1 from %d \n", srcpkt);
  for(int i=0; i<4; i++)
  {
    if(dt1.costs[i][srcpkt] > dt1.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i])
    {
      dt1.costs[i][srcpkt] = dt1.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i];
      if(i != 0)
        l = 1;
    }
  }
  
  if(l == 1)
  {
    printf("\n dtable at node 1 updated! \n");
    printdt1(&dt1);
    for(int i=0; i<4; i++)
    {
      if(i == 1)
        continue;
      else if(dt1.costs[i][0] < dt1.costs[i][2] && dt1.costs[i][0] < dt1.costs[i][3])
        packet1.mincost[i] = dt1.costs[i][0];
      else if(dt1.costs[i][2] < dt1.costs[i][0] && dt1.costs[i][2] < dt1.costs[i][3])
        packet1.mincost[i] = dt1.costs[i][2];
      else
        packet1.mincost[i] = dt1.costs[i][3];
    }
    packet1.mincost[1] = 0;
    printf("\n sending packets to nodes 0 and 2 with information: \n");
    for(int i=0; i<4; i++)
      printf("%d\t", packet1.mincost[i]);
    printf("\n");
    packet1.sourceid = 1;
    packet1.destid = 0;
    tolayer2(packet1);
    packet1.destid = 2;
    tolayer2(packet1);
  }
}


printdt1(dtptr)
  struct distance_table *dtptr;

{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



linkhandler1(linkid, newcost)
int linkid, newcost;
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */

{
}
