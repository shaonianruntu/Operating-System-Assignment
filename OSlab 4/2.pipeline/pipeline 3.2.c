#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXSIZE 100

#define check_error(err)                                        \
    if(err < 0){                                                \
        fprintf(stderr, "Error : %s \n",strerror(errno) );      \
        exit(EXIT_FAILURE);                                     \
    }       

int main()
{
    int err;
    int fd[2];
    pid_t pid[3]; //子进程pid
    err = pipe(fd);
    check_error(err);

    pid[0] = fork();
    check_error(pid[0]);
    if (pid[0] == 0)
    {
        close(fd[0]); //关闭读，只写
        char s[MAXSIZE] = "child 1 process is sending message!.\n";
        printf("child1\n");
        write(fd[1], s, sizeof(s));
        exit(0); //退出子进程
    }

    pid[1] = fork();
    check_error(pid[1]);
    if (pid[1] == 0)
    {
        close(fd[0]);
        char s[MAXSIZE] = "child 2 process is sending message!.\n";
        printf("child2\n");
        write(fd[1], s, sizeof(s));
        exit(0);
    }
    
    pid[2] = fork();
    check_error(pid[2]);
    if (pid[2] == 0)
    {
        close(fd[0]);
        char s[MAXSIZE] = "child 3 process is sending message!.\n";
        printf("child3\n");
        write(fd[1], s, sizeof(s));
        exit(0);
    }

    close(fd[1]); //关闭写，只读

    char buf[MAXSIZE];
    read(fd[0], buf, sizeof(buf));
    printf("%s", buf);
    
    char buf1[MAXSIZE];
    read(fd[0], buf1, sizeof(buf1));
    printf("%s", buf1);

    char buf2[MAXSIZE];
    read(fd[0], buf2, sizeof(buf2));
    printf("%s", buf2);
    wait(0);
    return 0;
}