//Harsh Daryani 2018B1A70645H
//Virag Lakhani 2018B3A70973H
//Jui Pradhan 2018B3A70984H
//Deepanshu Mody 2018B1A70949H
//Rohit Sethi 2018B4A70611H
//Saaketh Reddy Vaddi 2019A7PS1215H
//Rohan Sachan 2018B3A70992H
//Pranav Khabale 2018B1A70794H
//Aaryan Gupta 2018B1A70775H
//Group No. 45

#include<sys/types.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>
#include<errno.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include <stdio.h>
#include <sys/mman.h>
#define READ 0
#define WRITE 1
char er[14];

# define SIZE 100

//Below is the queue code we have used for round robin scheduling
void enqueue();
int dequeue();
int inp_arr[SIZE];
int Rear = - 1;
int Front = - 1;


void enqueue(int insert_item)
{
    
    if (Rear == SIZE - 1)
      printf("Overflow \n");
    else
    {
        if (Front == - 1)
       
        Front = 0;
        Rear = Rear + 1;
        inp_arr[Rear] = insert_item;
    }
} 
  
int dequeue()
{
    if (Front == - 1 || Front > Rear)
    {
        //printf("Underflow \n");
        return -1;
    }
    else
    {
        int a=inp_arr[Front];
        Front = Front + 1;
        return a;
    }
} 

int IsEmpty()
{
     
    if (Front == - 1 && Front > Rear)
    	return 0;
    else
    	return 0;
}   

long long int sum_c1=0;
long long int sum_c3=0;
long long int n1,n2,n3;//workload

//mutex initialisation
pthread_mutex_t mutex_c1;
pthread_mutex_t mutex_c2;
pthread_mutex_t mutex_c3;

double a,b,c;

FILE *fptr;//file

long long int arr[1000000];//array for c1

//Here the monitor thread has a pointer to the shared memory variable(so changes there will be reflected here) throug which is decides whether to sleep or awake the task thread.

//task thread of c1
void *add(void *param)
{
	pthread_mutex_lock(&mutex_c1);
	long long int *addr=(long long int *)param;
  	for(long long int i=0;i<=*addr;i++)
     		sum_c1+=i;
     	pthread_mutex_unlock(&mutex_c1);
	pthread_exit(0);
}


//monitor thread of c1
void *c1_monitor(void *param)
{
	char *addr=(char *)param;
	pthread_mutex_lock(&mutex_c1);
	//printf("%c",*addr);
  	while(1)
  	{
    		if(*addr == '0')
    		{
 
      			break;
      		}
  	}
  	pthread_mutex_unlock(&mutex_c1);	
	pthread_exit(0);
}

//task thread of c2
void *read_c2(void *param)
{
	pthread_mutex_lock(&mutex_c2);
       int num;
       long long int* upper=(long long int*) param;
       long long int lim=*upper;
    	if ((fptr = fopen("add.txt", "r")) == NULL)
    	{
        	printf("Error! opening file");
		exit(1);
   	 }
    	for (long long int i = 0; i <lim; i++)
    	{
        	fscanf(fptr, "%d", &num);
        	printf("%d\n",num);
    	}
	fclose(fptr);
	pthread_mutex_unlock(&mutex_c2);
	pthread_exit(0);
}

//monitor thread of c2
void *c2_monitor(void *param)
{
	char *addr=(char *)param;
	pthread_mutex_lock(&mutex_c2);
	while(1)
  	{
    		if(*addr == '1')
      		break;
  	}
  	pthread_mutex_unlock(&mutex_c2);	
	pthread_exit(0);
}

//task thread of c3
void *add_c3(void *param)
{
	pthread_mutex_lock(&mutex_c3);
       int num;
       long long int* upper=(long long int*) param;
       long long int lim=*upper;
    	if ((fptr = fopen("add.txt", "r")) == NULL)
    	{
        	printf("Error! opening file");
		exit(1);
   	 }
    	for (long long int i = 0; i <lim; i++)
    	{
        	fscanf(fptr, "%d", &num);
        	sum_c3 += num;
    	}
	fclose(fptr);
	pthread_mutex_unlock(&mutex_c3);
	pthread_exit(0);
}

