#include "file_system.h"

/***** 函数设计 *****/
// 系统主函数
int main() {
	char cmd[15][10] = { "mkdir", "rmdir", "ls", "cd", "create", "rm", "open", "close", "write", "read", "exit", "help","bigfile" };
	char command[30], *sp, *len, yesorno;
	int indexOfCmd, i;

	/*************************************************/
	printf("*********** 文件系统 **********\n");
	// 进入文件系统
	startSys();
	printf("开启文件系统.\n");
	printf("输入help来显示帮助页面.\n");

	while (1) {
		printf("%s>", openfilelist[currfd].dir);    //当前用户打开文件的目录名
		// gets(command);
		fgets(command,sizeof(command),stdin);
		indexOfCmd = -1;
		if (strcmp(command, "")) {                   // command 不为空
			sp = strtok(command, " ");              // 根据空格分解字符串
			for (i = 0; i < 15; i++) {               // 识别字符串中第一个操作符
				if (strcmp(sp, cmd[i]) == 0) {
					indexOfCmd = i;
					break;
				}
			}
			switch (indexOfCmd) {
			case 0:         // mkdir
				sp = strtok(NULL, " ");         // 继续抓取 command 之后的字符
												// ref: http://blog.csdn.net/zhangchaoq/article/details/45852709
				if (sp != NULL)
					my_mkdir(sp);               // 创建该目录名的子目录
				else
					error(const_cast<char *>("mkdir"));
				break;
			case 1:         // rmdir
				sp = strtok(NULL, " ");
				if (sp != NULL)
					my_rmdir(sp);               // 删除该目录名的子目录
				else
					error(const_cast<char *>("rmdir"));
				break;
			case 2:         // ls
				my_ls();                        // 显示目录
				break;
			case 3:         // cd
				sp = strtok(NULL, " ");
				if (sp != NULL)
					my_cd(sp);                  // 更改当前目录
				else
					error(const_cast<char *>("cd"));
				break;
			case 4:         // create
				sp = strtok(NULL, " ");
				if (sp != NULL)
					my_create(sp);              // 创建文件名为 sp 的文件
				else
					error(const_cast<char *>("create"));
				break;
			case 5:         // rm
				sp = strtok(NULL, " ");
				if (sp != NULL)
					my_rm(sp);                  // 删除文件名为 sp 的文件
				else
					error(const_cast<char *>("rm"));
				break;
			case 6:         // open
				sp = strtok(NULL, " ");
				if (sp != NULL)
					my_open(sp);                // 打开文件名为 sp 的文件
				else
					error(const_cast<char *>("open"));
				break;
			case 7:         // close
				if (openfilelist[currfd].attribute == 1)
					my_close(currfd);           // 关闭数据文件
				else
					cout << "当前没有的打开的文件" << endl;
				break;
			case 8:         // write
				if (openfilelist[currfd].attribute == 1)
					my_write(currfd);           // 写入数据到文件
				else
					cout << "请先打开文件,然后再使用wirte操作" << endl;
				break;
			case 9:         // read
				if (openfilelist[currfd].attribute == 1)
					my_read(currfd);            //  从文件中读出数据
				else
					cout << "请先打开文件,然后再使用read操作" << endl;
				break;
			case 10:        // exit
				my_exitsys();                   // 退出文件系统
				printf("退出文件系统.\n");
				return 0;
				break;
			case 11:        // help
				show_help();                    // 列出文件系统提供的各项功能及命令调用格式；
				break;
				// case 12:
				//     show_big_file();
				//     break;
			default:
				printf("没有 %s 这个命令\n", sp);
				break;
			}
		}
		else
			printf("\n");
	}
	return 0;
}


/* 进入文件系统函数 */
/**
* 1. 寻找myfsys.txt文件,如果存在而且开头是文件魔数,就读取到myvhard,否则创建文件并写入初始化信息
* 2. 设置用户打开文件表的第一个表项, 内容为根目录内容, 也就是默认打开根目录
* 3. 初始化一堆全局变量
*/
void startSys() {

	// 申请虚拟磁盘空间
	myvhard = (unsigned char *)malloc(SIZE);

	// 打开myfsys文件,并判断文件是否存在
	FILE *file;
	if ((file = fopen(FileName, "r")) != NULL) {          // 文件存在
														  // 读入myfsys文件内容到用户空间中的一个缓冲区中
		fread(buffer, SIZE, 1, file);
		// 判断文件开始的8个字节内容是否为“10101010”（文件系统魔数）
		if (memcmp(buffer, "10101010", 8) == 0) {           // 是魔数
															// 将上述缓冲区中的内容复制到内存中的虚拟磁盘空间中
			memcpy(myvhard, buffer, SIZE);
			cout << "myfsys文件读取成功!" << endl;
		}
		else {       // 文件开头不是魔数
			cout << "myfsys文件系统不存在，现在开始创建文件系统" << endl;
			// 调用my_format()对之前申请到的虚拟磁盘空间进行格式化操作
			my_format();
			// 将虚拟磁盘中的内容保存到 myfsys 文件中
			memcpy(buffer, myvhard, SIZE);
			fwrite(buffer, SIZE, 1, file);
		}
	}
	else {       // 文件不存在
		cout << "myfsys文件系统不存在，现在开始创建文件系统" << endl;
		// 调用my_format()对①中申请到的虚拟磁盘空间进行格式化操作
		my_format();
		// 将虚拟磁盘中的内容保存到 myfsys 文件中
		memcpy(buffer, myvhard, SIZE);
		fwrite(buffer, SIZE, 1, file);
	}

	// 关闭  myfsys 文件
	fclose(file);


	// 初始化用户文件表
	// 将表项0分配给根目录文件使用，并填写根目录文件的相关信息。
	fcb *root;
	root = (fcb *)(myvhard + 5 * BLOCKSIZE);
	strcpy(openfilelist[0].filename, root->filename);
	strcpy(openfilelist[0].exname, root->exname);
	openfilelist[0].attribute = root->attribute;
	openfilelist[0].time = root->time;
	openfilelist[0].date = root->date;
	openfilelist[0].first = root->first;
	openfilelist[0].length = root->length;
	openfilelist[0].free = root->free;

	// 根目录由于没有上级文件，所以
	//      dirno（根目录所在起始块号）为 5；
	//      diroff（dirno盘块中的目录项序号） 为 0
	openfilelist[0].dirno = 5;
	openfilelist[0].diroff = 0;
	strcpy(openfilelist[0].dir, "\\root\\");
	openfilelist[0].count = 0;
	openfilelist[0].fcbstate = 0;
	openfilelist[0].topenfile = 1;


	// 将 ptrcurdir 指针指向该用户打开文件表项
	startp = ((block0*)myvhard)->startblock;
	// 虚拟磁盘上数据区开始位置设为 0
	currfd = 0;     // 作用等价于 ptrcurdir 指针
	return;
}


