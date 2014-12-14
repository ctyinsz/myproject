#include	"base.h"
//#include	"pub.h"
#define BUF_SIZE 1000
#define MAXEPOLLSIZE 100000
#define MAXEPOLLEVENTS 100000
void myserver(int port);
void myepollserver(int port);
int main(int argc, char **argv)
{
	myepollserver(5000);
	return 1;
}

void myserver(int port)
{
	int server_sock,client_sock;
	int rcd,n;
	char buf[BUF_SIZE+1]={0};
	SOCKLIST stSockList;
	int timeout=30;
	memset(&stSockList,0x00,sizeof(stSockList));

	server_sock = server_init(port);

	server_start(server_sock);

	while(1)
	{
		client_sock = server_accept(server_sock,timeout);
		server_config(&stSockList,1,0,client_sock,R_OK);
		while(client_sock>0)
		{
			rcd = server_waiting(&stSockList,1,&timeout);
			if(rcd == -1)
			{
				printf("server_waiting error\n");
				exit(1);
			}
			else if(rcd == 0)
			{
				printf("server_waiting time_out\n");
				continue;
			}
			else
			{
				memset(buf,0x00,sizeof(buf));
				n = recv(client_sock, buf, BUF_SIZE,0);
				if(n > 0)
				{
					printf("received msg: [%s]\n",buf);
					write(client_sock, buf, n);
					if(strcmp(buf,"quit") == 0)
					{
						shutdown(client_sock,2);
						printf("socket %d closed \n",client_sock);
						return ;
					}
				}
				if(n <= 0 && errno != EAGAIN)
				{
					shutdown(client_sock,2);
					printf("socket %d closed \n",client_sock);
					break;
				}
			}		
		}
	}	
	return ;
}

void myepollserver(int port)
{
	int server_sock,client_sock,sockfd;
	int rcd,n,i;
	int timeout=30;
	char buf[BUF_SIZE+1]={0};
	struct epoll_event ev,events[MAXEPOLLEVENTS];
	memset(&ev,0x00,sizeof(ev));
	memset(events,0x00,sizeof(events));

	int epfd,nfds;
	
	epfd=epoll_create(MAXEPOLLSIZE);

	server_sock = server_init(port);
	if(server_sock > 0)
		server_start(server_sock);
	else
		return;

	while(1)
	{
//		printf("等待新连接\n");
		client_sock = server_accept(server_sock,timeout);
		epoll_config(epfd, EPOLL_CTL_ADD, client_sock,EPOLLIN|EPOLLET);
		sockfd = client_sock;
		while(sockfd > 0)
		{
//			printf("等待读取数据\n");
			nfds=epoll_wait(epfd,events,MAXEPOLLEVENTS,1000);
//			printf("有%d条数据可读\n",nfds);
			for(i=0;i<nfds;++i)
			{
				if(events[i].events&EPOLLIN)
				{
					if ( (sockfd = events[i].data.fd) < 0)
						continue;

					n = read(sockfd, buf, BUF_SIZE);
//					printf("读第%d条数据,返回%d\n",i+1,n);
					if(n <= 0)
					{
						if(errno != EAGAIN)
						{
							shutdown(sockfd,2);
							epoll_config(epfd, EPOLL_CTL_DEL, sockfd,NULL);
							printf("socket %d closed \n",sockfd);
							sockfd = -1;
							break;	
						}
						else
							continue;
					}
					printf("received msg: [%s]\n",buf);
					write(sockfd, buf, n);
					if(strcmp(buf,"quit") == 0)
					{
						shutdown(sockfd,2);
						events[i].data.fd = -1;
						printf("socket %d closed \n",sockfd);
						sockfd = -1;
						return ;
					}
					memset(buf,0,sizeof(buf));
				}
			}
		}
	}	
	return ;	
}

