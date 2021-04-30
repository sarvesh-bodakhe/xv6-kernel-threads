#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


void fun3(void *argv){
    printf(1, "(%d-%d-%d)\n", gettid(), getpid(), getppid());
    sleep(100);
    printf(1, "--------------------\n");
    // printf(1, "Exiting fun2()\n");
    
    exit();
}


void fun5(void *argv){
    printf(1, "(%d-%d-%d)\n", gettid(), getpid(), getppid());
    sleep(100);
    printf(1, "++++++++++++++++++++\n");
    // char* stack = malloc(4096);
    // int tid = clone(&fun3, argv, (void *)stack, CLONE_VM | CLONE_THREAD );
    // join(tid,0);
    // tid = tid + 0;
    exit();
}

void fun4(void *argv){
    printf(1, "(%d-%d-%d)\n", gettid(), getpid(), getppid());
    sleep(100);
    printf(1, "___________________\n");
    // printf(1, "Exiting fun2()\n");
    char* stack = malloc(4096);
    int tid = clone(&fun5, argv, (void *)stack, 0);
    // join(tid,0);
    wait();
    tid = tid + 0;
    exit();
}
void fun2(void *argv){
    printf(1, "(%d-%d-%d)\n", gettid(), getpid(), getppid());
    sleep(100);
    printf(1, "===================\n");
    // printf(1, "Exiting fun2()\n");
    
    char* stack = malloc(4096);
    int tid = clone(&fun3, argv, (void *)stack, CLONE_THREAD | CLONE_VM );
    join(tid,0);
    tid = tid + 0;
    char* stack2 = malloc(4096);
    int tid2 = clone(&fun4, argv, (void *)stack2, CLONE_THREAD | CLONE_VM );
    join(tid2,0);
    tid2 = tid2 + 0;
    exit();
}


void fun(void *argv)
{
    sleep(100);
    printf(1, "(%d-%d-%d)\n", gettid(), getpid(), getppid());
    printf(1, "******************\n");
    // sleep(300);
    // printf(1, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    // sleep(200);
    // printf(1, "Exiting fun1()\n");
    
    char* stack = malloc(4096);
    int tid = clone(&fun2, argv, (void *)stack, CLONE_THREAD | CLONE_VM );
    join(tid,0);
    tid = tid && 0;
    exit();
}


int main()
{
    printf(1, "(%d-%d-%d)\n", gettid(), getpid(), getppid());
    char* stack = malloc(4096);
    void *argv = 0;
    int tid = clone(&fun, argv, (void *)stack, CLONE_THREAD | CLONE_VM );
    // sleep(100);
    // tkill(tid);
    join(tid, 0);
    
    // tid = clone(&fun2, argv, stack,CLONE_THREAD | CLONE_VM);
    // join(tid, 0);
    printf(1, "\n##############\n");
    sleep(50);
    printf(1, "Exiting Parent thread\n");
    exit();
}
