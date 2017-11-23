#define _GNU_SOURCE
#include "unistd.h"
#include "sys/syscall.h"
#include "stdio.h"
#define __NR_mysetnice 333

int main(){
    int pid = 0;
    int flag = 0;
    int nicevalue = 0;
    int prio = 0;
    int nice = 0;
    int state;

    while(1){
        printf("\nplease input rhe params: pid, flag, nicevalue\n");
        scanf("%d %d %d", &pid, &flag, &nicevalue);
        state = syscall(__NR_mysetnice, pid, flag, nicevalue, &prio, &nice);
        if(state != 0){
            printf("\nsyscall error!\n");
            return 0;
        }
        printf("\npid: %d, prio: %d, nice: %d\n", pid, prio, nice);
    }
    return 0;
}