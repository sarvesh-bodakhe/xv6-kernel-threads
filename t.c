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

void test_file_util1_close(void *argv){
    // printf(1, "In function\n");
    // printInfo();
    // printf(1, "fd1:%d fd2:%d\n", fd1,fd2);
    printOpenFiles();
    close(fd1);
    // close(fd2);
    // printf(1, "after close(%d)\n", fd1);
    // printOpenFiles();
    sleep(50);
    exit();
}

void test_file_util1(void){
    printf(1, "######### test_file_util1 START\n\n");
    printInfo();
    fd1 = open("README" , O_CREATE | O_RDWR);
    // printf(1, "fd1:%d ", fd1);
    fd2 = open("README2" , O_CREATE | O_RDWR);
    // printf(1, "fd2:%d\n", fd2);
    printOpenFiles();
    char *stack1 = malloc(4096);
    int tid1 = clone(&test_file_util1_close, 0, stack1, CLONE_THREAD | CLONE_VM | CLONE_FILES);
    join(tid1, 0);
    printf(1, "After join(%d)\n", tid1);
    printOpenFiles();
    printf(1, "\n\n######### test_file_util1 PASSED\n\n");
    return;
}

void test_file_util1_open(void *argv){
    printOpenFiles();
    fd2 = open("README2" , O_CREATE | O_RDWR);
    printOpenFiles();
    // printf(1, "fd2:%d\n", fd2);
    sleep(50);
    // printOpenFiles();
    exit();
}

void test_file_util2(void){
    printf(1, "######### test_file_util2 START\n\n");
    printInfo();
    // fd1 = open("README" , O_CREATE | O_RDWR);
    // printf(1, "fd1:%d ", fd1);
    printOpenFiles();
    char *stack1 = malloc(4096);
    int tid1 = clone(&test_file_util1_open, 0, stack1, CLONE_THREAD | CLONE_VM | CLONE_FILES);
    join(tid1, 0);
    printf(1, "After join(%d)\n", tid1);
    printOpenFiles();
    printf(1, "\n\n######### test_file_util2 PASSED\n");
    return;
}

void test_file(){
    test_file_util1();
    test_file_util2();

}

int main(){
    // test_tgkill();
    test_file();
    exit();
}