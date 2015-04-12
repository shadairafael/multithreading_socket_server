

sem_t sem;
int free_thread;
int sched_flag=0,debug_flag=0,log_flag=0;
char * file=NULL;
pthread_t t_serve;
pthread_mutex_t qmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sthread_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
struct request
{
	int acceptfd;
	int size;
	char file_name[1024];
	unsigned int cli_ipaddr;
	char time_arrival[1024];
	char in_buf[2048];
	
}r2; 
// queue function declarations;
void insertion(int,char*, int, unsigned int,char*,char*);	
//void insertion(int,string, int);
struct request extract_element();
struct request removesjf();
void display();
void print_help_options();

//queue structre
struct node
{
	struct request r;	
	struct node *link;
}*new,*temp,*p,*front=NULL,*rear=NULL;
typedef struct node N;

void display()
{
	if(front==NULL)
		printf("\nempty queue");
	else
	{
		int a;
		
		temp=front;
		while(temp!=NULL)
		{
			a=(temp->r.acceptfd);			
			printf("\n acceptfd is %d, file name is %s, file size is %d , ip addr is %u, request is %s,time is %s",a,temp->r.file_name,temp->r.size,temp->r.cli_ipaddr,temp->r.in_buf,temp->r.time_arrival);
			temp=temp->link;
		}
	}
} 

// queue functions
void insertion(int afd,char *f,int size,unsigned int ip, char * time_arrival,char * in_buf)
{
	new=(N*)malloc(sizeof(N));
	int n;
	char a[1024];
	char b[1024];
	char c[1024];
	strcpy(a,f);
	strcpy(b,time_arrival);
	strcpy(c,in_buf);
	new->r.acceptfd=afd;
	strcpy(new->r.file_name,a);
	new->r.cli_ipaddr=ip;
	strcpy(new->r.time_arrival,b);
	strcpy(new->r.in_buf,c);
	
	//new->r.file_name=a;
	new->r.size=size;
	new->link=NULL;
	if(front==NULL)
		front=new;
	else
		rear->link=new;
		rear=new;
	//printf("\n inserted request into queue");
	display();	
		
}

 struct request extract_element()
{

	if(front==NULL)
		printf("\nempty queue");
	else
	{	
		struct request r1;
		p=front;
		//printf("\n element is : %d",p->r.acceptfd);
		front=front->link;
		r1.acceptfd=p->r.acceptfd;
		strcpy(r1.file_name,p->r.file_name);		
		r1.size=p->r.size;
		free(p);
		return(r1);
	}
}
 

struct request removesjf(int num)
{ 
	printf("\nentered removesjf");
	if(front==NULL)
	{
		 printf("\n\nempty list");
	}
	else
	{
		struct node *old,*temp;
		temp=front;
		while(temp!=NULL)
		{ 
			if(temp->r.acceptfd==num)
			{ 
				if(temp==front) 
				front=temp->link; 
				else 
				old->link=temp->link;
				return(temp->r);
				free(temp);
				
			}
			else
			{ 
				old=temp;
				temp=temp->link;
			}
		}
	}
}

// end of queue functions


// thread pool function

