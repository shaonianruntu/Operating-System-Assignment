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
		gets(command);
		// fgets(command,sizeof(command),stdin);
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
void startSys() {

	// 
	myvhard = (unsigned char *)malloc(SIZE);

	FILE *file;
	if ((file = fopen(FileName, "r")) != NULL) {          // 
														  // 
		fread(buffer, SIZE, 1, file);
		// 
		if (memcmp(buffer, "10101010", 8) == 0) {           // 
															// 
			memcpy(myvhard, buffer, SIZE);
			cout << "myfsys文件读取成功!" << endl;
		}
		else {       //
			cout << "myfsys文件系统不存在，现在开始创建文件系统" << endl;
			// 
			my_format();
			// 
			memcpy(buffer, myvhard, SIZE);
			fwrite(buffer, SIZE, 1, file);
		}
	}
	else {       // 
		cout << "myfsys文件系统不存在，现在开始创建文件系统" << endl;
		// 
		my_format();
		// 
		memcpy(buffer, myvhard, SIZE);
		fwrite(buffer, SIZE, 1, file);
	}

	//
	fclose(file);


	//
	// 
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

	//
	//      
	//     
	openfilelist[0].dirno = 5;
	openfilelist[0].diroff = 0;
	strcpy(openfilelist[0].dir, "\\root\\");
	openfilelist[0].count = 0;
	openfilelist[0].fcbstate = 0;
	openfilelist[0].topenfile = 1;


	// 
	startp = ((block0*)myvhard)->startblock;
	// 
	currfd = 0;     //
	return;
}


/* 磁盘格式化函数 */
void my_format() {

	
	block0 *boot = (block0 *)myvhard;
	
	strcpy(boot->magic_number, "10101010");

	//
	strcpy(boot->information, "文件系统,外存分配方式:FAT,磁盘空间管理:结合于FAT的位示图,目录结构:单用户多级目录结构.");
	boot->root = 5;
	boot->startblock = myvhard + BLOCKSIZE * 5;   // 


	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	// 
	for (int i = 0; i<5; i++) {
		fat1[i].id = END;
	}
	//
	for (int i = 5; i<1000; i++) {
		fat1[i].id = FREE;
	}

	// 
	fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
	memcpy(fat2, fat1, BLOCKSIZE);


	// 
	fat1[5].id = fat2[5].id = END;  //

									
	fcb *root = (fcb*)(myvhard + BLOCKSIZE * 5);
	strcpy(root->filename, ".");
	strcpy(root->exname, "di");
	root->attribute = 0;        // 

	time_t rawTime = time(NULL);
	struct tm *time = localtime(&rawTime);
	// 
	root->time = time->tm_hour * 4096 + time->tm_min * 64 + time->tm_sec;
	root->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
	root->first = 5;
	root->free = 1;
	root->length = 2 * sizeof(fcb);

	//
	fcb* root2 = root + 1;
	memcpy(root2, root, sizeof(fcb));
	strcpy(root2->filename, "..");

	for (int i = 2; i < int(BLOCKSIZE / sizeof(fcb)); i++) {
		root2++;
		strcpy(root2->filename, "");
		root2->free = 0;
	}

}


/* 更改当前目录函数 */
void my_cd(char *dirname) {

	//  
	if (openfilelist[currfd].attribute == 1) {    //
		cout << "数据文件里不能使用cd, 要是退出文件, 请用close指令" << endl;
		return;
	}
	else {           // 

					 //
		char *buf = (char *)malloc(MAX_TEXT_SIZE);
		openfilelist[currfd].count = 0;
		do_read(currfd, openfilelist[currfd].length, buf);

		// 
		int i = 0;
		fcb* fcbPtr = (fcb*)buf;
		for (; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
			// 
			if (strcmp(fcbPtr->filename, dirname) == 0 && fcbPtr->attribute == 0) {
				break;
			}
		}

		// 
		if (strcmp(fcbPtr->exname, "di") != 0) {          //
														  // 
			cout << "不允许cd非目录文件!" << endl;
			return;
		}
		else {           // 

						 //
						 //
			if (strcmp(fcbPtr->filename, ".") == 0) {
				return;
			}
			// 
			else if (strcmp(fcbPtr->filename, "..") == 0) {

				// 
				if (currfd == 0) {    // 
					return;
				}
				else {               // 
									 // 
					currfd = my_close(currfd);
					return;
				}
			}
			//
			else {

				// 
				int fd = getFreeOpenfilelist();
				// 
				if (fd == -1) {
					return;
				}
				else {

					// 
					openfilelist[fd].attribute = fcbPtr->attribute;
					openfilelist[fd].count = 0;
					openfilelist[fd].date = fcbPtr->date;
					openfilelist[fd].time = fcbPtr->time;
					strcpy(openfilelist[fd].filename, fcbPtr->filename);
					strcpy(openfilelist[fd].exname, fcbPtr->exname);
					openfilelist[fd].first = fcbPtr->first;
					openfilelist[fd].free = fcbPtr->free;
					//
					openfilelist[fd].fcbstate = 0;
					openfilelist[fd].length = fcbPtr->length;
					//
					strcpy(openfilelist[fd].dir,
						(char*)(string(openfilelist[currfd].dir) + string(dirname) + string("\\")).c_str());
					openfilelist[fd].topenfile = 1;
					openfilelist[fd].dirno = openfilelist[currfd].first;
					openfilelist[fd].diroff = i;

					// 
					currfd = fd;
				}
			}
		}
	}
}


