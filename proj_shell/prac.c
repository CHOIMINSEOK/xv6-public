#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const int MAX_LEN = 150;

char *getCopyToken(char *token);
char **PharsingCommand(char *cmd);
int main(int argc,char *argv[]){
	char command[MAX_LEN];
	char **arv;
	char **str;
	char **batch;
	int pid,i,j;
	
	if(argc==1){
		while(1){
			i = 0,j = 0;
			printf("prompt> ");
			fgets(command,MAX_LEN,stdin);
			command[strlen(command)-1] = NULL;
			int Number_Command = 10;
			str = malloc(sizeof(char*)*10);
			str[j++] = strtok(command,";");
			while(str[j] = strtok(NULL,";")){
				j++;
				if(j%10==0)
					realloc(str,sizeof(char*)*(j+10));
			}
			for(i = 0;i<j;i++){
				pid = fork();
	
				switch(pid){
					case -1:
						perror("Uncompleted Fork\n");
						exit(0);
					case 0:
						arv = PharsingCommand(str[i]);
						execvp(arv[0],arv);
						free(arv);
						break;
					default:
						wait(0);
				}
			}
		}
	}
	/*else{
		i = argc-1;
		batch = malloc(sizeof(char*)*10);
		for(j = 0;j<i;j++){
			//여기서부터 만들다가 멈춤
		}
	}*/
	return 0;
}

char **PharsingCommand(char *cmd){
	int Number_word = 10;
	int i = 0;
	char **param =(char**)malloc(sizeof(char*)*Number_word);
	char * ptr = strtok(cmd," ");
	param[i++] = getCopyToken(ptr);
	while(ptr=strtok(NULL," ")){
		if(i==Number_word) {
			Number_word += Number_word;
			realloc(param,sizeof(char*)*Number_word);
		}
		param[i++] = getCopyToken(ptr);
	}
	return param;
}
char* getCopyToken(char * token){
	char * param = (char *)malloc(sizeof(char)*(strlen(token)+1));
	strcpy(param,token);
	return param;
}
