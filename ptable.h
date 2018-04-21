struct {
  struct spinlock lock;
  struct proc proc[NPROC];

  int isNewbieComming;
  int process_num[3];
  struct proc* mlfq[3][NPROC];
  struct proc* stride[NPROC];
} ptable;
