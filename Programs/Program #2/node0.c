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
} dt0;

struct rtpkt packet0;
/* students to write the following two routines, and maybe some others */
void rtinit0() 
{
  printf("\n rtinit0 at time %f \n", clocktime);
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      dt0.costs[i][j] = 9999;
    }
  }

  dt0.costs[0][0] = 0;
  dt0.costs[1][1] = 1;
  dt0.costs[2][2] = 3;
  dt0.costs[3][3] = 7;

  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      packet0.mincost[i] = dt0.costs[i][j];
    }
  }
  packet0.sourceid = 0;
  packet0.destid = 1;
  tolayer2(packet0);
  packet0.destid = 2;
  tolayer2(packet0);
  packet0.destid = 3;
  tolayer2(packet0);
  printdt0(&dt0);
}


void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  int l = 0;
  printf("\n rtupdate0 at time %d \n", clocktime);
  int srcpkt = rcvdpkt->sourceid;
  printf("\n packet rcvd at node 0 from %d \n", srcpkt);
  for(int i=0; i<4; i++)
  {
    if(dt0.costs[i][srcpkt] > dt0.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i])
    {
      dt0.costs[i][srcpkt] = dt0.costs[srcpkt][srcpkt] + rcvdpkt->mincost[i];
      if(i != 0)
        l = 1;
    }
  }
  
  if(l == 1)
  {
    printf("\n dtable at node 0 updated! \n");
    printdt0(&dt0);
    for(int i=1; i<4; i++)
    {
      if(dt0.costs[i][1] < dt0.costs[i][2] && dt0.costs[i][1] < dt0.costs[i][3])
        packet0.mincost[i] = dt0.costs[i][1];
      else if(dt0.costs[i][2] < dt0.costs[i][1] && dt0.costs[i][2] < dt0.costs[i][3])
        packet0.mincost[i] = dt0.costs[i][2];
      else
        packet0.mincost[i] = dt0.costs[i][3];
    }
    packet0.mincost[0] = 0;
    printf("\n sending packets to nodes 1,2, and 3 with information: \n");
    for(int i=0; i<4; i++)
      printf("%d\t", packet0.mincost[i]);
    printf("\n");
    packet0.sourceid = 0;
    packet0.destid = 1;
    tolayer2(packet0);
    packet0.destid = 2;
    tolayer2(packet0);
    packet0.destid = 3;
    tolayer2(packet0);
  }
}


printdt0(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
}

linkhandler0(linkid, newcost)   
  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
}