/* 磁盘格式化函数 */
/**
* 1. 设置引导块(一个盘块)
* 2. 设置FAT1(2个盘块) FAT2(2个盘块)
* 3. 设置根目录文件的两个特殊目录项.和..(根目录文件占一个盘块,两个目录项是在写在这个盘块里面的)
*/
void my_format() {

	/* 引导块信息 */
	// 将虚拟磁盘第一个块作为引导块
	block0 *boot = (block0 *)myvhard;
	// 引导块的开始的8个字节是文件系统的魔数，记为“10101010”
	strcpy(boot->magic_number, "10101010");

	// 写入文件系统的描述信息
	strcpy(boot->information, "文件系统,外存分配方式:FAT,磁盘空间管理:结合于FAT的位示图,目录结构:单用户多级目录结构.");
	boot->root = 5;
	boot->startblock = myvhard + BLOCKSIZE * 5;   // 数据区从虚拟磁盘的第6块开始


												  /* 设置两个 FAT 表信息 */
												  // 建立 FAT1
	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	// 表项的前5块已经分配
	for (int i = 0; i<5; i++) {
		fat1[i].id = END;
	}
	// 表项的后995（根目录区和数据区空闲）
	for (int i = 5; i<1000; i++) {
		fat1[i].id = FREE;
	}

	// FAT2 与 FAT1 互为备份
	fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
	memcpy(fat2, fat1, BLOCKSIZE);


	/* 在 FAT2 后创建根目录文件 root */
	// 将数据区的第1块（虚拟磁盘的第6块）分配给根目录文件
	fat1[5].id = fat2[5].id = END;  //5号盘块（第6块）被根目录区占用了


									/* 在磁盘上创建两个特殊的目录项：“.”和“..”，其内容除了文件名不同之外，其他字段完全相同 */
									// 目录项"."指向根目录自己
	fcb *root = (fcb*)(myvhard + BLOCKSIZE * 5);
	strcpy(root->filename, ".");
	strcpy(root->exname, "di");
	root->attribute = 0;        // 只读

	time_t rawTime = time(NULL);
	struct tm *time = localtime(&rawTime);
	// 此处小时占 20 位， 分钟占 6 位， 秒钟占 6 位，可以完整显示 0 ~ 60。
	// 此处的设置实在 64 位的编译器条件下，其 unsigned short 类型的长度为 32 位。
	// 32 位的编译器，建议修改秒钟的位数为 5 位，时钟的位数为 5 位。
	root->time = time->tm_hour * 4096 + time->tm_min * 64 + time->tm_sec;
	root->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
	root->first = 5;
	root->free = 1;
	root->length = 2 * sizeof(fcb);

	//root2 指向根目录区的第二个fcb,即特俗或目录项..,因为根目录区没有上级目录,所以指向自己
	fcb* root2 = root + 1;
	memcpy(root2, root, sizeof(fcb));
	strcpy(root2->filename, "..");

	for (int i = 2; i < int(BLOCKSIZE / sizeof(fcb)); i++) {
		root2++;
		strcpy(root2->filename, "");
		root2->free = 0;
	}

	// //写到文件里面去
	// FILE *fp = fopen(FileName, "w");
	// fwrite(myvhard, SIZE, 1, fp);
	// fclose(fp);
}


