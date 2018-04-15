#include "types.h"
#include "stat.h"
#include "user.h"

void periodic();

int
main(int argc, char *argv[])
{
 	int i;

 	printf(1, "test starting\n");
	set_cpu_share(20);
	alarm(5, "my app : 20%");

	for(i = 0; 50*500000 ; i++){
	if((i++ % 500000) == 0)
	    write(2, ".", 1);
	}
	exit();
}

void
periodic()
{
	printf(1, "SHARE 20 : 10 used\n");
}
