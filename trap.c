#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "ptable.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;
//extern struct ptable;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);

	  if(ticks % 100 == 0) {
	  	acquire(&ptable.lock);
		struct proc* p;
		cprintf("*********** BOOST TIME ***********\n");
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			if(p->state == RUNNABLE || p->state == RUNNING ){	
				cprintf("%s running ticks in boost : %d, share : %d, lev : %d, index : %d, isSetCPU : %d, isInStride: %d\n", p->name, p->rticks_for_boost, p->share, p->q_lev, p->q_index, p->isSetCPU, p->isInStride);
				p->rticks_for_boost = 0;
			}
		}
		//cprintf("LEVEL 1 : ");

		cprintf("*********** INIT MLFQ ***********\n");

		for(int lev=1; lev < 3; lev++) {
			for(int qhead=0; qhead < NPROC; qhead++) {
				if(ptable.mlfq[lev][qhead] != 0) {
					struct proc* p = ptable.mlfq[lev][qhead];
					cprintf("In MLFQ Before: %s running ticks in boost : %d, share : %d, lev : %d, index : %d, isSetCPU : %d\n", p->name, p->rticks_for_boost, p->share, p->q_lev, p->q_index, p->isSetCPU);
		
					for(int i=0; i < NPROC; i++){
						if(ptable.mlfq[0][i]==0){
							ptable.mlfq[0][i] = p;
							p->q_lev = 0;
							p->q_index = i;
							p->time_allotment = 0;
							ptable.mlfq[lev][qhead] = 0;
							break;
						}
					}
					cprintf("In MLFQ After: %s running ticks in boost : %d, share : %d, lev : %d, index : %d, isSetCPU : %d\n", p->name, p->rticks_for_boost, p->share, p->q_lev, p->q_index, p->isSetCPU);
							
				}
			}
		}
		cprintf("*********** IN STRIDE ***************\n");	
			for(int i = 0; i < NPROC; i++ ) {
				p = ptable.stride[i];
				if(p == 0) continue;
				if(p->rticks_for_boost >= p->share) continue;
				
				cprintf("In Stride : %s running ticks in boost : %d, share : %d, lev : %d, index : %d, isSetCPU : %d\n", p->name, p->rticks_for_boost, p->share, p->q_lev, p->q_index, p->isSetCPU);
	
			}

		release(&ptable.lock);
	  }

    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER) {
	  
	  myproc()->rticks++;
	  myproc()->time_allotment++;
	  myproc()->rticks_for_boost++;

	  if(myproc()->rticks >= myproc()->share) {
		  myproc()->rticks = 0;
		  yield();
	  }
  }

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