/* 更改当前目录函数 */
/**
* 如果当前是目录文件下,那么需要把这个目录文件读取到buf里, 然后检索这个文件里的fcb有没有匹配dirname的目录项(而且必须是目录文件)
* 如果有,那就在openfilelist里取一个打开文件表项,把这个dirname这个目录文件的fcb写进去,然后切换currfd=fd
* 这样就算是打开一个目录
*/
void my_cd(char *dirname) {

	//  判断需要操作的文件是不是目录文件
	if (openfilelist[currfd].attribute == 1) {    // 是数据文件
		cout << "数据文件里不能使用cd, 要是退出文件, 请用close指令" << endl;
		return;
	}
	else {           // 是目录文件文件

					 // 将父目录（当前目录）的文件内容读取到内存中
		char *buf = (char *)malloc(MAX_TEXT_SIZE);
		openfilelist[currfd].count = 0;
		do_read(currfd, openfilelist[currfd].length, buf);

		// 在父目录文件中检查新的当前目录名是否存在
		int i = 0;
		fcb* fcbPtr = (fcb*)buf;
		for (; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
			// length 对于目录文件是目录项的个数
			if (strcmp(fcbPtr->filename, dirname) == 0 && fcbPtr->attribute == 0) {
				break;
			}
		}

		// 通过文件名后缀判断索引得到的文件是否为目录文件
		if (strcmp(fcbPtr->exname, "di") != 0) {          // 不是目录文件
														  // 显示错误信息并返回
			cout << "不允许cd非目录文件!" << endl;
			return;
		}
		else {           // 是目录文件

						 /* 判断需要 cd 的目录是 ".", ".." 还是子目录 */
						 // cd "." 即当前文件，不进行操作
			if (strcmp(fcbPtr->filename, ".") == 0) {
				return;
			}
			// cd ".."，返回上级目录
			else if (strcmp(fcbPtr->filename, "..") == 0) {

				// 判断当前目录是够为根目录
				if (currfd == 0) {    // 是根目录直接返回
					return;
				}
				else {               // 不是根目录
									 // 关闭当前目录文件并返回
					currfd = my_close(currfd);
					return;
				}
			}
			// cd 子文件
			else {

				// 为新建子目录文件分配一个空闲打开文件表项
				int fd = getFreeOpenfilelist();
				// 没有多余的文件打开表现，文件打开失败
				if (fd == -1) {
					return;
				}
				else {

					// 将该需要打开的子目录添加到文件打开表项中
					openfilelist[fd].attribute = fcbPtr->attribute;
					openfilelist[fd].count = 0;
					openfilelist[fd].date = fcbPtr->date;
					openfilelist[fd].time = fcbPtr->time;
					strcpy(openfilelist[fd].filename, fcbPtr->filename);
					strcpy(openfilelist[fd].exname, fcbPtr->exname);
					openfilelist[fd].first = fcbPtr->first;
					openfilelist[fd].free = fcbPtr->free;
					//前面是FCB内容
					openfilelist[fd].fcbstate = 0;
					openfilelist[fd].length = fcbPtr->length;
					// 修改 openfilelist[fd].dir[fd] = openfilelist[currfd].dir[currfd] + dirname;
					strcpy(openfilelist[fd].dir,
						(char*)(string(openfilelist[currfd].dir) + string(dirname) + string("\\")).c_str());
					openfilelist[fd].topenfile = 1;
					openfilelist[fd].dirno = openfilelist[currfd].first;
					openfilelist[fd].diroff = i;

					// 设置当前目录为该目录
					currfd = fd;
				}
			}
		}
	}
}


