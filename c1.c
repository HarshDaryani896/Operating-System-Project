#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>
#include<errno.h>
#define READ 0
#define WRITE 1
long val=0;
int n1;
void *add(void *param)
{
      int *addr=(int *)param;
  for(int i=0;i<=n1;i++)
     val+=addr[i];

 pthread_exit(0);
}
int main()
{

 pid_t child;
 int fd1[2];
 
  printf("Enter value of n1:");
  scanf("%d",&n1);
  
  int arr[10000];
  for(int i=0;i<n1;i++)
   scanf("%d",&arr[i]);

      if(pipe(fd1)<0)
       {
              perror("pipe failed");
              exit(1);
       }


if(( child=fork())<0)
{
 perror("fork faile");
 exit(2);
}

  if (child==0)
 {
   close(fd1[READ]);
 
   int *nl=&n1; 
  pthread_t tid[2];
  pthread_attr_t at[2];
  pthread_attr_init(&at[0]);
  pthread_create(&tid[0],&at[0],add,arr);

  pthread_join(tid[0],NULL);
  

   write(fd1[WRITE],&val,sizeof(val));
   close(fd1[WRITE]);

}
else
{

  
   
   close(fd1[WRITE]);
  long su;
 
 read(fd1[READ],&su,sizeof(su));
 close(fd1[READ]);
 printf("sum is %ld ",su);
  
 wait(NULL);
}



}