/* 创建子目录函数 */
void my_mkdir(char *dirname) {

	//
	char* fname = strtok(dirname, ".");
	char* exname = strtok(NULL, ".");
	if (exname) {
		cout << "不允许输入后缀名!" << endl;
		return;
	}

	//
	char text[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	int fileLen = do_read(currfd, openfilelist[currfd].length, text);

	// 
	fcb *fcbPtr = (fcb*)text;       //
	for (int i = 0; i < (int)(fileLen / sizeof(fcb)); i++) {
		if (strcmp(dirname, fcbPtr[i].filename) == 0 && fcbPtr->attribute == 0) {  // 重名
																				   // 
			cout << "目录名已经存在!" << endl;
			return;
		}
	}


	//
	int fd = getFreeOpenfilelist();
	if (fd == -1) {       // 
		cout << "打开文件表已全部被占用" << endl;
		return;
	}
	// 
	unsigned short int blockNum = getFreeBLOCK();
	if (blockNum == END) {
		// 
		openfilelist[fd].topenfile = 0;

		// 返回错误信息
		cout << "盘块已经用完" << endl;
		return;
	}

	
	// 
	fat *fat1 = (fat *)(myvhard + BLOCKSIZE);
	fat *fat2 = (fat *)(myvhard + BLOCKSIZE * 3);
	fat1[blockNum].id = END;
	fat2[blockNum].id = END;
	// 
	int i = 0;
	for (; i < (int)(fileLen / sizeof(fcb)); i++) {
		if (fcbPtr[i].free == 0) {
			break;
		}
	}
	openfilelist[currfd].count = i * sizeof(fcb);
	// 
	openfilelist[currfd].fcbstate = 1;


	// 
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
	// 
	do_write(currfd, (char *)fcbtmp, sizeof(fcb), 1);


	// 
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

	// 

	fcbtmp->attribute = 0;
	fcbtmp->date = fcbtmp->date;
	fcbtmp->time = fcbtmp->time;
	strcpy(fcbtmp->filename, ".");
	strcpy(fcbtmp->exname, "di");
	fcbtmp->first = blockNum;
	fcbtmp->length = 2 * sizeof(fcb);
	do_write(fd, (char*)fcbtmp, sizeof(fcb), 1);

	fcb *fcbtmp2 = new fcb;
	memcpy(fcbtmp2, fcbtmp, sizeof(fcb));
	strcpy(fcbtmp2->filename, "..");
	fcbtmp2->first = openfilelist[currfd].first;
	fcbtmp2->length = openfilelist[currfd].length;
	fcbtmp2->date = openfilelist[currfd].date;
	fcbtmp2->time = openfilelist[currfd].time;
	do_write(fd, (char*)fcbtmp2, sizeof(fcb), 1);


	my_close(fd);
	//
	// 
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
	//
	if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0) {
		cout << "不能删除" << dirname << "这个特殊目录项" << endl;
		return;
	}
	if (exname) {
		cout << "删除目录文件不用输入后缀名!" << endl;
		return;
	}

	// 
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);


	// 
	int i;
	fcb* fcbPtr = (fcb*)buf;
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (strcmp(fcbPtr->filename, fname) == 0 && fcbPtr->attribute == 0) {
			break;
		}
	}
	// 
	if (i == int(openfilelist[currfd].length / sizeof(fcb))) {
		cout << "没有这个目录文件" << endl;
		return;
	}


	// 
	if (fcbPtr->length > 2 * sizeof(fcb)) {
		cout << "请先清空这个目录下的所有文件,再删除目录文件" << endl;
		return;
	}


	// 
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


	// 
	fcbPtr->date = 0;
	fcbPtr->time = 0;
	fcbPtr->exname[0] = '\0';
	fcbPtr->filename[0] = '\0';
	fcbPtr->first = 0;
	fcbPtr->free = 0;
	fcbPtr->length = 0;
	// 
	openfilelist[currfd].count = i * sizeof(fcb);
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].length -= sizeof(fcb);


	// 
	fcbPtr = (fcb*)buf;
	fcbPtr->length = openfilelist[currfd].length;
	openfilelist[currfd].count = 0;
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].fcbstate = 1;
}


