#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char* argv[]){
	printf(1, "currnet : %d\n", getpid());
	printf(1, "parent : %d\n", getppid());
	exit();
}
