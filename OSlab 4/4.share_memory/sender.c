#include "share_memory.h"

int main(){
    // init 
    int shmid = shmget(KEY_NUM, 1024, 0666|IPC_CREAT);
    if(shmid < -1){
        fprintf(stderr, "shmget Error %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char * shmptr = shmat(shmid, NULL, 0);

    full = sem_open("full_shm", O_CREAT, 0666, 0);
    mutex = sem_open("mutex_shm", O_CREAT, 0666, 1);

    // input message 
    char input[1024];  
    printf("Please input the message you want to send.\n");
    scanf("%s", input);

    // send message 
    sem_wait(mutex);
    strcpy(shmptr, input);
    sem_post(mutex);

    sem_post(full);

  //  printf("[message send] %s\n", shmptr);
}
