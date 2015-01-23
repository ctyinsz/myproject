#include	"mpp.h"
extern pthread_mutex_t sq_lock;  
extern pthread_cond_t sq_flag;
extern  struct sockqueue *msg ;
extern  int portnumber;

void *thread_consumer(void * str)
{
	int ret;
	SOCKLIST workingsocket[MAXFDSET] = {0x00};
	int socknum = 0;
	int client_sockfd;
	int timeout=0;
	while(1)
	{
		//等待任务开始标志
		pthread_mutex_lock(&sq_lock); 
		while(isqempty(msg))
		{
			pthread_cond_wait(&sq_flag, &sq_lock); 
		}
		while(socknum < MAXFDSET)
		{
			ret = sem_trywait(&msg->rsem);
//			printf("sem_trywait ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
			if(ret == 0)
			{
				ret = dataget(msg,&client_sockfd);
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
					datapull(msg,client_sockfd);
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
		pthread_mutex_unlock(&sq_lock); 
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


int dealrequest(SOCKLIST *socketlist,size_t count)
{
	int ret,k,p;
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
//					for(k=0;k<1000;k++)
//						for(p=0;p<100000;p++);

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
	int timeout=10;
	
	memset(&ev,0x00,sizeof(ev));
	memset(events,0x00,sizeof(events));
	int epfd,nfds;
	
	epfd=epoll_create(MAXEPOLLSIZE);
	
	while(1)
	{
		//等待任务开始标志
		pthread_mutex_lock(&sq_lock); 
		while(isqempty(msg)&&!socknum)
		{
			pthread_cond_wait(&sq_flag, &sq_lock); 
		}
		while(socknum < MAXFDSET)
		{
			ret = sem_trywait(&msg->rsem);
//			printf("sem_trywait ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
			if(ret == 0)
			{
				ret = dataget(msg,&client_sockfd);
				if(ret)
					break;
				if(client_sockfd<=0)
					break;
//				printf("添加socket[%d]到监视列表\n",client_sockfd);
				
				setnonblocking(client_sockfd);
				ret = epoll_config(epfd, EPOLL_CTL_ADD, client_sockfd,EPOLLIN|EPOLLET);

//				printf("server_config ret=[%d],errno=[%d]:[%s]\n",ret,errno,strerror(errno));
				if(ret < 0 && errno == ENOMEM)
				{
					sem_wait(&msg->wsem);
					datapull(msg,client_sockfd);
					sem_post(&msg->rsem);
				}
				socknum++;
				sem_post(&msg->wsem);
			}
			else
			{
				if(errno == EAGAIN)
				{
					break;
				}
				else
				{
					printf("客户端文件描述符同步操作失败[%d][%s]\n",errno,strerror(errno));
					exit(1);
				}
			}
		}
		pthread_mutex_unlock(&sq_lock); 

		timeout = 5;
//		if(socknum<=0)
//		{
//			sleep(2);
//			continue;
//		}

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
					socknum--;
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
	int k,p;
//	char buf[BUF_SIZE+1]={0};
	char *buf = (char *)calloc(1,BUF_SIZE+1);
	char respbuf[BUF_SIZE+1]={0};

	n = recvallbytes(sockfd, &buf, BUF_SIZE,0);
//	n = recv(sockfd, buf, BUF_SIZE,0);
//			printf("socket %d recvd[%d],errno=[%d]:[%s]\n",sockfd,n,errno,strerror(errno));
	if(n > 0)
	{
		printf("socket[%d] received msg: [%d]:[%s] \n",sockfd,n,buf);
//    		xml_SetElement(hXml,FLOW"/flowname","mainflow.xml");
//    		xml_SetElement(hXml,COMMBUF,buf);
//    		ExeFlow(hXml);
//			strcpy(respbuf,buf);
//    		n=xml_GetElement(hXml,COMMBUF,respbuf,BUF_SIZE-1);
//					for(k=0;k<10000;k++)
//						for(p=0;p<1000000;p++);
		write(sockfd, buf,n);
//		write(sockfd, respbuf,n);
	}
	free(buf);
//	realloc(buf,0);
	return n;
}