void *thread_serve()
{
	while(1)
	{
		//printf("\nentered serving thread\n");		
		pthread_mutex_lock(&sthread_mutex);		
		pthread_cond_wait(&cond_var,&sthread_mutex);
		//printf("\ngot signal\n");
		//wait on condition mutex
		struct request r=r2;
		pthread_mutex_unlock(&sthread_mutex);
		//printf("\n serving thread unlocked sthread_mutex");
		//printf("\n is acceptfd %d, file name is %s, file size is %d , ip addr is %u, request is %s,time is %s",a,temp->r.file_name,temp->r.size,temp->r.cli_ipaddr,temp->r.in_buf,temp->r.time_arrival);

		time_t now;					// getting the time the job has been assigned to the serving thread
                time(&now);
       	        struct tm * ct=localtime(&now); //getting localtime
       	        int ch[128], time_serve[128];
       	        struct timeval tv;
       	        strftime(ch, sizeof ch, "[%d/%b/%Y : %H:%M:%S %z]", ct); //format of the timestamp string we need
       	        snprintf(time_serve, sizeof time_serve, ch, tv.tv_usec); //printing the needed timestamp string
		
		unsigned int ip=r.cli_ipaddr;
  
    /* code adapted from stackoverflow.com */
    
		unsigned char bytes[4];
		bytes[0] = ip & 0xFF;
		bytes[1] = (ip >> 8) & 0xFF;
		bytes[2] = (ip >> 16) & 0xFF;
		bytes[3] = (ip >> 24) & 0xFF;
 /* end of code adapted from stackoverflow.com */
		
		//struct request r= *((struct request *)arg);
		if(debug_flag==0&& log_flag==1)
		{
			FILE * file_des=fopen(file,"a"); 
			//printf("\n in  serving thread \n");
			fprintf(file_des,"%d.%d.%d.%d\t-\t ", bytes[0], bytes[1], bytes[2], bytes[3]);
			fprintf(file_des,"%s\t %s\t %s \t status\t %d\n",r.time_arrival,time_serve,r.in_buf,r.size);
			fclose(file_des);
		}
              
		else if(debug_flag==1)
		{  
			printf("\n%d.%d.%d.%d\t-\t %s\t %s\t %s \t status\t %d\n", bytes[0], bytes[1], bytes[2], bytes[3],r.time_arrival,time_serve,r.in_buf,r.size);
			//fprintf(stdout,"%s\t %s\t %s \t status\t %d\n",r.time_arrival,time_serve,r.in_buf,r.size);
		}
		//printf("\n in  serving thread copied structure\n");

		char           in_buf[BUF_SIZE];      
		char           out_buf[BUF_SIZE];
		char           *file_name;                 
		file_name=malloc(sizeof(char *));
		int acceptfd;
		unsigned int   fd1;                         
		unsigned int   buffer_length;                    
		unsigned int   retcode;  
		int m; 

		//printf("\n in  serving thread before copying variables\n");
		acceptfd=r.acceptfd;
		file_name=r.file_name;
		//printf("\n in  serving thread after copying variables\n");
                
	 	//printf("\nin serving thread file name is %s\n",file_name);

		//printf("\nin serving thread opening file\n");
		
		/* This part of code adopted from http://kturley.com/simple-multi-threaded-web-server-written-in-c-using-pthreads/ */

        fd1 = open(&file_name[1], O_RDONLY, S_IREAD | S_IWRITE);
	
        memset(out_buf, 0, sizeof(out_buf));

       	if (fd1 == -1)
       	{
       		printf("File %s not found - sending an HTTP 404 \n", &file_name[1]);
       		strcpy(out_buf, NOTOK_404);
        	send(acceptfd, out_buf, strlen(out_buf), 0);
		    strcpy(out_buf, MESS_404);
			send(acceptfd, out_buf, strlen(out_buf), 0);
        }
	    else
	    {
	    	printf("File %s is being sent \n", &file_name[1]);
		    if ((strstr(file_name, ".jpg") != NULL)||(strstr(file_name, ".gif") != NULL))
		    {
		    	strcpy(out_buf, OK_IMAGE);
		    }
	        else
            {
	        	strcpy(out_buf, OK_TEXT);
            }
		    send(acceptfd, out_buf, strlen(out_buf), 0);
 
		    buffer_length = 1;
		    while (buffer_length > 0)
		    {
		    	buffer_length = read(fd1, out_buf, BUF_SIZE);
			    if (buffer_length > 0)
			    {
			    	send(acceptfd, out_buf, buffer_length, 0);
			        sem_post(&sem);
			    }
		    }
	    }

	 }
}

