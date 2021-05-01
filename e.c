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

void test1(void){
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
    return;
}

void test2(void){
     sleep(50);
    char* stack = malloc(4096);
    int tid = clone(&fun1, 0, (void *)stack, CLONE_THREAD | CLONE_VM);
    join(tid, 0);
    sleep(50);
    tid += 0;

    sleep(50);
    char* stack2 = malloc(4096);
    tid = clone(&fun2,0, stack2, 0);
    join(tid, 0);
}


void test3_util2(void *argv){
    sleep(50);
    printInfo(); // 6-3-2
    // sleep(1000);
    wait(); 
    printInfo(); printf(1, "wait complete\n");
    exit(); 
}

void test3_util1(void *argv){
    printInfo();  // 4-3-2
    int pid;
    char *stack = malloc(4096);
    
    if((pid = fork()) == 0){
        printInfo(); // 5-5-3
        printf(1, "##########\n");
        sleep(500);
        printf(1, "##########\n");
        exit();
    }
    // wait();  
    int tid = clone(*test3_util2, 0, stack, CLONE_THREAD|CLONE_VM);
    join(tid, 0);
    tid += 0;
    exit();
}

void test3(void ){
    printInfo(); // 3-3-2
    int tid; 
    char *stack1 = malloc(4096);

    tid = clone(&test3_util1, 0, stack1, CLONE_THREAD | CLONE_VM );
    join(tid, 0);
    
    return;
}

int main()
{   
    // test3();
    printf(1, "*** Exiting Main\n");
    exit();
}
