#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 256
char ** cmdls = {"ls", "cd", "rm", "mkdir", "rmdir", "./", "cat", "pwd"};
void GetCmd(char * str);
int CheckCmd(char * str);
int Run();
void RunCmd(int argc, char * str);

int main() {
	
	while(1)
	{
		Run(); //Run until user command to exit
	}
	return 0;
}

void GetCmd(char * str){
	fgets(str, MAX_STR-1, stdin); //get command string
}
int Run(){
	char cmd[256];
	printf("prompt> "); //print initial line
	GetCmd(cmd);
	cond = CheckCmd(cmd);//get the condition number which command should run
	RunCmd(cond, cmd);   //run the command
}
int CheckCmd(char * str){
	char * str_token = strtok(str," ");
	char num=0;
	while(num!=0)
	{
		if (!strcmp(str_token,*(cmdls+num)))
		{
			return num;
			break;
		}
		num++;
		if (num>=sizeof(*cmdls)){
			printf("Incorrect Command !\n");
			num=0;
		}
	}
}
int RunCmd(int argc, char * str){
	switch(argc)
	{
		case 0:
			cmd_ls(str);
			break;
		case 1:
			cmd_cd(str);
			break;
		case 2:
			cmd_rm(str):
			break;
		case 3:
			cmd_mkdir(str);
			break;
		case 4:
			cmd_rmdir(str);
			break;
		case 5:
			cmd_open(str);
			break;
		case 6:
			cmd_cat(str);
			break;
		case 7:
			cmd_pwd();
			break;
	}
}
