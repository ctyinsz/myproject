#include	"mpp.h"
extern pthread_mutex_t sq_lock;  
extern pthread_cond_t sq_flag;
extern  struct sockqueue *msg ;
extern  int portnumber;

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
			pthread_mutex_lock(&sq_lock); 
			datapull(msg,client_sock);
			pthread_cond_signal(&sq_flag);
			pthread_mutex_unlock(&sq_lock); 
			sem_post(&msg->rsem);
			sem_wait(&msg->wsem);  
		}
		else
			break;
	}

	pthread_exit(NULL);	
}


