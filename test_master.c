/**
 *  This program runs child test programs concurrently.
 */

#include "types.h"
#include "stat.h"
#include "user.h"

// Number of child programs
#define CNT_CHILD          8

// Name of child test program that tests Stride scheduler
#define NAME_CHILD_STRIDE   "test_stride"
// Name of child test program that tests MLFQ scheduler
#define NAME_CHILD_MLFQ     "test_mlfq"

char *child_argv[CNT_CHILD][3] = {
  // Process scheduled by Stride scheduler, use 10% of CPU resources
  {NAME_CHILD_STRIDE, "10", "stride 10%"},
  // Process scheduled by Stride scheduler, use 40% of CPU resources
  {NAME_CHILD_STRIDE,  "5",  "stride 5%"},
  {NAME_CHILD_STRIDE, "12", "stride 12%"},
  {NAME_CHILD_STRIDE, "20", "stride 20%"},
  {NAME_CHILD_STRIDE,  "3",  "stride 3%"},
  {NAME_CHILD_STRIDE,  "1",  "stride 1%"},
  {NAME_CHILD_STRIDE,  "1",  "stride 1%"},
  {NAME_CHILD_STRIDE, "32", "stride 32%"},

  // Process scheduled by MLFQ scheduler, does not yield() itself
//  {NAME_CHILD_MLFQ, "20", "mlfq 20%"},
  // Process scheduled by MLFQ scheduler, frequently yield()
//  {NAME_CHILD_MLFQ, "30", "mlfq 30%"},
};

int
main(int argc, char *argv[])
{
  int pid;
  int i;

  for (i = 0; i < CNT_CHILD; i++) {
    pid = fork();
    if (pid > 0) {
      // parent
      continue;
    } else if (pid == 0) {
      // child
	  
	  printf(1, "exec %s\n", child_argv[i][2]);
      exec(child_argv[i][0], child_argv[i]);
      printf(1, "exec %s failed!!\n", child_argv[i][1]);
      exit();
    } else {
      printf(1, "fork failed!!\n");
      exit();
    }
  }
  
  for (i = 0; i < CNT_CHILD; i++) {
    wait();
  }

  exit();
}
