#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define TRUE 1

struct msgbuf
{
	long mtype;
	char mtext[1000];
}msgbuf;

// semaphore 
sem_t mutex;
sem_t full;
sem_t empty;

// pthread 
pthread_t send;
pthread_t receive;

int msgid;

void * sender();
void * receiver();

int main(){
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);

    msgid = msgget(0, 0666);

    printf("ID=%d\n", msgid);
    
    // 创建发送和接收线程
    pthread_create(&send, NULL, sender, NULL);
    pthread_create(&receive, NULL, receiver, NULL);    
        // pthread线程操作 ref : http://blog.csdn.net/ithomer/article/details/5920936

    pthread_join(send, NULL);
    pthread_join(receive, NULL);

    return 0;

}


void * sender(){
    char input[100];
    struct msgbuf msg;
    msg.mtype = 1;

    printf("hello sender\n");

    while(TRUE){

        //semaphore
		sem_wait(&empty);
		sem_wait(&mutex);

        printf("Please input the message you want to send. \n");
        scanf("%s", input);

        if(strcmp(input, "exit") == 0){
            memcpy(msg.mtext,"end", sizeof("end"));
            msgsnd(msgid, &msg, sizeof(msg), 0);
            break;
        }

        memcpy(msg.mtext, input, sizeof(input));
        msgsnd(msgid, &msg, sizeof(msg), 0);

        printf("[message send] %s\n", msg.mtext);

        //semaphore
        sem_post(&full);
        sem_post(&mutex);
    }

    // Clear Node
    memset(&msg, '\0', sizeof(msgbuf));
    // msg.mtype == 2
    msgrcv(msgid, &msg, sizeof(msgbuf), 2, 0);
    printf("[message send] %s\n", msg.mtext);

    //  Remove message Queue
    if(msgctl(msgid, IPC_RMID, 0) == -1){
        fprintf(stderr, "Remove Message Queue Error %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS); 
}

void * receiver(){
    struct msgbuf msg;
    msg.mtype = 1;

    while(TRUE){
        
        //semaphore 
        sem_wait(&full);
        sem_wait(&mutex);

        msgrcv(msgid, &msg, sizeof(msgbuf), 1, 0);

        if(strcmp(msg.mtext, "end") == 0){
            msg.mtype = 2;
            memcpy(msg.mtext, "over", sizeof("over"));
            msgsnd(msgid, &msg, sizeof(msgbuf), 0);
            break;
        }

        // Print
        printf("[message received] %s\n", msg.mtext);

        //semaphore
        sem_post(&empty);
        sem_post(&mutex);
    }

    exit(EXIT_SUCCESS);
}

