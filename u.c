#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
// #include "pthread.h"
#include "pthread.c"

void printInfo(){
    printf(1, "(tid:%d, pid:%d, ppid:%d)\n", gettid(), getpid(), getppid());
    return;
}

void printChar(char ch, int count){
    for(int i=0;i<count;i++) printf(1, "%c", ch);
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

void printStartTest(char *test_name, char *test_description){
    printf(1,"\n\n");
    printChar('-',20); 
    printf(1, "%s START ", test_name);
    printChar('-',20); 
    printf(1, "\n");    
}

void printEndTest(char* test_name, char *description, int passed){
    printf(1,"\n");
    printChar('-',20); 
    printf(1, "%s", test_name);
    if(passed) printf(1, ": PASSED");
    else printf(1, ": FAILED");
    printChar('-',20); 
    printf(1, "\n");
}


void test_tgkill(){
    printStartTest("TEST_TGKILL", "No description");
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
    printEndTest("TEST_TGKILL", "No description", 1);
    return;
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
    // printf(1,"\n\n");printChar('-',20); 
    // printf(1, " test_file_util1 START\n\n");
    printStartTest("TEST_FILE_UTIL1", "No description");
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
    // printf(1, "\n\n");
    // printChar('-',20);
    // printf(1, " test_file_util1 PASSED");
    // printChar('-',20);

    printEndTest("TEST_FILE_Util1", "No description", 1);
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
    // printf(1,"\n\n");printChar('-',20); 
    // printf(1, " test_file_util2 START\n\n");
    // printChar('-',20); 
    printStartTest("TEST_FILE_Util2", "No description");
    printInfo();
    // fd1 = open("README" , O_CREATE | O_RDWR);
    // printf(1, "fd1:%d ", fd1);
    printOpenFiles();
    char *stack1 = malloc(4096);
    int tid1 = clone(&test_file_util1_open, 0, stack1, CLONE_THREAD | CLONE_VM | CLONE_FILES);
    join(tid1, 0);
    printf(1, "After join(%d)\n", tid1);
    printOpenFiles();
    // printf(1, "\n\n");
    // printChar('-',20);
    // printf(1, "test_file_util2 PASSED");
    // printChar('-',20);
    printEndTest("TEST_FILE_Util2", "No description", 1);
    return;
}

void test_file(){
    test_file_util1();
    test_file_util2();

}

void test_pthread_util1_01(void *argv){
    printInfo();
    printf(1, "######### \n");
    sleep(100);
    exit();
}

void test_pthread_util1(void){
//    printf(1,"\n\n"); printChar('-',10); printf(1, "test_pthread START\n\n");
   printStartTest("TEST_PTHREAD_01", "No description");
    pthread_t thread=0;
    // pthread_create(thread, )
    pthread_create(&thread, &test_pthread_util1_01, 0, CLONE_THREAD | CLONE_VM);
    join(thread, 0);
    // printf(1, "\n\n");
    // printChar('-',20);printf(1, "test_pthread PASSED");
    // printChar('-',20);
    printEndTest("TEST_PTHREAD_Util1", "No description", 1);
}   


#define MAX 4
#define MAX_THREAD 4
int matA[MAX][MAX];
int matB[MAX][MAX];
int matC[MAX][MAX];
int matSolution[MAX][MAX] = {
    {40,50,60,70},
    {54,68,82,96},
    {68,86,104,122},
    {82,104,126,148},
};

int step_i = 0;

unsigned long randstate = 1;
unsigned int
rand()
{
  randstate = randstate * 1664525 + 1013904223;
  randstate += 2;
  return randstate;
}


void multi(void* arg)
{
    int core = step_i++;
  
    // Each thread computes 1/4th of matrix multiplication
    for (int i = core * MAX / 4; i < (core + 1) * MAX / 4; i++) 
        for (int j = 0; j < MAX; j++) 
            for (int k = 0; k < MAX; k++) 
                matC[i][j] += matA[i][k] * matB[k][j];

    exit();
}

void test_matrix_mul(void){
    // printf(1,"\n\n");
    // printChar('-',20); 
    // printf(1, " test_pthread_util2 START\n\n");
    printStartTest("TEST_MATRIX_MUL", "Matrix multiplication using threads");

    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            // matA[i][j] = rand() % 10;
            // matB[i][j] = rand() % 10;
            matA[i][j] = i+j+1;
            matB[i][j] = i+j+2;
        }
    }

    printf(1, "\nMatrix A:\n");
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) 
            printf(1, "%d ", matA[i][j]);
        printf(1, "\n");
    }

    printf(1, "\nMatrix B:\n");    
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) 
            printf(1, "%d ", matB[i][j]);
        printf(1, "\n");
    }

    pthread_t threads[MAX_THREAD];
    for (int i = 0; i < MAX_THREAD; i++) {   
        pthread_create(&threads[i],  multi, 0,  CLONE_THREAD | CLONE_VM );
    }
    for (int i = 0; i < MAX_THREAD; i++) 
        join(threads[i], 0);  

    printf(1, "\nMultiplication of A and B\n");
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) 
            printf(1, "%d ", matC[i][j]);
        printf(1, "\n");
    }

    // printf(1, "\nSolution\n");
    // for (int i = 0; i < MAX; i++) {
    //     for (int j = 0; j < MAX; j++) 
    //         printf(1, "%d ", matSolution[i][j]);
    //     printf(1, "\n");
    // }


    for(int i=0;i<MAX;i++)
        for(int j=0;j<MAX; j++)
            if(matSolution[i][j] != matC[i][j]){
                printEndTest("TEST_MATRIX_MUL", "Matrix Multiplication using threads", 0);            
                return;
            }

    printEndTest("TEST_MATRIX_MUL", "Matrix Multiplication using threads", 1);
    return;
}