/* 创建子目录函数 */
void my_mkdir(char *dirname) {

	//判断dirname是否合法
	char* fname = strtok(dirname, ".");
	char* exname = strtok(NULL, ".");
	if (exname) {
		cout << "不允许输入后缀名!" << endl;
		return;
	}

	// 将当前文件内容读入到内存
	char text[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	int fileLen = do_read(currfd, openfilelist[currfd].length, text);

	// 检查当前目录下新建目录文件是否重名
	fcb *fcbPtr = (fcb*)text;       //text里的内容就是一个个fcb
	for (int i = 0; i < (int)(fileLen / sizeof(fcb)); i++) {
		if (strcmp(dirname, fcbPtr[i].filename) == 0 && fcbPtr->attribute == 0) {  // 重名
																				   // 显示重名错误信息并返回
			cout << "目录名已经存在!" << endl;
			return;
		}
	}

	/* 为新建子目录文件分配一个空闲打开文件表项 */
	//在打开文件表里找一个空文件表项
	int fd = getFreeOpenfilelist();
	if (fd == -1) {       // 文件打开表项已满
		cout << "打开文件表已全部被占用" << endl;
		return;
	}
	// 检查 FAT 是否有空闲的盘块并为新的新建目录文件分配一个盘块
	unsigned short int blockNum = getFreeBLOCK();
	if (blockNum == END) {
		// 释放当前父目录分配的用户文件打开表项
		openfilelist[fd].topenfile = 0;

		// 返回错误信息
		cout << "盘块已经用完" << endl;
		return;
	}

	/* 在当前目录中追加一个新的目录项 */
	// 打开当前目录
	fat *fat1 = (fat *)(myvhard + BLOCKSIZE);
	fat *fat2 = (fat *)(myvhard + BLOCKSIZE * 3);
	fat1[blockNum].id = END;
	fat2[blockNum].id = END;
	// 修改当前目录文件的长度信息
	int i = 0;
	for (; i < (int)(fileLen / sizeof(fcb)); i++) {
		if (fcbPtr[i].free == 0) {
			break;
		}
	}
	openfilelist[currfd].count = i * sizeof(fcb);
	// 将当前目录文件的用户打开文件表项中的 fcbstate 置为 1
	openfilelist[currfd].fcbstate = 1;


	// 准备好新建目录文件的 FCB 的内容
	fcb* fcbtmp = new fcb;
	fcbtmp->attribute = 0;
	time_t rawtime = time(NULL);
	struct tm* time = localtime(&rawtime);
	fcbtmp->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
	fcbtmp->time = (time->tm_hour) * 4096 + (time->tm_min) * 64 + (time->tm_sec);
	strcpy(fcbtmp->filename, dirname);
	strcpy(fcbtmp->exname, "di");
	fcbtmp->first = blockNum;
	fcbtmp->length = 2 * sizeof(fcb);
	fcbtmp->free = 1;
	// 以覆盖写方式调用 do_write() 将其填写到对应的空目录项中
	do_write(currfd, (char *)fcbtmp, sizeof(fcb), 1);


	// 在新建目录文件所分配到的磁盘块中建立两个特殊的目录项“.”和“..”目录项
	// 设置打开文件表项
	openfilelist[fd].attribute = 0;
	openfilelist[fd].count = 0;
	openfilelist[fd].date = fcbtmp->date;
	openfilelist[fd].time = fcbtmp->time;
	openfilelist[fd].dirno = openfilelist[currfd].first;
	openfilelist[fd].diroff = i;
	strcpy(openfilelist[fd].exname, "di");
	strcpy(openfilelist[fd].filename, dirname);
	openfilelist[fd].fcbstate = 0;
	openfilelist[fd].first = fcbtmp->first;
	openfilelist[fd].free = fcbtmp->free;
	openfilelist[fd].length = fcbtmp->length;
	openfilelist[fd].topenfile = 1;
	// 修改 openfilelist[fd].dir[fd] = openfilelist[currfd].dir[currfd] + dirname;
	strcpy(openfilelist[fd].dir, (char*)(string(openfilelist[currfd].dir) + string(dirname) + string("\\")).c_str());

	// 在对应的盘块里添加两个特殊的目录.和..
	// 在用户空间中准备 "." 目录文件的内容
	fcbtmp->attribute = 0;
	fcbtmp->date = fcbtmp->date;
	fcbtmp->time = fcbtmp->time;
	strcpy(fcbtmp->filename, ".");
	strcpy(fcbtmp->exname, "di");
	fcbtmp->first = blockNum;
	fcbtmp->length = 2 * sizeof(fcb);
	// 以截断写或者覆盖写方式调用 do_write()将其写到之前分配到的空闲磁盘块中；
	do_write(fd, (char*)fcbtmp, sizeof(fcb), 1);

	// 同理建立 ".." 目录文件的内容
	fcb *fcbtmp2 = new fcb;
	memcpy(fcbtmp2, fcbtmp, sizeof(fcb));
	strcpy(fcbtmp2->filename, "..");
	fcbtmp2->first = openfilelist[currfd].first;
	fcbtmp2->length = openfilelist[currfd].length;
	fcbtmp2->date = openfilelist[currfd].date;
	fcbtmp2->time = openfilelist[currfd].time;
	do_write(fd, (char*)fcbtmp2, sizeof(fcb), 1);


	my_close(fd);
	// ?
	// 更新本currfd目录文件的fcb
	fcbPtr = (fcb *)text;
	fcbPtr->length = openfilelist[currfd].length;
	openfilelist[currfd].count = 0;
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].fcbstate = 1;
	delete fcbtmp;
	delete fcbtmp2;
}


/* 删除子目录函数 */
void my_rmdir(char *dirname) {

	char* fname = strtok(dirname, ".");
	char* exname = strtok(NULL, ".");
	//不允许删除.和..这两个特殊目录文件
	if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0) {
		cout << "不能删除" << dirname << "这个特殊目录项" << endl;
		return;
	}
	if (exname) {
		cout << "删除目录文件不用输入后缀名!" << endl;
		return;
	}

	// 读取当前目录文件内容到内存
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);


	// 检查当前目录下欲删除目录文件是否存在
	int i;
	fcb* fcbPtr = (fcb*)buf;
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (strcmp(fcbPtr->filename, fname) == 0 && fcbPtr->attribute == 0) {
			break;
		}
	}
	// 不存在则返回错误信息
	if (i == int(openfilelist[currfd].length / sizeof(fcb))) {
		cout << "没有这个目录文件" << endl;
		return;
	}


	// 检查欲删除目录文件是否为空（除了“.”和“..”外没有其他子目录和文件）
	// 根据其目录项中记录的文件长度来判断
	if (fcbPtr->length > 2 * sizeof(fcb)) {
		cout << "请先清空这个目录下的所有文件,再删除目录文件" << endl;
		return;
	}


	// 回收该目录文件所占据的磁盘块，修改 FAT
	int blockNum = fcbPtr->first;
	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	int next = 0;
	while (1) {
		next = fat1[blockNum].id;
		fat1[blockNum].id = END;
		if (next != END) {
			blockNum = next;
		}
		else {
			break;
		}
	}
	// 备份 FAT2
	fat1 = (fat*)(myvhard + BLOCKSIZE);
	fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
	memcpy(fat2, fat1, sizeof(fat));


	// 从当前目录文件中清空该目录文件的目录项
	fcbPtr->date = 0;
	fcbPtr->time = 0;
	fcbPtr->exname[0] = '\0';
	fcbPtr->filename[0] = '\0';
	fcbPtr->first = 0;
	fcbPtr->free = 0;
	fcbPtr->length = 0;
	// 写到磁盘上去, 更新 FCB 内容为空
	openfilelist[currfd].count = i * sizeof(fcb);
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].length -= sizeof(fcb);


	// 修改当前目录文件的用户打开表项中的长度信息
	fcbPtr = (fcb*)buf;
	fcbPtr->length = openfilelist[currfd].length;
	openfilelist[currfd].count = 0;
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].fcbstate = 1;
}


