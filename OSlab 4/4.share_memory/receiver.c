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

    // receive message
    char result[1024];

    sem_wait(full);	
    sem_wait(mutex);
    strcpy(result, shmptr);
    sem_post(mutex);

    printf("[message receive] %s\n", result);


    sem_close(mutex);
    sem_unlink("mutex_shm");

    sem_close(full);
    sem_unlink("full_shm");

   // shmctl(shmid, IPC_RMID, shmptr);

    return 0;
}
