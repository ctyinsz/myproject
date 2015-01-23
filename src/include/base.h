#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iconv.h>
#include <fcntl.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define BACKLOG 5

#define ERROR(errmsg) { \
  printf("[%s][%d]MSG:[%s]\n",__FILE__,__LINE__,errmsg); \
  return -1; \
}

struct struSOCKLIST                  
{                                    
  int sockfd;                      
  int status; /*R_OK 4 W_OK 02 X_OK 01 */
  int opstat; /*R_OK 4 W_OK 02 X_OK 01 0 ´ý²éÖ¤*/
};                            
typedef struct struSOCKLIST SOCKLIST;

void daemon_init( int ign_sigcld );

int server_init(int port);
int server_start(int sockfd);
int server_accept(int sockfd,int timeout);

int getsockinfo(int commfd,int *localport, char *localaddr,int *remoteport,char *remoteaddr);
int setnonblocking(int sock);

int server_config(SOCKLIST *socketlist,size_t count,int op,int fd,int status);
int server_waiting(SOCKLIST *socketlist,size_t count,size_t *maxtime);

int epoll_config(int epfd, int op, int fd,uint32_t events);
//int epoll_waiting(int epfd, struct epoll_event * events,int maxevents, int timeout);

int recvallbytes(int s, void **buf, size_t len,int flags);