/* 显示目录函数 */
void my_ls() {

	// 确保访问的是目录文件
	if (openfilelist[currfd].attribute == 1) {
		cout << "在数据文件里不能使用ls" << endl;
		return;
	}


	// 调用 do_read()读出当前目录文件内容到内存
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);


	// 将读出的目录文件的信息按照一定的格式显示到屏幕上	
	fcb* fcbPtr = (fcb*)buf;
	for (int i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++) {
		if (fcbPtr->free == 1) {
			// 目录文件
			// 年份占23位,月份占4位,日期占5位
			// 小时占20位,分钟占6位,秒占6位
			if (fcbPtr->attribute == 0) {
				printf("%s\\\t<DIR>\t%d/%d/%d\t%d:%d:%d\n",
					fcbPtr->filename,
					(fcbPtr->date >> 9) + 2000,
					(fcbPtr->date >> 5) & 0x000f,
					(fcbPtr->date) & 0x001f,
					(fcbPtr->time >> 12),
					(fcbPtr->time >> 6) & 0x003f,
					(fcbPtr->time) & 0x003f);
			}
			// 数据文件
			else {
				// length - 2 是因为末尾有/n和/0两个字符
				unsigned int length = fcbPtr->length;
				if (length != 0)
					length -= 2;
				printf("%s.%s\t%dB\t%d/%d/%d\t%02d:%02d:%02d\n",
					fcbPtr->filename,
					fcbPtr->exname,
					length,
					(fcbPtr->date >> 9) + 2000,
					(fcbPtr->date >> 5) & 0x000f,
					(fcbPtr->date) & 0x001f,
					(fcbPtr->time >> 12),
					(fcbPtr->time >> 6) & 0x003f,
					(fcbPtr->time) & 0x003f);
			}
		}
		fcbPtr++;
	}
}


/* 创建文件函数 */
int my_create(char *filename) {
	
	char* fname = strtok(filename, ".");
	char* exname = strtok(NULL, ".");
	if (strcmp(fname, "") == 0) {
		cout << "请输入文件名!" << endl;
		return -1;
	}
	if (!exname) {
		cout << "请输入后缀名!" << endl;
		return -1;
	}
	if (openfilelist[currfd].attribute == 1) {
		cout << "数据文件下不允许使用create" << endl;
		return -1;
	}


	// 读出该父目录文件内容到内存
	openfilelist[currfd].count = 0;
	char buf[MAX_TEXT_SIZE];
	do_read(currfd, openfilelist[currfd].length, buf);


	// 检查该目录下新文件是否重名
	int i;
	fcb* fcbPtr = (fcb*)(buf);
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (strcmp(fcbPtr->filename, filename) == 0 && strcmp(fcbPtr->exname, exname) == 0) {
			cout << "文件名已经存在!" << endl;
			return -1;
		}
	}


	// 为新文件分配一个盘块
	int blockNum = getFreeBLOCK();
	if (blockNum == -1) {
		return -1;
	}


	// 为新文件寻找一个空闲的目录项
	fcbPtr = (fcb*)(buf);
	fcb* debug = (fcb*)(buf);
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (fcbPtr->free == 0)break;
	}


	// 准备新文件的 FCB 的内容
	// 文件的属性为数据文件，长度为 0
	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	fat1[blockNum].id = END;
	fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
	memcmp(fat2, fat1, BLOCKSIZE * 2);
	// 往 FCB 里写信息
	strcpy(fcbPtr->filename, filename);
	strcpy(fcbPtr->exname, exname);
	time_t rawtime = time(NULL);
	struct tm* time = localtime(&rawtime);
	fcbPtr->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
	fcbPtr->time = (time->tm_hour) * 4096 + (time->tm_min) * 64 + (time->tm_sec);
	fcbPtr->first = blockNum;
	fcbPtr->free = 1;
	fcbPtr->length = 0;
	fcbPtr->attribute = 1;
	openfilelist[currfd].count = i * sizeof(fcb);
	do_write(currfd, (char *)fcbPtr, sizeof(fcb), 1);


	// 修改当前目录文件的.目录项的长度
	fcbPtr = (fcb*)buf;
	fcbPtr->length = openfilelist[currfd].length;
	openfilelist[currfd].count = 0;
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].fcbstate = 1;
}


