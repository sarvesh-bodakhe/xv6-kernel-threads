#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
int nexttid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  /* @desc  : tid is always unique. pid may or may not be uniuqe. 
        instead of changing how pid is initialized in allocproc(),
        write the code to handle pid in clone() as per the falgs given. (Assuming nextpid++ has called)
        Not making changes here cause alloproc() is also called by fork() and modifying allocproc() will affect other calls.      
  */
  p->pid = nextpid++;
  p->tid = nexttid++;
  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}



// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  // cprintf("exit: (%d,%d,%d)\n", curproc->tid, curproc->pid, curproc->parent->pid);
  // cprintf("Open Files for curproc: ");
  //   for(int i=0;i<NOFILE; i++){
  //     cprintf("%p ", ((struct file*)(curproc->ofile[i])) );
  //   }
  //   cprintf("\n");
  if(curproc == initproc)
    panic("init exiting");

  // Close all open files. Only the leader thread will be able to close files. All those children threads who have flag CLONE_FILES will not close this files
  if(!(curproc->flags & CLONE_FILES)){
    // cprintf("exit: Files are not shared. closing files\n");
    for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
    }
  }else{
    // cprintf("exit: files are being shared. do not close files\n");
  }

  if(!(curproc->flags  &CLONE_FS)){
    begin_op();
    iput(curproc->cwd);
    end_op();
    curproc->cwd = 0;
  }

  // for(fd = 0; fd < NOFILE; fd++){
  //   if(curproc->ofile[fd]){
  //     fileclose(curproc->ofile[fd]);
  //     curproc->ofile[fd] = 0;
  //   }
  // }
  

  

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  if(curproc->join_caller){
    // cprintf("exit: (%d,%d,%d) was joining on (%d,%d,%d)\n", curproc->join_caller->tid, curproc->join_caller->pid, curproc->join_caller->parent->pid, curproc->tid, curproc->pid, curproc->parent->pid);
    wakeup1(curproc->join_caller);
  } 
  else if(curproc->wait_caller){
    wakeup1(curproc->wait_caller);
  }
  else if(curproc->parent) {
    // cprintf("wakeup1(tid:%d, pid:%d)\n", curproc->parent->tid, curproc->parent->pid);
    wakeup1(curproc->parent);
  }
  

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc ){
      // cprintf("exit(): (tid:%d)->parent == currproc(tid:%d)\n", p->tid, curproc->tid);
      // cprintf("exit(): parent(%d,%d,%d) has child(%d,%d,%d) on which it has not called wait()\n", curproc->tid, curproc->pid, curproc->parent->pid, p->tid, p->pid, p->parent->pid);
      // kill_all_childs();
      struct proc *tp;
      int flag = 0;
      for(tp = ptable.proc; tp< &ptable.proc[NPROC];tp++){
        if(tp->pid == curproc->pid && tp->tid != p->tid){
          // cprintf("exit: There are still  threads in thread group of currproc. so they are not abandoned. No p->parent = initproc\n");
          flag = 1;
          break;
        }
      }
      if(!flag) p->parent = initproc;
    }
      if(p->state == ZOMBIE) // If this happens then there will zombie error !!!
        wakeup1(initproc);
  }
  

  // Jump into the scheduler, never to return.
  // cprintf("making (tid:%d, pid:%d) state ZOMBIE\n", curproc->tid, curproc->pid);
  // cprintf("exit: (%d,%d,%d) made ZOMBIE. now calling sched()\n", curproc->tid, curproc->pid, curproc->parent->pid);
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  // cprintf("wait: curproc(%d,%d,%d)\n", curproc->tid, curproc->pid, curproc->parent->pid);
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent->pid == curproc->pid){
        // cprintf("wait:  wait called on (%d,%d,%d)\n", curproc->pid,p->tid, p->tid, p->parent->pid);
        p->wait_caller = curproc;
      }
      else if(p->parent != curproc)
        continue;
      
      //  if(p->parent != curproc)
      //   continue;
      havekids = 1;
      // p->wait_caller = curproc;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        p->wait_caller = 0;
        p->join_caller = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}


