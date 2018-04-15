struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;
