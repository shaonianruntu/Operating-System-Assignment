#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#define __NR_mysetnice 326 //系统调用号

int main() {
    pid_t tid;
    int nicevalue;
    int prio = 0;
    int nice = 0;
    tid = getpid();
    syscall(__NR_mysetnice,tid,0,-5,&prio,&nice);//read
    printf("pid: %d\nprio: %d\nnice: %d\n", tid, prio,nice);
    syscall(__NR_mysetnice,tid,1,-5,&prio,&nice);//set
    printf("pid: %d\nprio: %d\nnice: %d\n", tid, prio,nice);
    syscall(__NR_mysetnice,tid,0,-5,&prio,&nice);//read
    printf("pid: %d\nprio: %d\nnice: %d\n", tid, prio,nice);    
    return 0;
}