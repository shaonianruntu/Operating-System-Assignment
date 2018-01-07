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

void write_to_pipe(int fd[2], int num);
void read_from_pipe(int fd[2]);

// 将其简化为一个写者优先的读者-写者问题。
int main(){
    sem_t *wmutex = sem_open("wmutex", O_CREAT, 666, 1);    // 读者与写者之间、写者与写者之间互斥使用共享数据

    pid_t pid[3];
    int fd[2];
    check_error(pipe(fd));

    int i;
    int j;

    for(i = 0; i < 3; i++){
        pid[i] = fork();
        check_error(pid[i]);
        if(pid[i] == 0){
            break;
        }
    }

    if(pid[0] == 0){
        sem_wait(wmutex);          
        write_to_pipe(fd, 1);
        sem_post(wmutex);
        exit(0);
    }
    if(pid[1] == 0){
        sem_wait(wmutex);          
        write_to_pipe(fd, 2);
        sem_post(wmutex);
        exit(0);
    }
    if(pid[2] == 0){
        sem_wait(wmutex);          
        write_to_pipe(fd, 3);
        sem_post(wmutex);
        exit(0);
    }

    wait(0);
    read_from_pipe(fd);
    read_from_pipe(fd);
    read_from_pipe(fd);
  //  wait(0);

    return 0;
}

void write_to_pipe(int fd[2], int num){
    char buf[MAXSIZE];
   
    sprintf(buf,"child %d process is sending message!\n", num);   
    close(fd[0]);
    write(fd[1], buf, WRITE_NUM);
}

void read_from_pipe(int fd[2]){
    char buf[MAXSIZE] = "\0";

    close(fd[1]);
    read(fd[0], buf, READ_NUM);
    printf("%s\n ", buf);  
}

