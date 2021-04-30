#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

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


int fd1, fd2;
void test_file_util(void *argv){
    printf(1, "In function\n");
    printInfo();
    printf(1, "fd1:%d fd2:%d\n", fd1,fd2);
    close(fd1);
    sleep(50);
    exit();

}

void test_file(){
    printInfo();
    fd1 = open("README" , O_CREATE | O_RDWR);
    printf(1, "fd1:%d\n", fd1);
    fd2 = open("README2" , O_CREATE | O_RDWR);
    printf(1, "fd2:%d\n", fd2);

    char *stack1 = malloc(4096);
    // int tid1 = clone(test_file_util, 0, stack1, CLONE_THREAD | CLONE_VM );
    int tid1 = clone(test_file_util, 0, stack1, CLONE_THREAD | CLONE_VM | CLONE_FILES);
    join(tid1, 0);
    printf(1, "Exiting test_file()\n");
    exit();

}

int main(){
    test_tgkill();
    // test_file();
    exit();
}