/* 删除文件函数 */
void my_rm(char *filename) {

	char* fname = strtok(filename, ".");
	char* exname = strtok(NULL, ".");
	if (!exname) {
		cout << "请输入后缀名!" << endl;
		return;
	}
	if (strcmp(exname, "di") == 0) {
		cout << "不能删除目录项" << endl;
		return;
	}


	// 读出该父目录文件内容到内存
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);

	// 检查该目录下欲删除文件是否存在
	int i;
	fcb* fcbPtr = (fcb*)buf;
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (strcmp(fcbPtr->filename, fname) == 0 && strcmp(fcbPtr->exname, exname) == 0) {
			break;
		}
	}
	// 若不存在则返回，并显示错误信息
	if (i == int(openfilelist[currfd].length / sizeof(fcb))) {
		cout << "没有这个文件" << endl;
		return;
	}

	// *
	// 回收该文件所占据的磁盘块，修改 FAT
	int blockNum = fcbPtr->first;
	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	int next = 0;
	while (1) {
		next = fat1[blockNum].id;
		fat1[blockNum].id = FREE;
		if (next != END) {
			blockNum = next;
		}
		else {
			break;
		}
	}
	// 备份 FAT2
	fat1 = (fat*)(myvhard + BLOCKSIZE);
	fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
	memcpy(fat2, fat1, sizeof(fat));


	// 从文件的父目录文件中清空该文件的目录项，且 free 字段置为 0
	fcbPtr->date = 0;
	fcbPtr->time = 0;
	fcbPtr->exname[0] = '\0';
	fcbPtr->filename[0] = '\0';
	fcbPtr->first = 0;
	fcbPtr->free = 0;
	fcbPtr->length = 0;
	//写到磁盘上去, 更新 FCB 内容为空
	openfilelist[currfd].count = i * sizeof(fcb);
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].length -= sizeof(fcb);


	// 修改该父目录文件的用户打开文件表项中的长度信息
	fcbPtr = (fcb*)buf;
	fcbPtr->length = openfilelist[currfd].length;
	openfilelist[currfd].count = 0;
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].fcbstate = 1;
}


/* 打开文件函数 */
int my_open(char *filename) {

	// 是否为数据文件
	char *fname = strtok(filename, ".");
	char *exname = strtok(NULL, ".");
	if (!exname) {
		cout << "请输入后缀名" << endl;
		return -1;
	}


	// 出父目录文件的内容到内存
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);


	// 检查该目录下欲打开文件是否存在 
	int i;
	fcb* fcbPtr = (fcb*)buf;
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (strcmp(fcbPtr->filename, fname) == 0 && strcmp(fcbPtr->exname, exname) == 0 && fcbPtr->attribute == 1) {
			break;
		}
	}
	if (i == int(openfilelist[currfd].length / sizeof(fcb))) {
		cout << "不存在此文件!" << endl;
		return -1;
	}


	// 为欲打开文件分配一个空表项
	int fd = getFreeOpenfilelist();
	if (fd == -1) {
		cout << "用户打开文件表已经用满" << endl;
		return -1;
	}

	
	// 为该文件填写空白用户打开文件表表项内容，读写指针置为 0
	openfilelist[fd].attribute = 1;
	openfilelist[fd].count = 0;
	openfilelist[fd].date = fcbPtr->date;
	openfilelist[fd].time = fcbPtr->time;
	strcpy(openfilelist[fd].exname, exname);
	strcpy(openfilelist[fd].filename, fname);
	openfilelist[fd].length = fcbPtr->length;
	openfilelist[fd].first = fcbPtr->first;
	strcpy(openfilelist[fd].dir, (string(openfilelist[currfd].dir) + string(filename)).c_str());
	openfilelist[fd].dirno = openfilelist[currfd].first;
	openfilelist[fd].diroff = i;
	openfilelist[fd].free = 1;
	openfilelist[fd].topenfile = 1;
	openfilelist[fd].fcbstate = 0;


	// 将该文件所分配到的空白用户打开文件表表项序号（数组下标）作为文件描述符 fd 返回
	currfd = fd;
	return 1;
}


/* 关闭文件函数 */
int my_close(int fd) {

	// 检查 fd 的有效性（fd 不能超出用户打开文件表所在数组的最大下标）
	if (fd > MAXOPENFILE || fd < 0) {
		cout << "不存在这个打开文件" << endl;
		return -1;
	}
	else {
		// 判断父目录文件是否存在, 不存在报错
		int fatherFd = find_father_dir(fd);
		if (fatherFd == -1) {
			cout << "父目录不存!" << endl;
			return -1;
		}

		//fcb被修改了, 要写回去
		//那就要先把父目录文件从磁盘中读取到buf中,然后把更新后的fcb内容写到buf里, 然后再从buf写到磁盘上
		//多说一句,写回到磁盘的时候,只要让游标count指向这个文件的对应fcb的位置,然后写入fcb就好了
		//这样只要写一个fcb大小的数据就行了

		if (openfilelist[fd].fcbstate == 1) {
			// 读取父目录文件到内存中
			char buf[MAX_TEXT_SIZE];
			do_read(fatherFd, openfilelist[fatherFd].length, buf);
			// 更新 FCB 
			fcb* fcbPtr = (fcb *)(buf + sizeof(fcb) * openfilelist[fd].diroff);
			strcpy(fcbPtr->exname, openfilelist[fd].exname);
			strcpy(fcbPtr->filename, openfilelist[fd].filename);
			fcbPtr->first = openfilelist[fd].first;
			fcbPtr->free = openfilelist[fd].free;
			fcbPtr->length = openfilelist[fd].length;
			openfilelist[fatherFd].count = 0;
			fcbPtr->time = openfilelist[fd].time;
			fcbPtr->date = openfilelist[fd].date;
			fcbPtr->attribute = openfilelist[fd].attribute;
			openfilelist[fatherFd].count = openfilelist[fd].diroff * sizeof(fcb);
			
			// 重新将更新 FCB 写入到 父目录文件相应的盘块中
			do_write(fatherFd, (char*)fcbPtr, sizeof(fcb), 1);
		}

		// 回收该文件占据的用户打开文件表表项（进行清空操作）
		memset(&openfilelist[fd], 0, sizeof(USEROPEN));

		// 修改当前下标
		currfd = fatherFd;
		return fatherFd;
	}

}


