#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void printInfo(){
    printf(1, "(tid:%d tgid:%d ppid:%d)\n", gettid(), getpid(), getppid());
    return;
}

void fun1(void* argv){
    sleep(50);
    printf(1, "--------------------\n");
    printInfo();
    printf(1, "--------------------\n");
    sleep(50);
    exit();
}

void fun2(void* argv){
    printf(1, "*********************\n");
    printInfo();
    printf(1, "*********************\n");
    sleep(50);
    exit();
}

void fun3(void *argv){
    printf(1, "$$$$$$$$$$$$$$$$$$$$$$$\n");
    printInfo();
    printf(1, "$$$$$$$$$$$$$$$$$$$$$$$\n");
    sleep(50);
    char *stack = malloc(4096);
    int tid = clone(&fun1, 0,stack, CLONE_THREAD | CLONE_VM);
    join(tid, 0);
    exit();
}

int main()
{
    printInfo();
    int pid;
    if((pid = fork()) ==0){
        char* stack = malloc(4096);
        printInfo();
        sleep(50);
        int tid = clone(&fun1, 0, (void *)stack, CLONE_THREAD);
        // int tid = clone(&fun1, 0, (void *)stack, CLONE_THREAD | CLONE_VM);
        join(tid,0);
        // join(clone(&fun1, 0, (void *)stack, CLONE_THREAD | CLONE_VM), 0);
        // join(clone(&fun1, 0, (void *)stack, CLONE_THREAD | CLONE_VM), 0);
        // int pid;
        // if((pid=fork()) == 0){
        //     sleep(50);
        //     printInfo();
        //     join(clone(&fun3, 0, (void *)stack,  0), 0);
        // }else{
        //     wait();
        //     printInfo();
        // }
        exit();
    }else{
        wait();
        printInfo();
    }
    
    // sleep(50);
    // char* stack = malloc(4096);
    // int tid = clone(&fun2, 0, (void *)stack, CLONE_THREAD | CLONE_VM);
    // join(tid, 0);
    // sleep(50);
    // tid += 0;

    // sleep(50);
    // char* stack = malloc(4096);
    // int tid = clone(&fun2,0, stack, 0);
    // join(tid, 0);
    

    // printInfo();
    printf(1, "*** Exiting First Parent\n");
    exit();
}