//monitor thread of c3
void *c3_monitor(void *param)
{
	char *addr=(char *)param;
	pthread_mutex_lock(&mutex_c3);
  	while(1)
  	{
    		if(*addr=='2')
      		break;
  	}
  	pthread_mutex_unlock(&mutex_c3);	
	pthread_exit(0);
}

//Shared memory to be used for time analysis
void* getSharedMem(size_t size)
{
    int prot = PROT_READ | PROT_WRITE;
    int vis = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, prot, vis, -1, 0);
}
double *ti;

int main()
{
	
	printf("Enter 0 for FCFS and 1 for Round Robin : ");
	//shared memory
	int shmid;
	char *shmptr;
	int schedule;// fcfs or RR
	scanf("%d",&schedule);
	if(schedule==0)
	{	
		//fcfs scheduling
		ti=getSharedMem(sizeof(double)*6);//shared memory double array to pass time from child to parent
 		printf("Enter value of n1:");
  		scanf("%lld",&n1);
		printf("Enter value of n2:");
		scanf("%lld",&n2);
		printf("Enter value of n3:");
		scanf("%lld",&n3);
		
		printf("\nEnter 0 to give n1 numbers as input or to add from 1 to n1 in c1\n");
		int po;
		scanf("%d",&po);
		if(po==0)
		{
			printf("\nInput %lld numbers to add in C1\n",n1);
		
  			for(int i=0;i<n1;i++)
   				scanf("%lld",&arr[i]);
   		}
   		else
   		{
   			for(int i=0;i<n1;i++)
   				arr[i]=i;
   		}	
   		printf("\n");
   		
		pid_t pid1;
		//3 pipes for communicating result of 3 childs to parent
		int fd1[2];
		int fd2[2];
		int fd3[2];
		if(pipe(fd1)<0)
        	{
         	     perror("pipe failed");
         	     exit(1);
        	}
        	if(pipe(fd2)<0)
        	{
         	     perror("pipe failed");
         	     exit(1);
        	}
        	if(pipe(fd3)<0)
        	{
         	     perror("pipe failed");
         	     exit(1);
        	}
        	
        	//first fork to create c1
		pid1 = fork();
		//recording arrival time of c1
		struct timespec arrive_c1={0,0};
    		clock_gettime(CLOCK_MONOTONIC, &arrive_c1);
		if (pid1 < 0) 
		{ 
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid1 == 0) 
		{	//child c1
			sleep(2);
			//attaching to shared memory
			if((shmid=shmget(2051, 32, 0))==-1)
			{
				printf("error in read c1");
				exit(1);
			}
			shmptr=shmat(shmid,0,0);
			if(shmptr==(char *)-1)
			{
				printf("error in shmptr in c1");
				exit(1);
			}
			
			close(fd1[READ]);
 			long long int *nl=&n1; 
  			pthread_t tid[2];
  			pthread_attr_t at[2];
  			pthread_attr_init(&at[0]);
  			pthread_attr_init(&at[1]);
  			//monitor thread of c1
  			if((pthread_create(&tid[0],&at[0],c1_monitor,shmptr))==-1)
  			{
  				printf("asdf");
  			}
  			//task thread of c1
  			if((pthread_create(&tid[1],&at[1],add,nl))==-1)
  			{
  				printf("asdf");
  			}
			pthread_join(tid[0],NULL);
			pthread_join(tid[1],NULL);
			
			//writing on pipe
  			write(fd1[WRITE],&sum_c1,sizeof(sum_c1));
   			close(fd1[WRITE]);
   			
   			//recording time
   			struct timespec finish_c1={0,0};
			clock_gettime(CLOCK_MONOTONIC, &finish_c1);
			ti[0]=((double)finish_c1.tv_sec  + 1.0e-9*finish_c1.tv_nsec);
			
			//detach from shared memory
			shmdt(shmptr);
		}
		else 
		{ 
			pid_t pid2;
			//second fork to create c2
			pid2 = fork();
			//recording arrival time of c2
			struct timespec arrive_c2={0,0};
    			clock_gettime(CLOCK_MONOTONIC, &arrive_c2);
    			
			if (pid2 < 0) 
			{ 
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if (pid2 == 0) 
			{
				//child c2
				sleep(2);
				//attaching to shared memory
				if((shmid=shmget(2051, 32, 0))==-1)
				{
					printf("error in read c1");
					exit(1);
				}
				shmptr=shmat(shmid,0,0);
				if(shmptr==(char *)-1)
				{
					printf("error in shmptr in c1");
					exit(1);
				}
				
				close(fd2[READ]);
 				long long int *nl=&n2; 
  				pthread_t tid2[2];
  				pthread_attr_t at2[2];
  				pthread_attr_init(&at2[0]);
  				pthread_attr_init(&at2[1]);
  				//monitor thread of c2
  				if((pthread_create(&tid2[0],&at2[0],c2_monitor,shmptr))==-1)
  				{
  					printf("asdf");
  				}
  				//task thread of c2
  				if((pthread_create(&tid2[1],&at2[1],read_c2,nl))==-1)
  				{
  					printf("asdf");
  				}
				pthread_join(tid2[0],NULL);
				pthread_join(tid2[1],NULL);
  
				//writing on pipe
   				write(fd2[WRITE],"Done Printing",14);
   				close(fd2[WRITE]);
			}
			else 
			{ 
				pid_t pid3;
				//third fork to create c3
				pid3 = fork();
				
				//recording arrival time of c3
				struct timespec arrive_c3={0,0};
    				clock_gettime(CLOCK_MONOTONIC, &arrive_c3);
				
				if (pid3 < 0) 
				{ 
					fprintf(stderr, "Fork Failed");
					return 1;
				}
				else if (pid3 == 0) 
				{
					//child c3
					sleep(2);
					//attaching to shared memory
					if((shmid=shmget(2051, 32, 0))==-1)
					{
						printf("error in read c1");
						exit(1);
					}
					shmptr=shmat(shmid,0,0);
					if(shmptr==(char *)-1)
					{
						printf("error in shmptr in c1");
						exit(1);
					}
					
					close(fd3[READ]);
 					long long int *nl=&n3; 
  					pthread_t tid3[2];
  					pthread_attr_t at3[2];
  					pthread_attr_init(&at3[0]);
  					pthread_attr_init(&at3[1]);
  					//monitor thread of c2
  					if((pthread_create(&tid3[0],&at3[0],c3_monitor,shmptr))==-1)
  					{
  						printf("asdf");
  					}
  					//task thread of c2
  					if((pthread_create(&tid3[1],&at3[1],add_c3,nl))==-1)
  					{
  						printf("asdf");
  					}
					pthread_join(tid3[0],NULL);
					pthread_join(tid3[1],NULL);
					
					//writing on pipe
   					write(fd3[WRITE],&sum_c3,sizeof(sum_c3));
   					close(fd3[WRITE]);
				}
				else 
				{ 
					//parent process
					
					//Creating shared memory
					if((shmid=shmget(2051, 32, 0666 | IPC_CREAT))<0)
					{
						printf("error in shared memory");
						exit(0);
					}
					shmptr=shmat(shmid,0,0);
					if(shmptr==(char*)-1)
					{
						printf("error in shmptr");
						exit(1);
					}
					shmptr[0]='0'+0;
					
					//recording time of start of execution of c1
					struct timespec a_c1={0,0};
					clock_gettime(CLOCK_MONOTONIC, &a_c1);
					
					//reading from c1
					close(fd1[WRITE]);
  					long long int sum_c1;
 					read(fd1[READ],&sum_c1,sizeof(sum_c1));
 					close(fd1[READ]);
					wait(NULL);
					
					//recording time for end of execution of c1
					struct timespec finish_c1={0,0};
					clock_gettime(CLOCK_MONOTONIC, &finish_c1);
					
					//printing result of c1
					printf("\nsum is %lld\n",sum_c1);
					//printing time taken by c1
					printf("\ntime required for C1 %.5f sec\n",ti[0] - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec)-2);
					a=ti[0] - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec)-2;
					
					//FCFS scheduling using shared memory
					shmptr[0]='1'+0;
					
					//reading from c2
					close(fd2[WRITE]);
  					char buf[14];
 					read(fd2[READ],buf,14);
 					
 					//printing result of c1
 					printf("\n%s\n",buf);
 					close(fd2[READ]);
					wait(NULL);
					
					//recording time for end of execution of c2
					struct timespec finish_c2={0,0};
					clock_gettime(CLOCK_MONOTONIC, &finish_c2);
					
					//printing time taken by c2
					printf("\ntime required for C2 %.5f sec\n",((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ti[0]);
					b=((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ti[0];
  					
  					//FCFS scheduling using shared memory
  					shmptr[0]='2'+0;
  					
  					//reading from c3
  					close(fd3[WRITE]);
  					long long int sum_c3;
 
 					read(fd3[READ],&sum_c3,sizeof(sum_c3));
 					close(fd3[READ]);
 					
 					//printing result of c1
 					printf("sum is %lld ",sum_c3);
 					wait(NULL);
 					
 					//recording time for end of execution of c2
 					struct timespec finish_c3={0,0};
					clock_gettime(CLOCK_MONOTONIC, &finish_c3);
					
					//printing time taken by c3
					printf("\ntime required for C3 %.5f sec\n",((double)finish_c3.tv_sec  + 1.0e-9*finish_c3.tv_nsec) - ((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec));
 					
 					// Detaching and Removing a Shared Memory Segment
 					shmctl(shmid,IPC_RMID,NULL);
 					c=((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ti[0];
 					
 					//printing fcfs scheduling using shared memory time from child
 					printf("\nC1 starts at %d sec\n",0);
 					printf("\nC2 starts at %.5f sec\n",ti[0] - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec)-2);
 					printf("\nC3 starts at %.5f sec\n",((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec)-2);
 					
 					//printing fcfs turn around time
 					printf("\nTurnaround time for c1 %.5fsec\n",ti[0] - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec));
 					printf("\nTurnaround time for c2 %.5fsec\n",((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ((double)arrive_c2.tv_sec  + 1.0e-9*arrive_c2.tv_nsec));
 					printf("\nTurnaround time for c3 %.5fsec\n",((double)finish_c3.tv_sec  + 1.0e-9*finish_c3.tv_nsec) - ((double)arrive_c3.tv_sec  + 1.0e-9*arrive_c3.tv_nsec));
 					
 					//printing waiting time
 					printf("\nWaiting time for c1 %.d sec\n",2);
 					printf("\nWaiting time for c2 %.5fsec\n",ti[0] - ((double)arrive_c2.tv_sec  + 1.0e-9*arrive_c2.tv_nsec));
 					printf("\nWaiting time for c3 %.5fsec\n",((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ((double)arrive_c3.tv_sec  + 1.0e-9*arrive_c3.tv_nsec));
 					
 					//avg trt and avg wt of fcfs
 					double q1,q2,q3,w1,w2,w3;
 					q1=ti[0] - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec);
 					q2=((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ((double)arrive_c2.tv_sec  + 1.0e-9*arrive_c2.tv_nsec);
 					q3=((double)finish_c3.tv_sec  + 1.0e-9*finish_c3.tv_nsec) - ((double)arrive_c3.tv_sec  + 1.0e-9*arrive_c3.tv_nsec);
 					w1=2;
 					w2=ti[0] - ((double)arrive_c2.tv_sec  + 1.0e-9*arrive_c2.tv_nsec);
 					w3=((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ((double)arrive_c3.tv_sec  + 1.0e-9*arrive_c3.tv_nsec);
 					
 					
 					double avg_wt = (w1+w2+w3)/3;  
					double avg_tat = (q1+q2+q3)/3;  
					printf("\n\n Average Turn Around Time: \t%lf\n\n", avg_wt);  
					printf("\n\n Average Waiting Time: \t%lf\n\n", avg_tat); 
				}
			}
		}
	}
	else
	{	
		//time quantum for round robin
		printf("\n Enter Time quantum in sec : ");
		double tq;
		scanf("%lf",&tq);
		ti=getSharedMem(sizeof(double)*6);
 		printf("Enter value of n1:");
  		scanf("%lld",&n1);
		printf("Enter value of n2:");
		scanf("%lld",&n2);
		printf("Enter value of n3:");
		scanf("%lld",&n3);
		
		printf("\nEnter 0 to give n1 numbers as input or to add from 1 to n1 in c1\n");
		int po;
		scanf("%d",&po);
		if(po==0)
		{
			printf("\nInput %lld numbers to add in C1\n",n1);
		
  			for(int i=0;i<n1;i++)
   				scanf("%lld",&arr[i]);
   		}
   		else
   		{
   			for(int i=0;i<n1;i++)
   				arr[i]=i;
   		}	
   		printf("\n");
   		
		pid_t pid1;
		//3 pipes for communicating result of 3 childs to parent
		int fd1[2];
		int fd2[2];
		int fd3[2];
		if(pipe(fd1)<0)
        	{
         	     perror("pipe failed");
         	     exit(1);
        	}
        	if(pipe(fd2)<0)
        	{
         	     perror("pipe failed");
         	     exit(1);
        	}
        	if(pipe(fd3)<0)
        	{
         	     perror("pipe failed");
         	     exit(1);
        	}
        	
        	//first fork to create c1
		pid1 = fork();
		//recording arrival time of c1
		struct timespec arrive_c1={0,0};
    		clock_gettime(CLOCK_MONOTONIC, &arrive_c1);
		if (pid1 < 0) 
		{ 
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid1 == 0) 
		{	//child c1
			sleep(2);
			//attaching to shared memory
			if((shmid=shmget(2051, 32, 0))==-1)
			{
				printf("error in read c1");
				exit(1);
			}
			shmptr=shmat(shmid,0,0);
			if(shmptr==(char *)-1)
			{
				printf("error in shmptr in c1");
				exit(1);
			}
			
			close(fd1[READ]);
 			long long int *nl=&n1; 
  			pthread_t tid[2];
  			pthread_attr_t at[2];
  			pthread_attr_init(&at[0]);
  			pthread_attr_init(&at[1]);
  			//monitor thread of c1
  			if((pthread_create(&tid[0],&at[0],c1_monitor,shmptr))==-1)
  			{
  				printf("asdf");
  			}
  			//task thread of c1
  			if((pthread_create(&tid[1],&at[1],add,nl))==-1)
  			{
  				printf("asdf");
  			}
			pthread_join(tid[0],NULL);
			pthread_join(tid[1],NULL);
			
			//writing on pipe
  			write(fd1[WRITE],&sum_c1,sizeof(sum_c1));
   			close(fd1[WRITE]);
   			
   			//recording time
   			struct timespec finish_c1={0,0};
			clock_gettime(CLOCK_MONOTONIC, &finish_c1);
			ti[0]=((double)finish_c1.tv_sec  + 1.0e-9*finish_c1.tv_nsec);
			
			//detach from shared memory
			shmdt(shmptr);
		}
		else 
		{ 
			pid_t pid2;
			//second fork to create c2
			pid2 = fork();
			//recording arrival time of c2
			struct timespec arrive_c2={0,0};
    			clock_gettime(CLOCK_MONOTONIC, &arrive_c2);
    			
			if (pid2 < 0) 
			{ 
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if (pid2 == 0) 
			{
				//child c2
				sleep(2);
				//attaching to shared memory
				if((shmid=shmget(2051, 32, 0))==-1)
				{
					printf("error in read c1");
					exit(1);
				}
				shmptr=shmat(shmid,0,0);
				if(shmptr==(char *)-1)
				{
					printf("error in shmptr in c1");
					exit(1);
				}
				
				close(fd2[READ]);
 				long long int *nl=&n2; 
  				pthread_t tid2[2];
  				pthread_attr_t at2[2];
  				pthread_attr_init(&at2[0]);
  				pthread_attr_init(&at2[1]);
  				//monitor thread of c2
  				if((pthread_create(&tid2[0],&at2[0],c2_monitor,shmptr))==-1)
  				{
  					printf("asdf");
  				}
  				//task thread of c2
  				if((pthread_create(&tid2[1],&at2[1],read_c2,nl))==-1)
  				{
  					printf("asdf");
  				}
				pthread_join(tid2[0],NULL);
				pthread_join(tid2[1],NULL);
  
				//writing on pipe
   				write(fd2[WRITE],"Done Printing",14);
   				close(fd2[WRITE]);
			}
			else 
			{ 
				pid_t pid3;
				//third fork to create c3
				pid3 = fork();
				
				//recording arrival time of c3
				struct timespec arrive_c3={0,0};
    				clock_gettime(CLOCK_MONOTONIC, &arrive_c3);
				
				if (pid3 < 0) 
				{ 
					fprintf(stderr, "Fork Failed");
					return 1;
				}
				else if (pid3 == 0) 
				{
					//child c3
					sleep(2);
					//attaching to shared memory
					if((shmid=shmget(2051, 32, 0))==-1)
					{
						printf("error in read c1");
						exit(1);
					}
					shmptr=shmat(shmid,0,0);
					if(shmptr==(char *)-1)
					{
						printf("error in shmptr in c1");
						exit(1);
					}
					
					close(fd3[READ]);
 					long long int *nl=&n3; 
  					pthread_t tid3[2];
  					pthread_attr_t at3[2];
  					pthread_attr_init(&at3[0]);
  					pthread_attr_init(&at3[1]);
  					//monitor thread of c2
  					if((pthread_create(&tid3[0],&at3[0],c3_monitor,shmptr))==-1)
  					{
  						printf("asdf");
  					}
  					//task thread of c2
  					if((pthread_create(&tid3[1],&at3[1],add_c3,nl))==-1)
  					{
  						printf("asdf");
  					}
					pthread_join(tid3[0],NULL);
					pthread_join(tid3[1],NULL);
					
					//writing on pipe
   					write(fd3[WRITE],&sum_c3,sizeof(sum_c3));
   					close(fd3[WRITE]);
				}
				else 
				{ 
					//parent process
					
					//Creating shared memory
					if((shmid=shmget(2051, 32, 0666 | IPC_CREAT))<0)
					{
						printf("error in shared memory");
						exit(0);
					}
					shmptr=shmat(shmid,0,0);
					if(shmptr==(char*)-1)
					{
						printf("error in shmptr");
						exit(1);
					}
					shmptr[0]='0'+0;
					
					//recording time of start of execution of c1
					struct timespec a_c1={0,0};
					clock_gettime(CLOCK_MONOTONIC, &a_c1);
					
					//reading from c1
					close(fd1[WRITE]);
  					long long int sum_c1;
 					read(fd1[READ],&sum_c1,sizeof(sum_c1));
 					close(fd1[READ]);
					wait(NULL);
					
					//recording time for end of execution of c1
					struct timespec finish_c1={0,0};
					clock_gettime(CLOCK_MONOTONIC, &finish_c1);
					
					//printing result of c1
					printf("\nsum is %lld\n",sum_c1);
					//printing time taken by c1
					printf("\ntime required for C1 %.5f sec\n",ti[0] - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec)-2);
					a=ti[0] - ((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec)-2;
					
					//FCFS scheduling using shared memory
					shmptr[0]='1'+0;
					
					//reading from c2
					close(fd2[WRITE]);
  					char buf[14];
 					read(fd2[READ],buf,14);
 					
 					//printing result of c1
 					printf("\n%s\n",buf);
 					close(fd2[READ]);
					wait(NULL);
					
					//recording time for end of execution of c2
					struct timespec finish_c2={0,0};
					clock_gettime(CLOCK_MONOTONIC, &finish_c2);
					
					//printing time taken by c2
					printf("\ntime required for C2 %.5f sec\n",((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ti[0]);
					b=((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ti[0];
  					
  					//FCFS scheduling using shared memory
  					shmptr[0]='2'+0;
  					
  					//reading from c3
  					close(fd3[WRITE]);
  					long long int sum_c3;
 
 					read(fd3[READ],&sum_c3,sizeof(sum_c3));
 					close(fd3[READ]);
 					
 					//printing result of c1
 					printf("sum is %lld ",sum_c3);
 					wait(NULL);
 					
 					//recording time for end of execution of c2
 					struct timespec finish_c3={0,0};
					clock_gettime(CLOCK_MONOTONIC, &finish_c3);
					
					//printing time taken by c3
					printf("\ntime required for C3 %.5f sec\n",((double)finish_c3.tv_sec  + 1.0e-9*finish_c3.tv_nsec) - ((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec));
 					
 					// Detaching and Removing a Shared Memory Segment
 					shmctl(shmid,IPC_RMID,NULL);
 					c=((double)finish_c2.tv_sec  + 1.0e-9*finish_c2.tv_nsec) - ti[0];
 					
 					//round robin scheduling using queue data structure
 					enqueue(0);
					enqueue(1);
					enqueue(2);
					int y1=0,l=0,z=0;
 					while(IsEmpty())
					{
						
						int u;
						u=dequeue();
						
						if(u==0)
						{
							shmptr[0]='0'+0;
							struct timespec s_c1={0,0};
							clock_gettime(CLOCK_MONOTONIC, &s_c1);
							
						}	
						else if(u==1)
						{
							shmptr[0]='1'+0;
							struct timespec s_c2={0,0};
							clock_gettime(CLOCK_MONOTONIC, &s_c2);
							
						}	
						else if(u==2)
						{
							shmptr[0]='2'+0;
							struct timespec s_c3={0,0};
							clock_gettime(CLOCK_MONOTONIC, &s_c3);
							
						}
							
							
						sleep(tq);
						
						if(u==0)
						{
							wait(NULL);
							close(fd1[WRITE]);
  							long long int sum_c1=-1;
 							read(fd1[READ],&sum_c1,sizeof(sum_c1));
 							close(fd1[READ]);
 							a=sum_c1;
 							
 							if(sum_c1==-1)
 								{enqueue(0);y1++;}
 								
 						}
 						else if(u==1)
 						{
 							wait(NULL);
 							close(fd2[WRITE]);
  							char buf[14];
 							read(fd2[READ],buf,14);
 							
 							strncpy(er,buf,14);
 							close(fd2[READ]);
 							
 							if(buf[0]=='\0')
 								{enqueue(1);l++;}
 						}
 						else if(u==2)
 						{
 							
 							wait(NULL);
 							close(fd3[WRITE]);
  							long long int sum_c3=-1;
 							read(fd3[READ],&sum_c3,sizeof(sum_c3));
 							close(fd3[READ]);
 							b=sum_c3;
 							if(sum_c3==-1)
 								{enqueue(2);z++;}
 						}
						
					}	
 					
 					double q[100000],w[100000],e[100000];
 					int ii=0,j=0,k=0;
 					double aa=a;	
 					while(aa>0)
 					{
 						if(aa>tq)
 						{	
 							q[ii]=tq;
 							ii++;
 							aa=aa-tq;
 						}
 						else
 						{
 							q[ii]=aa;
 							aa=0;
 						}
 					}
 					
 					double bb=b;	
 					while(bb>0)
 					{
 						if(bb>tq)
 						{	
 							w[j]=tq;
 							j++;
 							bb=bb-tq;
 						}
 						else
 						{
 							w[j]=bb;
 							bb=0;
 						}
 					}
 					
 					double cc=c;	
 					while(cc>0)
 					{
 						if(cc>tq)
 						{	
 							e[k]=tq;
 							k++;
 							cc=cc-tq;
 						}
 						else
 						{
 							e[k]=cc;
 							cc=0;
 						}
 					}
 					
 					ii=ii+1;j=j+1;k=k+1;
 					int as=ii+j+k;
 					double wer[as];
 					int ia=0,ib=0,ic=0,id=0;
 					while(ib<ii && ic<j && id<k)
 					{
 						wer[ia]=q[ib];
 						ib++;
 						ia++;
 						wer[ia]=w[ic];
 						ic++;
 						ia++;
 						wer[ia]=e[id];
 						id++;
 						ia++;
 						
 					}
 					while(ib<ii && ic<j)
 					{
 						wer[ia]=q[ib];
 						ib++;
 						ia++;
 						wer[ia]=w[ic];
 						ic++;
 						ia++;
 						
 					}
 					while( ic<j && id<k)
 					{
 						
 						wer[ia]=w[ic];
 						ic++;
 						ia++;
 						wer[ia]=e[id];
 						id++;
 						ia++;
 						
 					}
 					while(ib<ii  && id<k)
 					{
 						wer[ia]=q[ib];
 						ib++;
 						ia++;
 						
 						wer[ia]=e[id];
 						id++;
 						ia++;
 						
 					}
 					while(ib<ii )
 					{
 						wer[ia]=q[ib];
 						ib++;
 						ia++;
 						
 						
 					}
 					while(ic<j )
 					{
 						
 						wer[ia]=w[ic];
 						ic++;
 						ia++;
 						
 						
 					}
 					while(id<k)
 					{
 						
 						wer[ia]=e[id];
 						id++;
 						ia++;
 						
 					}
 					
 					double sumq[as];
 					sumq[0]=0;
 					sumq[1]=wer[0];
 					for(int p=0;p<as;p++)
 					{
 						sumq[p+2]=sumq[p+1]+wer[p+1];
 					}
 					
 					
 					//Printing scheduling of process by round robin
 					ia=0;ib=0;ic=0;id=0;
 					while(ib<ii && ic<j && id<k)
 					{
 						printf("c1 starts at %lf\n",sumq[ia]);
 						ia++;
 						ib++;
 						printf("c2 starts at %lf\n",sumq[ia]);
 						ia++;
 						ic++;
 						printf("c3 starts at %lf\n",sumq[ia]);
 						ia++;
 						id++;
 						
 					}
 					while(ib<ii && ic<j)
 					{
 						printf("c1 starts at %lf\n",sumq[ia]);
 						ia++;
 						ib++;
 						printf("c2 starts at %lf\n",sumq[ia]);
 						ia++;
 						ic++;
 						
 					}
 					while( ic<j && id<k)
 					{
 						printf("c2 starts at %lf\n",sumq[ia]);
 						ia++;
 						ic++;
 						printf("c3 starts at %lf\n",sumq[ia]);
 						ia++;
 						id++;
 						
 					}
 					while(ib<ii  && id<k)
 					{
 						printf("c1 starts at %lf\n",sumq[ia]);
 						ia++;
 						ib++;
 						printf("c3 starts at %lf\n",sumq[ia]);
 						ia++;
 						id++;
 					}
 					if(ib<ii )
 					{
 						
 						printf("c1 starts at %lf\n",sumq[ia]);
 						ia++;
 						ib++;
 					}
 					if(ic<j )
 					{
 						
 						printf("c2 starts at %lf\n",sumq[ia]);
 						ia++;
 						ic++;
 					}
 					if(id<k)
 					{
 						
 						printf("c2 starts at %lf\n",sumq[ia]);
 						ia++;
 						id++;
 					}
 					
 					//time analysis in round robin
 					double z1,z2,z3;
 					
 					z1=0;
 					z2=((double)arrive_c2.tv_sec  + 1.0e-9*arrive_c2.tv_nsec)-((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec);
 					z3=((double)arrive_c3.tv_sec  + 1.0e-9*arrive_c3.tv_nsec)-((double)a_c1.tv_sec  + 1.0e-9*a_c1.tv_nsec);
 					
    
 					int i;  
    					double  NOP, sum=0,count=0, y, wt=0, tat=0, at[10], bt[10], temp[10];  
    					double avg_wt, avg_tat;   
    					y = 3;
  					NOP=3;
					at[0]=z1;at[1]=z2;at[2]=z3;
					bt[0]=a;bt[1]=b;bt[2]=c;  
					for(i=0; i<NOP; i++)  
					{    
						temp[i] = bt[i]; 
					}  
					double quant = tq;
					
					//Printing turn around and waiting time for round robin scheduling of process
					printf("\n Process No \tTAT \t\t Waiting Time ");  
					for(sum=0, i = 0; y!=0; )  
					{  
						if(temp[i] <= quant && temp[i] > 0)   
						{  
    							sum = sum + temp[i];  
    							temp[i] = 0;  
    							count=1;  
    						}     
    						else if(temp[i] > 0)  
   						{  
        						temp[i] = temp[i] - quant;  
        						sum = sum + quant;    
   						}  
    							if(temp[i]==0 && count==1)  
    							{  
        							y--;  
        							printf("\nC%d \t\t %lf\t %lf", i+1, sum-at[i]+2, sum-at[i]-bt[i]+2);  
        							wt = wt+sum-at[i]-bt[i];  
        							tat = tat+sum-at[i];  
        							count =0;     
    							}  
    							if(i==NOP-1)  
    							{  
        							i=0;  
   							 }  
    							else if(at[i+1]<=sum)  
   							 {  
        							i++;  
   							 }  
    							else  
    								i=0;
    					}
					// represents the average waiting time and Turn Around time  
					avg_wt = wt * 1.0/NOP;  
					avg_tat = tat * 1.0/NOP;  
					printf("\n\n Average Turn Around Time: \t%f\n", avg_wt+2);  
					printf("\n\n Average Waiting Time: \t%f\n", avg_tat+2); 			
				}
			}
		}
	}
	return 0;
}