/*  All of the threads in a process are peers: any thread can join
    with any other thread in the process.
*/
int join(int thread_id, void **join_ret){
    struct proc *p;
    int haveKids;
    struct proc *curproc = myproc();
    // cprintf("join: curproc(%d,%d,%d) flags:%d joined_thread id: %d\n", curproc->tid, curproc->pid, curproc->parent->pid,curproc->flags,  thread_id);
    acquire(&ptable.lock);
    for(;;){
      haveKids = 0;
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        // if(p->parent != curproc && p->pid != thread_id) 
        //   continue;
        // if((curproc->flags & CLONE_THREAD) ){
        //   if(p->tid != thread_id  || p->pid != curproc->pid) 
        //   continue;
        // }
        // else{
        //   if(p->parent != curproc ){
        //     continue;
        //   }
        // }
        // if(p->tid == thread_id){
        //   cprintf("join(): p->flags: %d\n", p->flags);
        // }
        if(p->tid != thread_id){
            continue;
        }
        // if(p->tid != thread_id  || p->pid != curproc->pid) 
        //   continue;

        if((p->flags & CLONE_THREAD ) && (p->flags & CLONE_VM)){
            // cprintf("join(): CLONE_VM & CLONE_THREAD\n");
            if(p->pid != curproc->pid) continue;
        }
        if(!(p->flags & CLONE_THREAD ) && !(p->flags & CLONE_VM)){
                        
        }
        // if(p->join_caller){
        //   release(&ptable.lock);
        //   return EINVAL; //Another thread is already waiting to join with this thread.
        // }
        // cprintf("join: childThread(%d,%d,%d)\n", p->tid, p->pid, p->parent->pid);      
        haveKids = 1;
        if(curproc->join_caller == p){
          cprintf("deadlock detected\n");
          
          release(&ptable.lock);
          tkill(p->tid);
          int **ptr = (int**)join_ret;
          **ptr = 4;
          // cprintf("*join_ret: %d\n", **ptr);
          exit();
          return EDEADLK; //Deadlock
        }
        p->join_caller = curproc;
        if(p->state == ZOMBIE){
          // cprintf("join: childThread(%d,%d,%d) has run completely\n", p->tid, p->pid, p->parent->pid);
          // pid = p->pid;
          // int tid = p->tid;
          kfree((char*)p->kstack);
          p->kstack = 0;
          if(p->flags & CLONE_VM){
              /*Do not free vm*/
              // cprintf("join(): vm is shared with parent(%d,%d,%d) and child(%d,%d,%d)\n", p->tid, p->pid, p->parent->pid, curproc->tid, curproc->pid, curproc->parent->pid);
          }else{
            // cprintf("join: caller(%d,%d,%d). VM is not shared between caller and child(%d,%d,%d) . so freevm(child->pgdir).\n",curproc->tid, curproc->pid, curproc->parent->pid, p->tid,p->pid, p->parent->pid);
            freevm(p->pgdir);
          }
          p->pid = 0;
          p->tid = 0;
          p->parent = 0;
          p->name[0] = 0;
          p->killed = 0;
          p->state = UNUSED;
          p->join_caller = 0;
          p->wait_caller = 0;
          release(&ptable.lock);
          return 0;
        }
      }

      if(!haveKids || curproc->killed){
        release(&ptable.lock);
        return ESRCH;
      }
      // cprintf("join: childThread is still executing. Calling sleep for parentThread(%d,%d,%d)\n", curproc->tid,curproc->pid,curproc->parent->pid);
      sleep(curproc, &ptable.lock);
    }
    
    return -1; // No thread with the ID thread could be found.
}



//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

int tkill(int tid){
  struct proc *p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->tid == tid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

int tgkill(int tgid, int tid){
  struct proc *p;
  acquire(&ptable.lock);
  // cprintf("tgkill: tgid:%d tid:%d\n", tgid, tid);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == tgid && p->tid == tid ){
      // cprintf("tgkill: killing (%d,%d,%d)\n", p->tid, p->pid, p->parent->pid);
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}


//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int tkillExceptLeader(int tgid){
  struct proc *p;
  cprintf("tkillExceptLeader: tgid:%d\n", tgid);
  acquire(&ptable.lock);

   for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == tgid && p->tid != tgid ){
      // cprintf("tgkill: killing (%d,%d,%d)\n", p->tid, p->pid, p->parent->pid);
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}



// If clone is called with CLONE_VM=0 &* CLONE_THREAD=0 then parent process must call wait() not join()
// Any thread in a thread group can join another thread in same group. i.e. peers can wait for each other
// Use wait when you want to wait for child process to execute.

int clone(void (*fun)(void*), void* argv,void *stack, int flags){
    int tid;
    struct proc *np;
    struct proc *curproc = myproc();
    // cprintf("clone(): flags: %d\n", flags);

  

    if(((flags&CLONE_VM) || (flags&CLONE_THREAD) || (flags&CLONE_FS)) && !(flags&CLONE_THREAD) ){
      return -1;
    }

    if((np = allocproc()) == 0){
        // cprintf("allocproc failed\n");
        // panic("allocproc failed\n");
        return -1;
    }

  if(flags & CLONE_PARENT) {
      np->parent = curproc->parent;
  }else{
      np->parent = curproc;
  }
    

  if(flags & CLONE_THREAD){
      np->pid = curproc->pid;
      np->parent = curproc->parent;
      np->join_caller = curproc;
  }else{
      np->parent = curproc;
  }

  if(flags & CLONE_VM){
      np->pgdir = curproc->pgdir;
      np->shared_resources = np->shared_resources | SHARED_VM;
  }
  else{
    if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0)
    {
          kfree(np->kstack);
          np->kstack = 0;
          np->state = UNUSED;
          return -1;
    }
  }

    np->sz = curproc->sz;
    uint ustack[2];
    ustack[0] = 0xffffffff;
    ustack[1] = (uint)argv;
    *np->tf = *curproc->tf;
    uint sp = (uint)stack + PGSIZE;
    np->tf->esp = sp;
    np->tf->esp -= 8;
    np->tf->eip = (uint)fun;
    np->tf->eax = 0;

    if(copyout(np->pgdir, np->tf->esp, ustack, 8) < 0){
      cprintf("clone: copyout() failed\n");
      goto bad;
    }

    for (uint i = 0; i < NOFILE; i++)
      if (curproc->ofile[i])
        np->ofile[i] = filedup(curproc->ofile[i]);

    np->cwd = idup(curproc->cwd);

    np->flags = flags;
    tid = np->tid;
    acquire(&ptable.lock);
    np->state = RUNNABLE;
    release(&ptable.lock);
    return tid;

  bad:
    return -1;
}





