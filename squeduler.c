//scheduler thread
void *thread_scheduler(void *arg)
{
	unsigned int schedalg=*((unsigned int*)arg);
	int acceptfd,n;
	if(schedalg==0)
	{	
		while(1)
		{
			if(front!=NULL)
			{
				sem_wait(&sem);		
				//printf("\nin sched thread before extracting element\n");
				//printf("\nscheduler locking mutex\n");		
				pthread_mutex_lock(&sthread_mutex);
				pthread_mutex_lock(&qmutex);
				r2=extract_element();
				//printf("\n popped element in scheduler thread");
				pthread_mutex_unlock(&qmutex);
				//printf("\nscheduler unlocked mutex\n");
				// call serving thread from thread pool
				//printf("\nin sched thread before sending to serving thread\n");
				pthread_cond_signal(&cond_var);
				free_thread--;
				pthread_mutex_unlock(&sthread_mutex);				
				//printf("\nin sched thread unlocked sthread mutex\n");
				//thread_serve(&r2);
				//pthread_create(&t_serve,NULL,&thread_serve,&r);	
				//thread_serve(&r);
			}
			else 
			{
				continue;
			}			
		}
	}
	else
	{
	//code for SJF scheduling algorithm
		//printf("\n entered SJF scheduling algorithm");
		int shortestjob_fd=0;
		int min;
		int a,b;
		while(1)			
		{
			pthread_mutex_lock(&qmutex);
			temp=front;
			if (temp==NULL)
			{
				
				continue;
			}
			else if(temp->link==NULL) 
			{
				//printf("\n only one");
				shortestjob_fd=temp->r.acceptfd;
			}
			else
			{
				min=temp->r.size;
				while(temp->link!=NULL)  //should modify
				{
					
					b=temp->link->r.size;
					if(min<=b)
					{
						shortestjob_fd=temp->r.acceptfd;
					}		
					else if(min>b)
					{			
						min=temp->link->r.size;						
						shortestjob_fd=temp->link->r.acceptfd;
					}
					printf("\n %d",a);
					temp=temp->link;
				}
			}
			pthread_mutex_lock(&sthread_mutex);
			
			r2=removesjf(shortestjob_fd);
			//printf("extracted element");
			pthread_cond_signal(&cond_var);
			pthread_mutex_unlock(&sthread_mutex);
			pthread_mutex_unlock(&qmutex);
		}
	
	}
	
}