/* 显示目录函数 */
void my_ls() {

	// 
	if (openfilelist[currfd].attribute == 1) {
		cout << "在数据文件里不能使用ls" << endl;
		return;
	}


	// 
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);


	//
	fcb* fcbPtr = (fcb*)buf;
	for (int i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++) {
		if (fcbPtr->free == 1) {
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


	// 
	openfilelist[currfd].count = 0;
	char buf[MAX_TEXT_SIZE];
	do_read(currfd, openfilelist[currfd].length, buf);


	//
	int i;
	fcb* fcbPtr = (fcb*)(buf);
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (strcmp(fcbPtr->filename, filename) == 0 && strcmp(fcbPtr->exname, exname) == 0) {
			cout << "文件名已经存在!" << endl;
			return -1;
		}
	}


	// 
	int blockNum = getFreeBLOCK();
	if (blockNum == -1) {
		return -1;
	}


	// 
	fcbPtr = (fcb*)(buf);
	fcb* debug = (fcb*)(buf);
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (fcbPtr->free == 0)break;
	}


	// 
	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	fat1[blockNum].id = END;
	fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
	memcmp(fat2, fat1, BLOCKSIZE * 2);
	// 
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


	// 
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


	// 
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);

	// 
	int i;
	fcb* fcbPtr = (fcb*)buf;
	for (i = 0; i < int(openfilelist[currfd].length / sizeof(fcb)); i++, fcbPtr++) {
		if (strcmp(fcbPtr->filename, fname) == 0 && strcmp(fcbPtr->exname, exname) == 0) {
			break;
		}
	}
	// 
	if (i == int(openfilelist[currfd].length / sizeof(fcb))) {
		cout << "没有这个文件" << endl;
		return;
	}

	// *
	// 
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
	//
	fat1 = (fat*)(myvhard + BLOCKSIZE);
	fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
	memcpy(fat2, fat1, sizeof(fat));


	// 
	fcbPtr->date = 0;
	fcbPtr->time = 0;
	fcbPtr->exname[0] = '\0';
	fcbPtr->filename[0] = '\0';
	fcbPtr->first = 0;
	fcbPtr->free = 0;
	fcbPtr->length = 0;
	//
	openfilelist[currfd].count = i * sizeof(fcb);
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].length -= sizeof(fcb);


	// 
	fcbPtr = (fcb*)buf;
	fcbPtr->length = openfilelist[currfd].length;
	openfilelist[currfd].count = 0;
	do_write(currfd, (char*)fcbPtr, sizeof(fcb), 1);
	openfilelist[currfd].fcbstate = 1;
}


/* 打开文件函数 */
int my_open(char *filename) {

	// 
	char *fname = strtok(filename, ".");
	char *exname = strtok(NULL, ".");
	if (!exname) {
		cout << "请输入后缀名" << endl;
		return -1;
	}


	// 
	char buf[MAX_TEXT_SIZE];
	openfilelist[currfd].count = 0;
	do_read(currfd, openfilelist[currfd].length, buf);


	// 
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


	// 
	int fd = getFreeOpenfilelist();
	if (fd == -1) {
		cout << "用户打开文件表已经用满" << endl;
		return -1;
	}

	
	// 
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


	// 
	currfd = fd;
	return 1;
}


