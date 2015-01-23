#include	"mpp.h"

pthread_mutex_t sq_lock;  
pthread_cond_t sq_flag;


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

	/*初始化互斥锁和条件变量*/
	pthread_mutex_init(&sq_lock, NULL);  
	pthread_cond_init(&sq_flag, NULL);  
	
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
  res = pthread_join(thread_c, &thread_result);  
  if(res != 0)  
  {  
      perror("pthread_join failed\n");  
      exit(EXIT_FAILURE);  
  }
  printf("Thread joined\n");  
  
  sockqueue_close(msg);
  pthread_mutex_destroy(&sq_lock);
  pthread_cond_destroy(&sq_flag);
	
	exit(EXIT_SUCCESS); 
}

