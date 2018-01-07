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

#define MAXSIZE 100	
#define READ_NUM 50
#define WRITE_NUM 70

#define check_error(err)                                        \
    if(err < 0){                                                \
        fprintf(stderr, "Error : %s \n",strerror(errno) );      \
        exit(EXIT_FAILURE);                                     \
    }          

int pid[3];

void write_to_pipe(int fd[2], int num);
void read_from_pipe(int fd[2]);

int main(){
    int fd[2];
    pid_t pid1, pid2, pid3;
    sem_t *wmutex = sem_open("wmutex", O_CREAT, 666, 1);    // 读者与写者之间、写者与写者之间互斥使用共享数据
//    sem_t *S1 = sem_open("S1", O_CREAT, 666, 1);            // 当至少有一个写者准备访问共享数据时，它可使后续的读者等待写完成
//    int write_count = 0;
//    sem_t *mutex2 = sem_open("mutex2", O_CREAT, 666, 1);    // 多个写者互斥使用 wirte_count

    check_error(pipe(fd));

    pid1 = fork();
    check_error(pid1);
    if(pid1 == 0){
        sem_wait(wmutex);
        // 进行写操作
        printf("writing ... \n");   
        write_to_pipe(fd, i);
        sleep(5);
        sem_post(wmutex);

        return 0;
    }

    pid2 = fork();
    check_error(pid2);
    if(pid2 == 0){
        sem_wait(wmutex);
        // 进行写操作
        printf("writing ... \n");   
        write_to_pipe(fd, i);
        sleep(5);
        sem_post(wmutex);

        return 0;
    }

    pid3 = fork();
    check_error(pid3);
    if(pid3 == 0){
        sem_wait(wmutex);
        // 进行写操作
        printf("writing ... \n");   
        write_to_pipe(fd, i);
        sleep(5);
        sem_post(wmutex);

        return 0;
    }

    wait(pid1);
    wait(pid2);
    wait(pid3);

    printf("\nreading ... \n");  
    read_from_pipe(fd);  
    
    sem_unlink("wmutex");
//    sem_unlink(name2);

    return 0;

}

void write_to_pipe(int fd[2], int num){
    char buf[MAXSIZE];
    int num_writed;
   
    sprintf(buf,"child %d process is sending message!\n", i);
  
    close(fd[0]);
    num_writed = write(fd[1], buf, WRITE_NUM);
 
 //   printf("Write Process: have inputted %d charcters. \n", num_writed);
}

void read_from_pipe(int fd[2]){
    char buf[MAXSIZE] = "\0";
    int num_read = 0;

    close(fd[1]);
    num_read = read(fd[0], buf, READ_NUM);
 
//    printf("Read Process: have read %d charcters. \n", num_read);
//    printf("Read Process: read message is : \n%s\n ", buf);
     printf("%s\n ", buf);  
}