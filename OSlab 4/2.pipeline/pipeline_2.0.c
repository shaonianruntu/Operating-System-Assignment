#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <wait.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define check_error(err)                                        \
    if(err < 0){                                                \
        fprintf(stderr, "Error : %s \n",strerror(errno) );      \
        exit(EXIT_FAILURE);                                     \
    }                                                           

#define MAXSIZE 100	
#define READ_NUM 50
#define WRITE_NUM 70




// int reader_process(int fd[2]);
// int writer_process(int fd[2]);
void write_to_pipe(int fd[2], int len);
void read_from_pipe(int fd[2], int len);
void initStr(char *text, int len);

// 将其简化为一个写者优先的读者-写者问题。
int main(){
    sem_t *wmutex = sem_open("wmutex", O_CREAT, 666, 1);    // 读者与写者之间、写者与写者之间互斥使用共享数据
    sem_t *S1 = sem_open("S1", O_CREAT, 666, 1);            // 当至少有一个写者准备访问共享数据时，它可使后续的读者等待写完成
    sem_t *S2 = sem_open("S2", O_CREAT, 666, 1);            // 阻塞第二个以后的等待读者
    int read_count = 0;         // 当前读者数量、写者数量
    int write_count = 0;
    sem_t *mutex1 = sem_open("mutex1", O_CREAT, 666, 1);    // 多个读者互斥使用 read_count
    sem_t *mutex2 = sem_open("mutex2", O_CREAT, 666, 1);    // 多个写者互斥使用 wirte_count

    pid_t pid[3];
    int fd[2];
    check_error(pipe(fd));

    int i;
    for(i = 0; i < 3; i++){
        pid[i] = fork();
        check_error(pid[i]);
        if(pid[i] == 0){
            break;
        }
    }

    if(pid[i] == 0){
        // writer_process(fd);
        sem_wait(mutex2);           // 申请访问 writecount
        if(write_count == 0)         
            sem_wait(S1);           // 写者进程进入等待队列
        write_count++;
        sem_post(mutex2);           // 释放 writecount
        sem_wait(wmutex);           // 是否有读者或者写者正在操作

        // 进行写操作
        printf("writing ... \n");
        write_to_pipe(fd, WRITE_NUM);

        sem_post(wmutex);
        sem_wait(mutex2);
        write_count--;
        if(write_count == 0)
            sem_post(S1);
        sem_post(mutex2);

        exit(0);
    }

    // wait(pid[0]);
    // wait(pid[1]);
    // wait(pid[2]);    
   wait(0);
//    wait(0);

// reader_process(fd);
    sem_wait(S2);               // 是否已经有读者进程在等待队列中
    sem_wait(S1);               // 是否有写者进程在等待队列中
    sem_wait(mutex1);           // 没有其他的读者进程在访问 readcount
    if(read_count == 0)         
        sem_wait(wmutex);       // 判断是否有写者进程在写
    read_count++;
    sem_post(mutex1);           // 释放 read_count
    sem_post(S1);
    sem_post(S2);   

    // 进行读操作
    printf("reading ... \n");
    read_from_pipe(fd, READ_NUM);

    sem_wait(mutex1);           
    read_count--;
    if(read_count == 0)         
        sem_post(wmutex);       // 允许写者进程写
    sem_post(mutex1);  

    // wait(0);
    // reader_process(fd);

    // wait(0);
    // reader_process(fd);
    sem_unlink("wmutex");
    sem_unlink("S1");
    sem_unlink("S2");
    sem_unlink("mutex1");
    sem_unlink("mutex2");
    return 0;
}

// // 读者进程
// int reader_process(int fd[2]){
//     sem_wait(S2);               // 是否已经有读者进程在等待队列中
//     sem_wait(S1);               // 是否有写者进程在等待队列中
//     sem_wait(mutex1);           // 没有其他的读者进程在访问 readcount
//     if(read_count == 0)         
//         sem_wait(wmutex);       // 判断是否有写者进程在写
//     read_count++;
//     sem_post(mutex1);           // 释放 read_count
//     sem_post(S1);
//     sem_post(S2);   

//     // 进行读操作
//     printf("reading ... \n");
//     read_from_pipe(fd, READ_NUM);

//     sem_wait(mutex1);           
//     read_count--;
//     if(read_count == 0)         
//         sem_post(wmutex);       // 允许写者进程写
//     sem_post(mutex1);  
// }

// // 写者进程
// int writer_process(int fd[2]){
//     sem_wait(mutex2);           // 申请访问 writecount
//     if(write_count == 0)         
//         sem_wait(S1);           // 写者进程进入等待队列
//     write_count++;
//     sem_post(mutex2);           // 释放 writecount
//     sem_wait(wmutex);           // 是否有读者或者写者正在操作

//     // 进行写操作
//     printf("writing ... \n");
//     write_to_pipe(fd, WRITE_NUM);

//     sem_post(wmutex);
//     sem_wait(mutex2);
//     write_count--;
//     if(write_count == 0)
//         sem_post(S1);
//     sem_post(mutex2);
// }

void write_to_pipe(int fd[2], int len){
    char text[MAXSIZE];
    int num_writed;

    if(len < MAXSIZE){
        initStr(text, WRITE_NUM);
    }
    else{
        printf("Write to pipe ERROR: len is too long ! \n");
        exit(1);
    }

    printf("Write Process: prepare to input %d charcters. \n", len);
  
    close(fd[0]);
    num_writed = write(fd[1], text, len);
 
    printf("Write Process: have inputted %d charcters. \n", num_writed);
}

void read_from_pipe(int fd[2], int len){
    char buf[MAXSIZE] = "\0";
    int num_read = 0;

    printf("Read Process: prepare to read %d charcters. \n", len);
  
    close(fd[1]);
    num_read = read(fd[0], buf, len);
 
    printf("Read Process: have read %d charcters. \n", num_read);
    printf("Read Process: read message is : \n%s\n ", buf);
}

void initStr(char *text, int len){
    int i;
    char str[MAXSIZE];
 	
     for(i = 0; i < len; i++){
		strcat(str,"*");
	}

	strcpy(text,"\0");
	strcat(text,str);   
}
