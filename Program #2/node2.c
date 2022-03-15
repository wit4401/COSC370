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
} dt2;


/* students to write the following two routines, and maybe some others */
struct rtpkt packet2;

void rtinit2()
{
printf("\n rtinit2 at time %f \n", clocktime);
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      dt2.costs[i][j] = 9999;
    }
  }

  dt2.costs[0][0] = 3;
  dt2.costs[1][1] = 1;
  dt2.costs[2][2] = 0;
  dt2.costs[3][3] = 2;

  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      packet2.mincost[i] = dt2.costs[i][j];
    }
  }
  packet2.sourceid = 2;
  packet2.destid = 0;
  tolayer2(packet2);
  packet2.destid = 1;
  tolayer2(packet2);
  packet2.destid = 3;
  tolayer2(packet2);
  printdt2(&dt2);
}


void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;

{
int l = 0;
  printf("\n rtupdate2 at time %d \n", clocktime);
  int srcpkt = rcvdpkt->sourceid;
  printf("\n packet rcvd at node 2 from %d \n", srcpkt);
  for(int i=0; i<4; i++)
  {
    if(dt2.costs[i][srcpkt] > dt2.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i])
    {
      dt2.costs[i][srcpkt] = dt2.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i];
      if(i != 0)
        l = 1;
    }
  }
  
  if(l == 1)
  {
    printf("\n dtable at node 2 updated! \n");
    printdt2(&dt2);
    for(int i=0; i<4; i++)
    {
      if(i == 2)
        continue;
      else if(dt2.costs[i][0] < dt2.costs[i][1] && dt2.costs[i][0] < dt2.costs[i][3])
        packet2.mincost[i] = dt2.costs[i][0];
      else if(dt2.costs[i][1] < dt2.costs[i][0] && dt2.costs[i][1] < dt2.costs[i][3])
        packet2.mincost[i] = dt2.costs[i][1];
      else
        packet2.mincost[i] = dt2.costs[i][3];
    }
    packet2.mincost[2] = 0;
    printf("\n sending packets to nodes 0,1 and 3 with information: \n");
    for(int i=0; i<4; i++)
      printf("%d\t", packet2.mincost[i]);
    printf("\n");
   packet2.sourceid = 2;
   packet2.destid = 0;
   tolayer2(packet2);
   packet2.destid = 1;
   tolayer2(packet2);
   packet2.destid = 3;
   tolayer2(packet2);
  }
}


printdt2(dtptr)
  struct distance_table *dtptr;

{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}