/* 关闭文件函数 */
int my_close(int fd) {

	// 
	if (fd > MAXOPENFILE || fd < 0) {
		cout << "不存在这个打开文件" << endl;
		return -1;
	}
	else {
		// 
		int fatherFd = find_father_dir(fd);
		if (fatherFd == -1) {
			cout << "父目录不存!" << endl;
			return -1;
		}

		//

		if (openfilelist[fd].fcbstate == 1) {
			// 
			char buf[MAX_TEXT_SIZE];
			do_read(fatherFd, openfilelist[fatherFd].length, buf);
			//
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
			
			// 
			do_write(fatherFd, (char*)fcbPtr, sizeof(fcb), 1);
		}

		// 
		memset(&openfilelist[fd], 0, sizeof(USEROPEN));

		// 
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

	
	// 
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


	// 
	cout << "请输入文件数据, 以换行+ctrl+z 为文件结尾" << endl;
	getchar();
	while (gets(textTmp)) {
	// while (fgets(textTmp,sizeof(textTmp),stdin)) {
		textTmp[strlen(textTmp)] = '\n';
		strcat(text, textTmp);
	}
	text[strlen(text)] = '\0';
	

	// 
	do_write(fd, text, strlen(text) + 1, wstyle);  	//
	
	
	// 
	openfilelist[fd].fcbstate = 1;

	return 1;
}


/* 实际写文件函数 */
int do_write(int fd, char *text, int len, char wstyle) {

	//
	int blockNum = openfilelist[fd].first;
	fat *fatPtr = (fat*)(myvhard + BLOCKSIZE) + blockNum;
	//
	fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
	if (wstyle == 0) {
		openfilelist[fd].count = 0;
		openfilelist[fd].length = 0;
	}
	//
	else if (wstyle == 1) {
		if (openfilelist[fd].attribute == 1 && openfilelist[fd].length != 0) {
			openfilelist[fd].count -= 1;
		}
	}
	//
	else if (wstyle == 2) {
		if (openfilelist[fd].attribute == 0) {
			openfilelist[fd].count = openfilelist[fd].length;
		}
		//
		else if (openfilelist[fd].attribute == 1 && openfilelist[fd].length != 0) {
			openfilelist[fd].count = openfilelist[fd].length - 1;
		}
	}


	int off = openfilelist[fd].count;										

	while (off >= BLOCKSIZE) {
		blockNum = fatPtr->id;
		if (blockNum == END) {
			blockNum = getFreeBLOCK();
			if (blockNum == END) {
				cout << "盘块不足" << endl;
				return -1;
			}
			else {
				//
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
	//
	while (len > lenTmp) {
		//
		memcpy(buf, blockPtr, BLOCKSIZE);
		//
		for (; off < BLOCKSIZE; off++) {
			*(buf + off) = *textPtr;
			textPtr++;
			lenTmp++;
			if (len == lenTmp) {
				break;
			}
		}
		//
		memcpy(blockPtr, buf, BLOCKSIZE);
		//
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
	//
	if (openfilelist[fd].count > openfilelist[fd].length)
		openfilelist[fd].length = openfilelist[fd].count;
	free(buf);
	//
	int i = blockNum;
	while (1) {
		//
		if (fat1[i].id != END) {
			int next = fat1[i].id;
			fat1[i].id = FREE;
			i = next;
		}
		else {
			break;
		}
	}
	//
	fat1[blockNum].id = END;
	//备份fat2
	memcpy((fat*)(myvhard + BLOCKSIZE * 3), (fat*)(myvhard + BLOCKSIZE), sizeof(fcb));
	return len;

}


/* 读文件函数 */
int my_read(int fd) {

	// 
	if (fd >= MAXOPENFILE || fd < 0) {
		cout << "文件不存在" << endl;
		return -1;
	}


	openfilelist[fd].count = 0;
	char text[MAX_TEXT_SIZE] = "\0";		// 


	// 
	do_read(fd, openfilelist[fd].length, text);
	

	// 
	cout << text;

	return 1;
}


/* 实际读文件函数 */
int do_read(int fd, int len, char *text) {

	//
	int lenTmp = len;

	unsigned char* buf = (unsigned char*)malloc(1024);
	if (buf == NULL) {
		cout << "do_read申请内存空间失败" << endl;
		return -1;
	}

	int off = openfilelist[fd].count;
	//
	int blockNum = openfilelist[fd].first;
	//
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
	//
	unsigned char* blockPtr = myvhard + BLOCKSIZE * blockNum;
	//
	memcpy(buf, blockPtr, BLOCKSIZE);
	char *textPtr = text;
	fcb* debug = (fcb*)text;
	while (len > 0) {
		//
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
			//
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
	//
	while (currfd) {
		my_close(currfd);		// 
	}


	// 
	FILE *fp = fopen(FileName, "w");
	// 
	fwrite(myvhard, SIZE, 1, fp);

	fclose(fp);
}



