#include <unistd.h>
#include <stdlib.h>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include <sys/wait.h>
#include <sys/types.h>
int pid_1,pid_2;
int filedes_1[2],filedes_2[2];

void fun(int sig)
{
	kill(pid_1,SIGUSR1);
	kill(pid_2,SIGUSR1);
    waitpid(pid_1,NULL,0);
    waitpid(pid_2,NULL,0);
    close(filedes_1[0]);
    close(filedes_1[1]);
    close(filedes_2[0]);
    close(filedes_2[1]);
    printf("Parent Process is Killed!\n");
    exit(0);
}


void fun1(int sig)
{
	printf("\nChild1 process1  is  killed by parent!\n");
    close(filedes_1[0]);
    close(filedes_1[1]);
    exit(0);
}

void fun2(int sig)
{
	printf("\nChild2 process2  is  killed by parent!\n");
    close(filedes_2[0]);
    close(filedes_2[1]);
    exit(0);
}
int main(){
    pipe(filedes_1);
    pipe(filedes_2);

    char s[80];
    int x=0;
    pid_1=fork();
    if(pid_1>0){
        pid_2=fork();
        if(pid_2>0){
            signal(SIGINT,fun);
            while(1){
                x++;
                char buf[80];
                sprintf(buf, "I send you %d times", x);
                write(filedes_1[1],buf,sizeof(buf));
                write(filedes_2[1],buf,sizeof(buf));
                sleep(1);
            }
            return 0;
        }
        else{
            signal(SIGINT,SIG_IGN);
            signal(SIGUSR1,fun2);
            while(1){
                read(filedes_2[0],s,sizeof(s));
                printf("%s c2\n",s);
                sleep(1);
            }
        }
    }
    else{
        signal(SIGINT,SIG_IGN);
        signal(SIGUSR1,fun1);
        while(1){
            read(filedes_1[0],s,sizeof(s));
            printf("%s c1\n",s);
            sleep(1);
        }
    }
}
