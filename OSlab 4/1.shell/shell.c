#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#define CMD_MAX_LEN     20  
#define CMD_CASE_NUM    4

// command index
#define INVALID_CMD     -1
#define EXIT    0
#define CMD_1   1
#define CMD_2   2
#define CMD_3   3

char *cmdStr[CMD_CASE_NUM] = {"exit", "cmd1", "cmd2", "cmd3"};

int getCmdIndex(char *cmd);
void runCmd(int cmdIndex);
void forkChild(int cmdIndex);

int main(){
    pid_t pid;
    char cmdString[CMD_MAX_LEN];
    int cmd_index;

    while(1){
        printf("please input your command : ");
        scanf("%s", cmdString);
        cmd_index = getCmdIndex(cmdString);
        runCmd(cmd_index);

        wait(0);
        printf("~~~ waiting for next command ~~~\n");
    }
}


// 将用户输入的相应的命令名转化为命令索引值
int getCmdIndex(char *cmd){
    int i;
    for(int i = 0; i < CMD_CASE_NUM; i++){
        if(strcmp(cmd, cmdStr[i]) == 0){
            return i;
        }
    }
    return -1;
}

// 执行相应的进程任务
void runCmd(int cmdIndex){
    switch(cmdIndex){
        case INVALID_CMD:
            printf("Command not found!\n");
            break;
        case EXIT:
            exit(0);
            break;
        default:
            forkChild(cmdIndex);
            break;
    }
}


// 为相应的命令创建子进程并让它去执行相应的程序
void forkChild(int cmdIndex){
    pid_t pid;
    pid = fork();
        // ref : http://blog.csdn.net/jason314/article/details/5640969
    if(pid < 0){
        printf("error in fork!\n");
    }
    else if(pid == 0){
        printf("child process is running!\n");

        int execl_status = -1;
            // ref : http://c.biancheng.net/cpp/html/271.html
        switch(cmdIndex){
            case CMD_1:
                execl_status = execl("./cmd1", "cmd1", NULL);
                break;
            case CMD_2:
                execl_status = execl("./cmd2", "cmd2", NULL);
                break;
            case CMD_3:
                execl_status = execl("./cmd3", "cmd3", NULL);
                break;
            default:
                printf("error in cmd index!\n");
                break;
        }

        if(execl_status < 0){
            printf("error in fork!\n");
            exit(0);
        }

        printf("child process is done!\n");
        exit(0);
    }
}



