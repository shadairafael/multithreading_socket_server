#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<pthread.h>
#include<malloc.h>
#include <fcntl.h>        
#include <sys/stat.h> 
#include<time.h>
#include<semaphore.h>
#define BUF_SIZE 1024

//listener thread
void *thread_listen(void *arg)
{
	unsigned int sockfd=*((unsigned int*)arg);
	int i,size;	
	unsigned int acceptfd,ids2;
	socklen_t clilen;
	int newsockfd[10],c;
	int n;
	char buffer[256];
	pthread_t t_serve[10];
	struct sockaddr_in cli_addr;	
	clilen = sizeof(cli_addr);
	unsigned int retval;
	char request_buffer[1024];
	int retcode;
	off_t file_size;
	char in_buf[BUF_SIZE];
	
	char *fname=malloc(sizeof(char *));	
	struct stat st; 
	int k,j;
	
	int l;
	listen(sockfd,5);								// listens
	//printf("\nin listening thread before listen\n");

	while(1)
	{
		//printf("\nin listening thread before accept\n");
		acceptfd= accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
		if (acceptfd < 0) 
          	perror("error in accepting");

		unsigned int ip=cli_addr.sin_addr.s_addr;

		time_t now;
                time(&now);
                struct tm * ct=localtime(&now); //getting localtime
                int ch[128], time_arrival[128];
                struct timeval tv;
                strftime(ch, sizeof ch, "[%d/%b/%Y : %H:%M:%S %z]", ct); //format of the timestamp string we need
                snprintf(time_arrival, sizeof time_arrival, ch, tv.tv_usec); //printing the needed timestamp string

	/*	FILE * file_des=fopen(file,"a"); 
	printf("\n in  serving thread \n");

	fprintf(file_des,"%s\n",time_arrival);
	
	fclose(file_des);
		
	*/	
		char *file_name=malloc(sizeof(char *));
		memset(in_buf, 0, sizeof(in_buf));
		retcode = recv(acceptfd, in_buf, BUF_SIZE, 0);			
            
		//printf("\nin listening thread before getting file name\n");
		
		if (retcode < 0)
		{ 
			printf("recv error detected ...\n"); 
		}
    		else
		{    
										
			strtok(in_buf, " ");
		        file_name = strtok(NULL, " ");			
		}

		if(file_name!=NULL)
		{
			//ids2=acceptfd;
			//pthread_create(&t_serve[i],NULL,&thread_serve,&ids2);	
			
			//off_t fsize(const char *filename) 

			k=1,j=0; 
			while(k<strlen(file_name))
			{
				fname[j]=file_name[k];
				k++;
				j++;			
			}
			if (stat(fname, &st) == 0)
			file_size=st.st_size;
			//printf(" size of file %s is %zd",file_name,file_size);
			//printf("\nin listening thread after accepting and before inserting into queue\n");
			//pthread_mutex_lock(&qmutex);
			insertion(acceptfd,file_name,file_size,ip,time_arrival,in_buf);
			//printf("\n acceptfd is %d, file name is %s, file name is %zd",acceptfd,file_name,file_size);
			//printf("inserted ino queue");
			//pthread_mutex_unlock(&qmutex);
			//printf("newsockfd in thread is : %d",newsockfd);
		}
		else
		{
			continue;
		}		
	}				
}


int main(int argc, char *args[])
{
	int thread_status[10];     	
	pthread_t t_listener,t_scheduler,t_serve[10];
	int sockfd,ids;
	char *dir;
	file=malloc(sizeof(char *));
	dir=malloc(sizeof(char *));

	int portnum=8080,threadnum=4,sleep_time=60;
	int i;
	int help_flag=0,dir_flag=0,time_flag,threadnum_flag=0;
	
	// Parser code
	for(i=0;i<argc;i++)
	{
		//printf("\ncomparing%d",i);
		
		if(strcmp(args[i],"-h")==0)
		{
			help_flag=1;
		}
		else if(strcmp(args[i],"-n")==0)
		{
			threadnum=atoi(args[i+1]);
		}
		else if(strcmp(args[i],"-d")==0)
		{
			debug_flag=1;
			threadnum=1;
			
		}
		else if(strcmp(args[i],"-l")==0)
		{
			log_flag=1;
			file=args[i+1];
		}
		else if(strcmp(args[i],"-p")==0)
		{
			portnum=atoi(args[i+1]);
		}
		else if(strcmp(args[i],"-r")==0)
		{
			dir_flag=1;		
			dir=args[i+1];
		}
		else if(strcmp(args[i],"-t")==0)
		{
			time_flag=1;		
		 	sleep_time=atoi(args[i+1]);
		}
		else if(strcmp(args[i],"-s")==0)
		{
			if (strcmp(args[i+1],"FCFS")==0)		
				sched_flag=0;
			else if(args[i+1],"SJF")
				sched_flag=1;
			else
				printf("Please enter a proper scheduling algorithm");
		}
	
	}
	
	//free_thread=threadnum;
	sem_init(&sem,0,threadnum);

//printf( "\n debug : %d, help: %d, log: %d, file name : %s port num : %d, dir : %d dir name: %s, time :%d ,thread num : %d, sched : %d",debug_flag,help_flag,log_flag,file,portnum,dir_flag,dir,sleep_time,threadnum,sched_flag);	
	//Parser code ends

	if(help_flag==1)			// printing help options and exit if -h option is specified
	{
		print_help_options();
		exit(1);
	}
	else if(dir_flag==1)			//changing directory if -d option is specified
	{
		if(chdir(dir)<0)
			{
				perror("\ndirectory doesnt exist");
				exit(1);
			}
	}
	
	struct sockaddr_in serv_addr;
	//printf("before socket creation");
	sockfd = socket(AF_INET, SOCK_STREAM,0);			//creation of socket	
	//printf("\n after socket creation socket id is %d", sockfd);
	if (sockfd < 0) 
        perror("error creating socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port =htons(portnum);
	//printf("before bind");
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)		//binding socket
	perror("binding error");
	//printf("\nafter bind");
	
    int w;
	for(w=0;w<threadnum;w++)
	{
		pthread_create(&t_serve[w],NULL,&thread_serve,NULL); 
    }

	ids=sockfd;
	//printf("\n before creating scheduler thread");
	//printf("\nbefore creating thread sockfd is %d",ids);
	pthread_create(&t_listener,NULL,&thread_listen,&ids);			//creating listener thread
	sleep(sleep_time);							// putting scheduler to sleep
	pthread_create(&t_scheduler,NULL,&thread_scheduler,&sched_flag);	//creating scheduler thread
    pthread_join(t_listener,NULL);
	pthread_join(t_scheduler,NULL);
	//printf("\nafter join in main");
	display();
	close(sockfd);
	return 0;
}

