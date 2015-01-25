#include	"pub.h"
#include	"base.h"
#include	"ocilib.h"
#include  <unistd.h> 

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
int dataget(struct sockqueue *msg ,int *sockfd);
int datapull(struct sockqueue *msg ,int sockfd);
int isqempty(struct sockqueue *msg );
void sigchld_handler( int signo );
struct sockqueue* sockqueue_init();
int sockqueue_close(struct sockqueue* msg);

