#include	"pub.h"
#include	"base.h"

#define BACKLOG   5
#define MAXEPOLLSIZE 100000
#define MAXEPOLLEVENTS 100000
#define MAXFDSET 1024
struct sockqueue
{
	int Qsock[MSG_NUM];
	int ridx;
	int widx;
	int count;
	sem_t rsem;
	sem_t wsem;
};

void *thread_producer(void *);
void *thread_consumer(void *); 
void *thread_consumer_epoll(void *); 
int dataget(int *sockfd);
int datapull(int sockfd);
void sigchld_handler( int signo );
struct sockqueue* sockqueue_init();
struct sockqueue *msg = NULL;
int portnumber;

int main(int argc, char **argv)
{
	int res,i;
  if ( 2 == argc )
  {
      if( (portnumber = atoi(argv[1])) < 0 )
      {
          fprintf(stderr,"Usage:%s 端口号/a/n",argv[0]);
          return 1;
      }
  }
  else
  {
      fprintf(stderr,"Usage:%s 端口号/a/n",argv[0]);
      return 1;
  }

  pthread_t thread_p,thread_c;
	void *thread_result = NULL; 
	
	msg = sockqueue_init();
	if(msg <= 0)
	{
		perror("初始化失败\n"); 
		exit(EXIT_FAILURE); 
	}


    //创建生产者线程，并把msg作为线程函数的参数  
    res = pthread_create(&thread_p, NULL, thread_producer, NULL);  
    if(res != 0)  
    {
        perror("producer thread_create failed\n");  
        exit(EXIT_FAILURE);  
    }
    //创建消费者线程，并把msg作为线程函数的参数  
//    res = pthread_create(&thread_c, NULL, thread_consumer, NULL);  
    res = pthread_create(&thread_c, NULL, thread_consumer_epoll, NULL);  
    if(res != 0)  
    {
        perror("consumer thread_create failed\n");  
        exit(EXIT_FAILURE);  
    }	

  //等待子线程结束    
  res = pthread_join(thread_p, &thread_result);  
  if(res != 0)  
  {  
      perror("pthread_join failed\n");  
      exit(EXIT_FAILURE);  
  }
  printf("Thread joined\n");  
  
  sockqueue_close(msg);
	
	exit(EXIT_SUCCESS); 
}

void* thread_producer(void *msgs)
{
	
	int server_sock,client_sock;
	int timeout=30;

	server_sock = server_init(portnumber);

	if(server_sock > 0)
		server_start(server_sock);
	else
		exit(1);

	sem_wait(&msg->wsem);

	while(1)
	{
		client_sock = server_accept(server_sock,timeout);
		if(client_sock > 0)
		{
			datapull(client_sock);
			sem_post(&msg->rsem);
			sem_wait(&msg->wsem);  
		}
		else
			break;
	}

	pthread_exit(NULL);	
}
void *thread_consumer(void * str)
{
	int ret;
	SOCKLIST workingsocket[MAXFDSET] = {0x00};
	int socknum = 0;
	int client_sockfd;
	int timeout=0;
	while(1)
	{
		while(socknum < MAXFDSET)
		{
			ret = sem_trywait(&msg->rsem);
//			printf("sem_trywait ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
			if(ret == 0)
			{
				ret = dataget(&client_sockfd);
				if(ret)
					break;
				if(client_sockfd<=0)
					break;
//				printf("添加socket[%d]到监视列表\n",client_sockfd);
				ret = server_config(workingsocket,MAXFDSET,0,client_sockfd,R_OK);	
//				printf("server_config ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
				if(ret < 0)
				{
					sem_wait(&msg->wsem);
					datapull(client_sockfd);
					sem_post(&msg->rsem);
				}
				socknum++;
				sem_post(&msg->wsem);
			}
			else
			{
				if(errno == EAGAIN)
					break;
				else
				{
					printf("客户端文件描述符同步操作失败\n");
					exit(1);
				}
			}
		}
		timeout = 0;
		ret = server_waiting(workingsocket,MAXFDSET,&timeout);
//		printf("server_waiting ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
		if(ret == 0)
		{
//			printf("server_waiting ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
			socknum = socknum -dealrequest(workingsocket,MAXFDSET);
		}
		else
			continue;
	}
    //退出线程  
    pthread_exit(NULL);  
}

/**************************
队列操作函数
***************************/
int dataget(int *sockfd)
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

int datapull(int sockfd)
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
/**************************
进程终止信号回调函数
***************************/
void sigchld_handler( int signo ){
	printf("sigchld_handler\n");
	if (signo == SIGINT) 
	{ 
		printf("get SIGINT\n"); 
  }
    return;
}

