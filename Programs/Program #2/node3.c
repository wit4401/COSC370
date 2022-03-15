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

struct distance_table
{
  int costs[4][4];
} dt3;

/* students to write the following two routines, and maybe some others */

struct rtpkt packet3;

void rtinit3()
{
printf("\n rtinit3 at time %f \n", clocktime);
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      dt3.costs[i][j] = 9999;
    }
  }

  dt3.costs[0][0] = 7;
  dt3.costs[1][1] = 9999;
  dt3.costs[2][2] = 2;
  dt3.costs[3][3] = 0;

  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      packet3.mincost[i] = dt3.costs[i][j];
    }
  }
  packet3.sourceid = 3;
  packet3.destid = 0;
  tolayer2(packet3);
  packet3.destid = 2;
  tolayer2(packet3);
  printdt3(&dt3);
}


void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;

{
int l = 0;
  printf("\n rtupdate3 at time %d \n", clocktime);
  int srcpkt = rcvdpkt->sourceid;
  printf("\n packet rcvd at node 3 from %d \n", srcpkt);
  for(int i=0; i<4; i++)
  {
    if(dt3.costs[i][srcpkt] > dt3.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i])
    {
      dt3.costs[i][srcpkt] = dt3.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i];
      if(i != 0)
        l = 1;
    }
  }
  
  if(l == 1)
  {
    printf("\n dtable at node 3 updated! \n");
    printdt3(&dt3);
    for(int i=0; i<4; i++)
    {
      if(i == 3)
        continue;
      else if(dt3.costs[i][0] < dt3.costs[i][1] && dt3.costs[i][0] < dt3.costs[i][2])
        packet3.mincost[i] = dt3.costs[i][0];
      else if(dt3.costs[i][1] < dt3.costs[i][0] && dt3.costs[i][1] < dt3.costs[i][2])
        packet3.mincost[i] = dt3.costs[i][1];
      else
        packet3.mincost[i] = dt3.costs[i][2];
    }
    packet3.mincost[3] = 0;
    printf("\n sending packets to nodes 0 and 2 with information: \n");
    for(int i=0; i<4; i++)
      printf("%d\t", packet3.mincost[i]);
    printf("\n");
   packet3.sourceid = 3;
   packet3.destid = 0;
   tolayer2(packet3);
   packet3.destid = 2;
   tolayer2(packet3);
  }
}


printdt3(dtptr)
  struct distance_table *dtptr;

{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}
