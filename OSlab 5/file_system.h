#ifndef __FILE_SYSTEM_H
#define __FILE_SYSTEM_H

#include <stdlib.h>
#include <string.h>
#include "iostream"
#include "fstream"
#include "time.h"
using namespace std;



///***** 定义变量 *****/
#define BLOCKSIZE       1024        // 磁盘块大小
#define SIZE            1024000     // 虚拟磁盘空间大小
#define END             65535       // FAT中的文件结束标志
#define FREE            0           // FAT中盘块空闲标志
#define MAXOPENFILE     10          // 最多同时打开文件个数

#define MAX_TEXT_SIZE  10000


/***** 定义数据结构 *****/

// 文件控制块
// 用于记录文件的描述和控制信息，每个文件设置一个FCB，它也是文件的目录项的内容
typedef struct FCB {
    char filename[8];           // 文件名
    char exname[3];             // 文件扩展名
    unsigned char attribute;    // 文件属性字段
    unsigned short time;        // 文件创建时间
    unsigned short date;        // 文件创建日期
    unsigned short first;       // 文件起始盘块号
    unsigned long length;       // 文件长度（字节数）
    char free;                  // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配
} fcb;

// 文件分配表
typedef struct FAT {
    unsigned short id;
} fat;

/* 用户打开文件表 */
typedef struct USEROPEN {
    char filename[8];           // 文件名
    char exname[3];             // 文件扩展名
    unsigned char attribute;    // 文件属性字段
    unsigned short time;        // 文件创建时间
    unsigned short date;        // 文件创建日期
    unsigned short first;       // 文件起始盘块号
    unsigned long length;       // 文件长度（对数据文件是字节数，对目录文件可以是目录项个数）
    char free;                  // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配

    int dirno;                  // 相应打开文件的目录项在父目录文件中的盘块号
    int diroff;                 // 相应打开文件的目录项在父目录文件的dirno盘块中的目录项序号
    char dir[80];               // 相应打开文件所在的目录名，这样方便快速检查出指定文件是否已经打开
    int count;                  // 读写指针在文件中的位置
    char fcbstate;              // 是否修改了文件的FCB的内容，如果修改了置为1，否则为0
    char topenfile;             // 表示该用户打开表项是否为空，若值为0，表示为空，否则表示已被某打开文件占据
} useropen;

/* 引导块 BLOCK0 */
typedef struct BLOCK0 {
    char magic_number[8];       // 文件系统的魔数
    char information[200];
    unsigned short root;        // 根目录文件的起始盘块号
    unsigned char *startblock;  // 虚拟磁盘上数据区开始位置
} block0;


/***** 全局变量定义 *****/
unsigned char *myvhard;             // 指向虚拟磁盘的起始地址
useropen openfilelist[MAXOPENFILE]; // 用户打开文件表数组
int currfd;                         // 记录当前用户打开文件表项的下标
unsigned char *startp;              // 记录虚拟磁盘上数据区开始位置
char *FileName = const_cast<char *>("myfsys.txt");
unsigned char buffer[SIZE];


/***** 函数申明 *****/
// 方楠
void startSys();                   // 进入文件系统
void my_exitsys();                 // 退出文件系统
void my_format();                  // 磁盘格式化函数
// 马诗墨
void my_mkdir(char *dirname);      // 创建子目录
void my_rmdir(char *dirname);      // 删除子目录
// 王振哲
void my_ls();                      // 显示目录中的内容
void my_cd(char *dirname);         // 用于更改当前目录
// 于俊泽
int  my_create(char *filename);    // 创建文件
void my_rm(char *filename);        // 删除文件
int  my_open(char *filename);      // 打开文件
int  my_close(int fd);             // 关闭文件
// 陈岑
int  my_write(int fd);             // 写文件
int  do_write(int fd, char *text, int len, char wstyle);
// 曹美玲
int  my_read(int fd);              // 读文件
int  do_read(int fd, int len, char *text);


unsigned short getFreeBLOCK();      // 获取一个空闲的磁盘块
int getFreeOpenfilelist();          // 获取一个空闲的文件打开表项
int find_father_dir(int fd);        // 寻找一个打开文件的父目录打开文件


void show_help();
void error(char *command);


/* 检查是否有空的用户文件打开表项 */
int getFreeOpenfilelist(){
    for(int i=0; i<MAXOPENFILE; i++){
        // 如果当前文件未打开就打开当前文件
        if(openfilelist[i].topenfile == 0){ 
            openfilelist[i].topenfile = 1;
            return i;
        }
    }
    return -1;
}


/* 检查 FAT 是否有空闲的盘块 */
unsigned short int getFreeBLOCK(){
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    for(int i=0; i < (int)(SIZE/BLOCKSIZE); i++){
        if(fat1[i].id == FREE){
            return i;
        }
    }
    return END;
}


/* 查找当前文件的父目录文件 */
int find_father_dir(int fd){
    for(int i=0; i<MAXOPENFILE; i++){
        if(openfilelist[i].first == openfilelist[fd].dirno){
            return i;
        }
    }
    return -1;
}


/* 帮助指令 */
void show_help() {
	printf("命令名\t\t命令参数\t\t命令说明\n\n");
	printf("cd\t\t目录名(路径名)\t\t切换当前目录到指定目录\n");
	printf("mkdir\t\t目录名\t\t\t在当前目录创建新目录\n");
	printf("rmdir\t\t目录名\t\t\t在当前目录删除指定目录\n");
	printf("ls\t\t无\t\t\t显示当前目录下的目录和文件\n");
	printf("create\t\t文件名\t\t\t在当前目录下创建指定文件\n");
	printf("rm\t\t文件名\t\t\t在当前目录下删除指定文件\n");
	printf("open\t\t文件名\t\t\t在当前目录下打开指定文件\n");
	printf("write\t\t无\t\t\t在打开文件状态下，写该文件\n");
	printf("read\t\t无\t\t\t在打开文件状态下，读取该文件\n");
	printf("close\t\t无\t\t\t在打开文件状态下，关闭该文件\n");
	printf("exit\t\t无\t\t\t退出系统\n\n");
}


/* 错误信息显示 */
void error(char *command) {
	printf("%s : 缺少参数\n", command);
	printf("输入 'help' 来查看命令提示.\n");
}


#endif /* __FILE_SYSTEM_H */