struct sockqueue* sockqueue_init()
{
	int res,i;
	struct sockqueue* msg = NULL;
//	if(msg == NULL)
//		return 0;
	msg = (struct sockqueue*)calloc(sizeof(struct sockqueue),0);
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

int dealrequest(SOCKLIST *socketlist,size_t count)
{
	int ret;
	int i,j,n,cli_sock,closedsocknum=0;
	char buf[BUF_SIZE+1]={0};
	char respbuf[BUF_SIZE+1]={0};
	HXMLTREE hXml;
	
	for(i=0;i<count;i++)
	{
		if(socketlist[i].opstat == R_OK)
		{
			cli_sock = socketlist[i].sockfd;
      if((hXml = xml_Create("root"))==FAIL )
      	pthread_exit(NULL);  
      xml_SetElement(hXml,"/sys/respath","/home/traxex/app/myproject/cfg/");

//    	printf("waitting msg from socket %d \n",cli_sock);			

			n = recv(cli_sock, buf, BUF_SIZE,0);
//			printf("socket %d recvd[%d],errno=[%d]:[%s]\n",cli_sock,n,errno,strerror(errno));
			if(n > 0)
    	{
    		printf("socket[%d] received msg: [%s] \n",cli_sock,buf);
//    		xml_SetElement(hXml,FLOW"/flowname","mainflow.xml");
//    		xml_SetElement(hXml,COMMBUF,buf);
//    		ExeFlow(hXml);
    			strcpy(respbuf,buf);
//    		n=xml_GetElement(hXml,COMMBUF,respbuf,BUF_SIZE-1);

    		write(cli_sock, respbuf, n);
    		socketlist[i].opstat = 0 ;
    		xml_Destroy(hXml);
    		memset(buf,0x00,sizeof(buf));
    		memset(respbuf,0x00,sizeof(respbuf));
    	}

  		if(n == 0 )//1 短连接 0长连接&& errno != EAGAIN
    	{
    		server_config(socketlist,MAXFDSET,1,cli_sock,0);	
    		shutdown(cli_sock,2);
    		printf("socket %d closed \n",cli_sock);
    		closedsocknum++;
    	}
    	else
    	{
    		continue;
    	}	
		}
	}
	return closedsocknum;
}

void *thread_consumer_epoll(void * str)
{
	int ret,i;
	struct epoll_event ev,events[MAXEPOLLEVENTS];
	int socknum = 0;
	int client_sockfd,sockfd;
	int timeout=0;
	
	memset(&ev,0x00,sizeof(ev));
	memset(events,0x00,sizeof(events));
	int epfd,nfds;
	
	epfd=epoll_create(MAXEPOLLSIZE);
	
	while(1)
	{
		while(socknum < MAXFDSET)
		{
			ret = sem_trywait(&msg->rsem);
//			printf("sem_trywait ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
			if(ret == 0)
			{
				ret = dataget(&client_sockfd);
				if(ret)
					break;
				if(client_sockfd<=0)
					break;
//				printf("添加socket[%d]到监视列表\n",client_sockfd);
				
				ret = epoll_config(epfd, EPOLL_CTL_ADD, client_sockfd,EPOLLIN|EPOLLET);

//				ret = server_config(workingsocket,MAXFDSET,0,client_sockfd,R_OK);	
//				printf("server_config ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
				if(ret < 0 && errno == ENOMEM)
				{
					sem_wait(&msg->wsem);
					datapull(client_sockfd);
					sem_post(&msg->rsem);
				}
				socknum++;
				sem_post(&msg->wsem);
			}
			else
			{
				if(errno == EAGAIN)
					break;
				else
				{
					printf("客户端文件描述符同步操作失败\n");
					exit(1);
				}
			}
		}
		timeout = 0;
		
		nfds=epoll_wait(epfd,events,MAXEPOLLEVENTS,timeout);
		for(i=0;i<nfds;++i)
		{
			if(events[i].events&EPOLLIN)
			{
				if ( (sockfd = events[i].data.fd) < 0)
					continue;
				ret = epoll_dealrequest(sockfd);
				if(ret == 0)
				{
					epoll_config(epfd, EPOLL_CTL_DEL, sockfd,NULL);	
					shutdown(sockfd,2);
					printf("socket %d closed \n",sockfd);
				}
			}
		}
	}
    //退出线程  
    pthread_exit(NULL);  
}

int epoll_dealrequest(int sockfd)
{
	int n=0;
//	char buf[BUF_SIZE+1]={0};
	char *buf = (char *)calloc(BUF_SIZE+1,0);
	char respbuf[BUF_SIZE+1]={0};

//	n = recvallbyte(sockfd, &buf, BUF_SIZE,0);
	n = recv(sockfd, buf, BUF_SIZE,0);
//			printf("socket %d recvd[%d],errno=[%d]:[%s]\n",cli_sock,n,errno,strerror(errno));
	if(n > 0)
	{
		printf("socket[%d] received msg: [%s] \n",sockfd,buf);
//    		xml_SetElement(hXml,FLOW"/flowname","mainflow.xml");
//    		xml_SetElement(hXml,COMMBUF,buf);
//    		ExeFlow(hXml);
			strcpy(respbuf,buf);
//    		n=xml_GetElement(hXml,COMMBUF,respbuf,BUF_SIZE-1);

//		write(sockfd, buf,n);
		write(sockfd, respbuf,n);
	}
//	realloc(buf,0);
	return n;
}


