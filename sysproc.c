#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_clone(void)
{
  void  (*fun_ptr)(void*);
  void *argv;
  void *stack_ptr;
  int flags; 
  if(argptr(0,(void*)&fun_ptr, sizeof(void*)) < 0) return -1;
  if(argptr(1,(void*)&argv, sizeof(void*)) < 0) return -1;
  if (argptr(2,(void*)&stack_ptr, sizeof(void*)) < 0) return -1;
  if(argint(3, &flags) < 0) return -1;
  return clone(fun_ptr, argv, stack_ptr, flags);
}

int sys_join(void){
    int thread_id;
    void* join_ret;
    if(argptr(0,(void*)&thread_id, sizeof(thread_id)) < 0) return -1;
    if(argptr(1, (void*)&join_ret, sizeof(void**)) < 0) return -1;
    return join(thread_id, join_ret);
}

int sys_tkill(void){
  int tid;
  if(argptr(0,(void*)&tid, sizeof(tid)) < 0) return -1;
  return tkill(tid);
}

int sys_gettid(void){
  return myproc()->tid;
}

int sys_getppid(void){
  return myproc()->parent->pid;
}
int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

/* getpid(2) return the TGID of the caller. */
int
sys_getpid(void)
{
  return myproc()->pid;
}

int sys_tgkill(void){
    int tgid, tid;
    if(argptr(0,(void*)&tgid, sizeof(tgid)) < 0) return -1;
    if(argptr(1,(void*)&tid, sizeof(tid)) < 0) return -1;
    return tgkill(tgid, tid);
}


int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
