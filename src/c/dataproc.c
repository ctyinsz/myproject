#include	"mpp.h"

/**************************
队列操作函数
***************************/
int dataget(struct sockqueue *msg ,int *sockfd)
{
	if(msg->count<=0)
	{
		printf("Queue is empty\n");
		return 1;
	}
	else
	{
		msg->count--;
		if(msg->ridx >= MSG_NUM)
			msg->ridx = 0;
	}
	*sockfd = msg->Qsock[msg->ridx];
	msg->Qsock[msg->ridx] = -1;
	msg->ridx++;
//	printf("pop:%d,idx:%d\n",*sockfd,msg->ridx-1);
	return 0;
}

int datapull(struct sockqueue *msg ,int sockfd)
{
	
	if(msg->count>=MSG_NUM)
	{
		printf("Queue is full\n");
		return 1;
	}
	else
	{
		msg->count++;
		if(msg->widx >= MSG_NUM)
			msg->widx = 0;
	}
	msg->Qsock[msg->widx]=sockfd;
	msg->widx++;
//	printf("push:%d,idx:%d\n",sockfd,msg->widx-1);
	return 0;
}
int isqempty(struct sockqueue *msg )
{
	if(msg->count>0)
		return 0;
	else
		return 1;
}

struct sockqueue* sockqueue_init()
{
	int res,i;
	struct sockqueue* msg = NULL;
//	if(msg == NULL)
//		return 0;
	msg = (struct sockqueue*)calloc(1,sizeof(struct sockqueue));
  //初始化结构体
  msg->ridx = 0;
  msg->ridx = 0;
  msg->count = 0;
  for(i=0;i<MSG_NUM;i++)
  	msg->Qsock[i]=0;
   
  //初始化信号量,初始值为0  
  res = sem_init(&msg->rsem, 0, 0);  
  if(res == -1)  
  {  
      perror("semaphore intitialization failed\n");  
      return -1;
  }  
  //初始化信号量,初始值为1 
  res = sem_init(&msg->wsem, 0, MSG_NUM);  
  if( res == -1)  
  {  
      perror("semaphore intitialization failed\n");  
      return -1;  
  }	
	return msg;	
}

int sockqueue_close(struct sockqueue* msg)
{
	int res;
	if(msg == NULL)
		return 0;	
  sem_destroy(&msg->rsem);  
  sem_destroy(&msg->wsem); 
  free(msg);	
	return 1;
}