void dummy(void *argv){
    // sleep(100);
    // printf(1, "dummy\n");
    for(int i=0;i<50;i++){
        if((i+1)%10 == 0) printf(1, "%d:%d\n", gettid(), i);
        sleep(50);
    }
    exit();
}

void test_stress(void){
    printStartTest("TEST STRESS", "No description now");
    printInfo();
    // pthread_t thread;
    int max = 0;
    int ret = 0;
    int threads[100] = {0};
    while(1){
        // ret = pthread_create(&thread, &dummy, 0, CLONE_THREAD | CLONE_VM);
        char *stack = malloc(4096);
         ret = clone(&dummy, 0, stack, CLONE_THREAD | CLONE_VM);
         threads[max] = ret;
        // printf(1, "%d ret:%d\n", max, ret);    
        if(ret == -1) break;
        max++;
    }
    printf(1, "max:%d\n", max);
    for(int i=0;i<max;i++) join(threads[i], 0);
    printEndTest("TEST STRESS", "", 1);
    return;
}

void test_join01_util(void *argv){    
    int *ptr = (int*)argv;
    *ptr = *ptr + 1;
    exit();
}
void test_join01(void){
    printStartTest("TEST_JOIN_01", "Creating 25 threads, where each thread increments a global value.");
    int val = 0;
    for(int i=0;i<25;i++){
        pthread_t tid;
        int ret;
        ret = pthread_create(&tid, &test_join01_util, &val, CLONE_THREAD | CLONE_VM );
        if(ret <0){
            printEndTest("TEST_JOIN_01", "", 0);   
            return ;
        }
        join(tid, 0);
    }
    if(val == 25) printEndTest("TEST_JOIN_01", "", 1);
    else printEndTest("TEST_JOIN_01", "", 0);
    return;
}


struct arg{
    int tid;
    int value;
};

void test_join02_util1(void *argv){
    sleep(300);
    // int **arr = ((int*)argv);    
    struct arg *argument = (struct arg*) argv;
    argument->value = 100;
    exit();
}

