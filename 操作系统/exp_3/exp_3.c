#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define BLOCK_SIZES 5 //缓冲区个数
#define BUFFER_SIZE 1024 //缓冲区大小

union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    };
union semun arg;
int semid;//信号量集合首地址
struct share_buffer
{
	int end;
    char buffer[BUFFER_SIZE];
	struct share_buffer *next;
};
struct share_buffer *start=NULL;

char *share[BLOCK_SIZES];//指向共享缓冲区
int pid1,pid2;//子进程标识符
int running=1;
int shmid[BLOCK_SIZES];//共享缓冲区标识符

/***对信号量数组index编号的信号量做P操作***/
void P(int semid, int index){
	struct sembuf sem={index,-1,0};
    semop(semid,&sem,1);
}

/***对信号量数组index编号的信号量做V操作***/
void V(int semid, int index){
    struct sembuf sem={index,+1,0};
    semop(semid,&sem,1);
}

void readbuf()
{
    int file_end;
	int times=0;
	const char* pathname;
	struct share_buffer *in=start;
	int fd;//文件描述符
	char pn[100]="/home/databrains/input.pdf";
	pathname=pn;
	if ((fd=open(pathname,O_RDONLY))==-1){
		printf("打开源文件失败\n");
		return;
	}
	else
		printf("打开源文件成功\n");
	while(running){
		P(semid,0);
		file_end=read(fd,in->buffer, BUFFER_SIZE-1);
		if(file_end!=BUFFER_SIZE-1&&file_end!=0){
			printf("最后一次从源文件读出%d字节\n",file_end);
			in->end=file_end;
			V(semid,1);
			return;
		}
		printf("第%d次从源文件读出\n",++times);
		in=in->next;
		V(semid,1);
	}
	exit(EXIT_SUCCESS);
}

void writebuf()
{
	int times=0;
	const char* pathname;
	struct share_buffer *out=start;
	int fd;//文件描述符
	char pn[100]="/home/databrains/output.pdf";
	pathname=pn;
	if ((fd=open(pathname,O_WRONLY|O_CREAT,S_IRWXU|S_IXGRP|S_IROTH|S_IXOTH))==-1){
		printf("打开目标文件失败");
		return;
	}
	else
		printf("打开目标文件成功\n");
	while(running){
		P(semid,1);
		write(fd,out->buffer, BUFFER_SIZE-1);
		printf("第%d次写入到目标文件\n",++times);
		out=out->next;
		V(semid,0);
		if(out->end!=0)//写到文件尾
		{
			printf("最后一次写入%d个字节到目标文件\n",out->end);
			write(fd,out->buffer,out->end);
		 	return ;
		}
	}
	exit(EXIT_SUCCESS);
}
int main()
{
	int ret;
	int i;
	void* shm=NULL;
	key_t key=1234,key1;
	struct share_buffer *head=NULL,*tail=NULL;
	int size=sizeof(struct share_buffer);
	/***创建共享内存组***/
	for(i=0;i<BLOCK_SIZES;i++){
		shmid[i]=shmget(key,size,0666|IPC_CREAT);
		if (shmid[i]==-1){
		    fprintf(stderr, "shmget fail\n");
		    exit(EXIT_FAILURE);
		}
		key++;
		//将共享内存连接到当前进程的地址空间
		shm=shmat(shmid[i],(void*)0,0);
		if(shm==(void*)-1)fprintf(stderr, "shmat fail\n");
		//设置共享内存
		head=(struct share_buffer *)shm;
		head->end=0;
		head->next=tail;
		if(i==0)start=head;
		tail=head;
	}
	start->next=head;
	/***获得键值***/
	key1=ftok("/tmp", 0x66 ) ;
    if(key<0){
        perror("ftok key error") ;
        return -1 ;
    }
    /***创建两个信号量***/
    semid=semget(key,2,IPC_CREAT|0666);
    if(semid==-1){
         perror("create semget error");
         return ;
    }
    /***对两个信号量赋初值***/
	arg.val=BLOCK_SIZES;
    ret=semctl(semid,0,SETVAL,arg);//信号量semid【0】为1
	arg.val=0;
	ret =semctl(semid,1,SETVAL,arg);//信号量semid【1】为0
    if (ret < 0 ){
        perror("ctl sem error");
        semctl(semid,0,IPC_RMID,arg);
        return -1 ;
    }
	/***创建子进程***/
	 if((pid1=fork())<0){
            perror("Fail to fork");
            exit(EXIT_FAILURE);
        }
	else if(pid1==0)readbuf();
 	else {
		if((pid2=fork())<0){
			perror("Fail to fork");
			exit(EXIT_FAILURE);
		}
		else if(pid2==0)writebuf();
		else
		{
			/***等待子进程结束***/
			waitpid(pid1,NULL,0);
			waitpid(pid2,NULL,0);
			printf("子进程已结束\n");
			/***删除信号量***/
			semctl(semid,0,IPC_RMID,arg);
			semctl(semid,1,IPC_RMID,arg);
			printf("信号量删除成功\n");
			/***删除缓冲区***/
			for(i=0;i<BLOCK_SIZES;i++)
				if (shmctl(shmid[i],IPC_RMID,0)==-1){
				    fprintf(stderr, "shmctl(IPC_RMID) failed\n");
				    exit(EXIT_FAILURE);
				}
			printf("缓冲区删除成功\n");
			printf("父进程结束\n");
		}
	}
	return 0;
}