/* 写文件函数 */
int my_write(int fd) {

	// 检查 fd 的有效性（fd 不能超出用户打开文件表所在数组的最大下标）
	if (fd < 0 || fd >= MAXOPENFILE) {
		cout << "文件不存在" << endl;
		return -1;
	}

	
	// 提示并等待用户输入写方式：（1：截断写； 2：覆盖写； 3：追加写）
	int wstyle;
	while (1) {
		cout << "输入: 0=截断写, 1=覆盖写, 2=追加写" << endl;
		cin >> wstyle;
		if (wstyle > 2 || wstyle < 0) {
			cout << "指令错误!" << endl;
		}
		else {
			break;
		}
	}
	char text[MAX_TEXT_SIZE] = "\0";
	char textTmp[MAX_TEXT_SIZE] = "\0";


	// 提示用户：整个输入内容通过 CTR+Z 键（或其他设定的键）结束；
	// 用户可分多次输入写入内容，每次用回车结束
	cout << "请输入文件数据, 以换行+ctrl+z 为文件结尾" << endl;
	getchar();
	// while (gets(textTmp)) {
	while (fgets(textTmp,sizeof(textTmp),stdin)) {
		textTmp[strlen(textTmp)] = '\n';
		strcat(text, textTmp);
	}
	text[strlen(text)] = '\0';
	

	// 调用 do_write()函数将通过键盘键入的内容写到文件中
	do_write(fd, text, strlen(text) + 1, wstyle);  	//+1是因为要把结尾的\0也写进去
	
	
	// 修改打开文件表项中的文件长度信息，并将 fcbstate 置 1
	openfilelist[fd].fcbstate = 1;

	return 1;
}


/* 实际写文件函数 */
int do_write(int fd, char *text, int len, char wstyle) {

	//盘块号
	int blockNum = openfilelist[fd].first;
	fat *fatPtr = (fat*)(myvhard + BLOCKSIZE) + blockNum;
	//三种写入方式预处理
	// 0截断写,直接从头开始写,偏移量就是0了,而且长度变成0
	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	if (wstyle == 0) {
		openfilelist[fd].count = 0;
		openfilelist[fd].length = 0;
	}
	//1,覆盖写, 如果是数据文件,那么要考虑删除文件末尾的\0才能继续往下写
	else if (wstyle == 1) {
		if (openfilelist[fd].attribute == 1 && openfilelist[fd].length != 0) {
			openfilelist[fd].count -= 1;
		}
	}
	//2追加写,就把游标指向末尾
	else if (wstyle == 2) {
		if (openfilelist[fd].attribute == 0) {
			openfilelist[fd].count = openfilelist[fd].length;
		}
		//同理,如果是数据文件要删除末尾的\0
		else if (openfilelist[fd].attribute == 1 && openfilelist[fd].length != 0) {
			openfilelist[fd].count = openfilelist[fd].length - 1;
		}
	}


	int off = openfilelist[fd].count;										//??????????

																			//如果off > BLOCKSIZE,也就是游标现在指向的不是文件中的第一个盘块,那么需要找到那个盘块
																			//另外,如果游标很大,但是在寻找对应盘块的时候发现没有那个盘块,那么把缺少的盘块全都补上
	while (off >= BLOCKSIZE) {
		blockNum = fatPtr->id;
		if (blockNum == END) {
			blockNum = getFreeBLOCK();
			if (blockNum == END) {
				cout << "盘块不足" << endl;
				return -1;
			}
			else {
				//修改了fat, 后面要修改fat2, 不必判断修改了没有fat, 直接无脑复制fat1到fat2就行
				fatPtr->id = blockNum;
				fatPtr = (fat*)(myvhard + BLOCKSIZE + blockNum);
				fatPtr->id = END;
			}
		}
		fatPtr = (fat*)(myvhard + BLOCKSIZE) + blockNum;
		off -= BLOCKSIZE;
	}

	unsigned char *buf = (unsigned char*)malloc(BLOCKSIZE * sizeof(unsigned char));
	if (buf == NULL) {
		cout << "申请内存空间失败!";
		return -1;
	}


	fcb *dBlock = (fcb *)(myvhard + BLOCKSIZE * blockNum);
	fcb *dFcb = (fcb *)(text);
	unsigned char *blockPtr = (unsigned char *)(myvhard + BLOCKSIZE * blockNum);					//盘块指针
	int lenTmp = 0;
	char *textPtr = text;
	fcb *dFcbBuf = (fcb *)(buf);
	//第二个循环,读取盘块内容到buf, 把text内容写入buf, 然后再从buf写入到盘块
	while (len > lenTmp) {
		//盘块内容读取到buf里
		memcpy(buf, blockPtr, BLOCKSIZE);
		//把text内容写到buf里面去
		for (; off < BLOCKSIZE; off++) {
			*(buf + off) = *textPtr;
			textPtr++;
			lenTmp++;
			if (len == lenTmp) {
				break;
			}
		}
		//把buf内容写到盘块里面去
		memcpy(blockPtr, buf, BLOCKSIZE);
		//如果off==BLCOKSIZE,意味着buf写满了, 如果len != lebTmp 意味着数据还没写完, 那么就要看看这个文件还有没有剩余盘块
		//没有剩余盘块,那就要分配新的盘块了
		if (off == BLOCKSIZE && len != lenTmp) {
			off = 0;
			blockNum = fatPtr->id;
			if (blockNum == END) {
				blockNum = getFreeBLOCK();
				if (blockNum == END) {
					cout << "盘块用完了" << endl;
					return -1;
				}
				else {
					blockPtr = (unsigned char *)(myvhard + BLOCKSIZE * blockNum);
					fatPtr->id = blockNum;
					fatPtr = (fat *)(myvhard + BLOCKSIZE) + blockNum;
					fatPtr->id = END;
				}
			}
			else {
				blockPtr = (unsigned char *)(myvhard + BLOCKSIZE * blockNum);
				fatPtr = (fat *)(myvhard + BLOCKSIZE) + blockNum;
			}
		}
	}
	openfilelist[fd].count += len;
	//若读写指针大于原来文件的长度，则修改文件的长度
	if (openfilelist[fd].count > openfilelist[fd].length)
		openfilelist[fd].length = openfilelist[fd].count;
	free(buf);
	//如果原来文件占几个盘块,现在修改了文件,结果占用的盘块变少了,那就要把后面占用的盘块全部释放掉
	int i = blockNum;
	while (1) {
		//如果这个fat的下一个fat不是end,那么就是释放掉它,一路释放下去
		if (fat1[i].id != END) {
			int next = fat1[i].id;
			fat1[i].id = FREE;
			i = next;
		}
		else {
			break;
		}
	}
	//按照上面这种操作,会把本文件的最后一个盘块也变成free,这里要把他重新设置成END
	fat1[blockNum].id = END;
	//备份fat2
	memcpy((fat*)(myvhard + BLOCKSIZE * 3), (fat*)(myvhard + BLOCKSIZE), sizeof(fcb));
	return len;

}


