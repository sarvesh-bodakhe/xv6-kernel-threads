#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void printInfo(){
    printf(1, "(%d,%d,%d)\n", gettid(), getpid(), getppid());
    return;
}

void fun1(void *argv){
    sleep(50);
    printf(1, "*** \n");
    printInfo();
    
    sleep(100);
    printf(1, "*** \n");
    exit();
}

void fun2(void *argv){
    sleep(100);
    printf(1, "### \n");
    printInfo();
    // sleep(100);
     
    sleep(150);
    printf(1, "### \n"); 
    
    exit();
}

void test_tgkill(){
    printf(1, "TEST_TGKILL\n"); 
    printInfo();

    int pid;
    if((pid= fork()) == 0){ //4
        printInfo();
        char *stack = malloc(4096);
        int tid1 = clone(&fun1, 0, stack, CLONE_THREAD | CLONE_VM); //5
        char *stack2 = malloc(4096);
        int tid2 = clone(&fun2, 0, stack2, CLONE_THREAD | CLONE_VM); //6
        printf(1, "tid1:%d tid2:%d\n", tid1, tid2);
        tgkill(getpid(), tid1);
        // sleep(150);
        tgkill(getpid(), tid2);
        tgkill(getpid(), tid2);
        
        // join(tid1, 0);
        join(tid1, 0);
        join(tid2, 0);
        
        // char *stack3 = malloc(4096);
        // int tid3 = clone(&fun2, 0, stack3, CLONE_THREAD | CLONE_VM); //6
        // join(tid3, 0);
        // printf(1, "$$$$\n");
        // tid2 = tid2 + 0;
        // tid1 = tid1 + 0;
        exit();

    }
    wait();
    printInfo();
    exit();
}

int  global_var;


void fun_mem(void *argv){
    printInfo();
    printf(1, "global_val: %d\n", global_var);
    global_var++;
    exit();
}

void test_mem(void){
    printInfo();
    char *stack1 = malloc(4096);
    int tid = clone(&fun_mem, 0, stack1, CLONE_VM | CLONE_THREAD);
    join(tid, 0);

    char *stack2 = malloc(4096);
    int tid2 = clone(&fun_mem, 0, stack2, CLONE_VM | CLONE_THREAD);
    join(tid2, 0);
    exit();
}

int main(){
    // test_tgkill();
    test_mem();
    exit();
}