void test_join02_util2(void *argv){
    struct arg *argument = (struct arg*) argv;
    argument->value = 10;
    pthread_join(argument->tid, 0);
    // printf(1, "Joined thread after 300\n");
    exit();
}

void test_join02(void){
    printStartTest("TEST_JOIN_02", "Creating 2 child threads. And joining a thread from another thread(Not a leader).");
    pthread_t tid1, tid2;
    struct arg argument;
    if(pthread_create(&tid1, &test_join02_util1, &argument, CLONE_THREAD | CLONE_VM) < 0) return;
    argument.tid = tid1;
    if(pthread_create(&tid2, &test_join02_util2, &argument, CLONE_THREAD | CLONE_VM) < 0) return;
    pthread_join(tid2, 0);

    // printf(1, "value:%d\n", argument.value);
    if(argument.value == 100) printEndTest("TEST_JOIN_01", "", 1);
    else printEndTest("TEST_JOIN_02", "", 0);
    return;
}

void test_join03_util1(void *argv){
    sleep(300);
    exit();
}

void test_join03(void){
    printStartTest("TEST_JOIN_03", "Joining a thread which is created without any CLONE flag.(i.e. forked)\n");
    pthread_t tid;
    if(pthread_create(&tid, &test_join03_util1, 0, 0) < 0){
        printf(1, "Error: clone()\n");
        printEndTest("TEST_JOIN_03", "", 0);
        return;
    }
    pthread_join(tid, 0);
    printEndTest("TEST_JOIN_03", "", 1);
    return;
}

void test_fork_wait_01_util1(void *argv){
    int pid;
    if((pid = fork()) < 0){
        printEndTest("TEST_WAIT_01", "Unable to fork a new process", 0);
        return;
    }else if(pid == 0){
        sleep(1000);
        exit();
    }
    exit();
}
void test_fork_wait_01_util2(void *argv){
    sleep(10);
    wait();
    printf(1, "Wait finished for a process which was created by another thread\n");
    exit();
}


void test_fork_wait_01(void){
    printStartTest("TEST_WAIT_01", "1.Create a new process using fork in a thread\n2.Wait for newly created thread in another thread within the same thread group as of called of fork()");
    pthread_t tid1, tid2;
    if(pthread_create(&tid1, &test_fork_wait_01_util1, 0, CLONE_THREAD | CLONE_VM) < 0){
        printEndTest("TEST_WAIT_01", "Error clone()", 0);
        return;
    }
    if(pthread_create(&tid2, &test_fork_wait_01_util2, 0, CLONE_THREAD | CLONE_VM) < 0){
        printEndTest("TEST_WAIT_01", "Error clone()", 0);
        return;
    }
    pthread_join(tid2,0);
    pthread_join(tid1,0);
    printEndTest("TEST_WAIT_01", "Made a thread wait for a process created by another thread(which in the same thread group)", 1);
    return;
}

void test_CLONE_FILES_01_util01(void *argv){
    int fd = open("README", O_CREATE | O_RDWR);
    int *ptr = (int*)argv;
    *ptr = fd;
    exit();
}

void test_CLONE_FILES_01(void){
    printStartTest("test_CLONE_FILES_01", "");
    int fd;
    pthread_t tid;
    if(pthread_create(&tid, &test_CLONE_FILES_01_util01, &fd, CLONE_THREAD | CLONE_VM | CLONE_FILES) < 0){
        printEndTest("TEST_CLONE_FILES_01", "Error clone()", 0);
        return;
    }
    pthread_join(tid, 0);

    char str[8];
    int byteRead = read(fd, str, 8);
    if(byteRead > 0) printEndTest("test_CLONE_FILES_01", "", 1);
    else printEndTest("test_CLONE_FILES_01", "", 0);

}

int main(){
    // test_tgkill();
    // test_file();
    // test_pthread_util1();
    // test_pthread_util2();
    // test_stress();    
    // test_join01();
    // test_join02();
    // test_join03();
    // test_matrix_mul();
    // test_fork_wait_01();

    // test_CLONE_FILES_01();
    

    exit();
}