/* 读文件函数 */
int my_read(int fd) {

	// 检查 fd 的有效性
	if (fd >= MAXOPENFILE || fd < 0) {
		cout << "文件不存在" << endl;
		return -1;
	}


	openfilelist[fd].count = 0;
	char text[MAX_TEXT_SIZE] = "\0";		// 用来接收用户从文件中读出的文件内容


	// 将指定文件中的 len 字节内容读出到 text[]中
	do_read(fd, openfilelist[fd].length, text);
	

	// 将 text[]中的内容显示到屏幕上
	cout << text;

	return 1;
}


/* 实际读文件函数 */
int do_read(int fd, int len, char *text) {

	//lenTmp 用于记录要求读取的长度,一会返回实际读取的长度
	int lenTmp = len;

	unsigned char* buf = (unsigned char*)malloc(1024);
	if (buf == NULL) {
		cout << "do_read申请内存空间失败" << endl;
		return -1;
	}

	int off = openfilelist[fd].count;
	//当前fd对应的起始盘块号, 后面变成当前盘块号
	int blockNum = openfilelist[fd].first;
	//ptrfat 当前盘块对应的fat
	fat* fatPtr = (fat *)(myvhard + BLOCKSIZE) + blockNum;
	while (off >= BLOCKSIZE) {
		off -= BLOCKSIZE;
		blockNum = fatPtr->id;
		if (blockNum == END) {
			cout << "do_read寻找的块不存在" << endl;
			return -1;
		}
		fatPtr = (fat*)(myvhard + BLOCKSIZE) + blockNum;
	}
	//当前盘块号对应的盘块
	unsigned char* blockPtr = myvhard + BLOCKSIZE * blockNum;
	//把文件内容读进buf
	memcpy(buf, blockPtr, BLOCKSIZE);
	char *textPtr = text;
	fcb* debug = (fcb*)text;
	while (len > 0) {
		//一个盘块就能放的下
		if (BLOCKSIZE - off > len) {
			memcpy(textPtr, buf + off, len);
			textPtr += len;
			off += len;
			openfilelist[fd].count += len;
			len = 0;
		}
		else {
			memcpy(textPtr, buf + off, BLOCKSIZE - off);
			textPtr += BLOCKSIZE - off;
			off = 0;
			len -= BLOCKSIZE - off;
			//寻找下一个块
			blockNum = fatPtr->id;
			if (blockNum == END) {
				cout << "len长度过长! 超出了文件大小!" << endl;
				break;
			}
			fatPtr = (fat*)(myvhard + BLOCKSIZE) + blockNum;
			blockPtr = myvhard + BLOCKSIZE * blockNum;
			memcpy(buf, blockPtr, BLOCKSIZE);
		}
	}
	free(buf);
	return lenTmp - len;
}

/* 退出文件系统函数 */
void my_exitsys() {
	//当currfd=0的时候,也就是根节点, 它是不用更新的
	//因为我们之前在任何目录下mkdir或者create时,已经把length的变化写到目录文件下,名字叫.的目录项里了
	//即任何目录文件的'.'目录项都是实时更新的,但是他的父目录文件的没有被更新,所以需要一个个close
	//但是根目录没有父目录文件, 所以不需要close
	while (currfd) {
		my_close(currfd);		// 关闭 myfsys 文件
	}


	// 撤销用户打开文件表，释放其内存空间
	FILE *fp = fopen(FileName, "w");
	// 释放虚拟磁盘空间
	fwrite(myvhard, SIZE, 1, fp);
	fclose(fp);
}



