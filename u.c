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
    printf(1, "%s\n");
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



void test_pthread_util1(void *argv){
    // printInfo();
    // printf(1, "######### \n");
    sleep(10);
    exit();
}

void test_pthread(void){
//    printf(1,"\n\n"); printChar('-',10); printf(1, "test_pthread START\n\n");
//    printStartTest("TEST_PTHREAD_01", "No description");
        printf(1, "normal pthread_create() and pthread_join(): test\n");
    pthread_t thread;
    // pthread_create(thread, )
    pthread_create(&thread, &test_pthread_util1, 0, CLONE_THREAD | CLONE_VM);
    pthread_join(thread, 0);
    // printf(1, "\n\n");
    // printChar('-',20);printf(1, "test_pthread PASSED");
    // printChar('-',20);
    // printEndTest("TEST_PTHREAD_Util1", "No description", 1);
    printf(1, "normal pthread_create() and pthread_join(): ok\n");
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
    // printStartTest("TEST_MATRIX_MUL", "Matrix multiplication using threads");
    printf(1, "Multithreaded Matrix multiplication: test\n");

    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            // matA[i][j] = rand() % 10;
            // matB[i][j] = rand() % 10;
            matA[i][j] = i+j+1;
            matB[i][j] = i+j+2;
        }
    }

    // printf(1, "\nMatrix A:\n");
    // for (int i = 0; i < MAX; i++) {
    //     for (int j = 0; j < MAX; j++) 
    //         printf(1, "%d ", matA[i][j]);
    //     printf(1, "\n");
    // }

    // printf(1, "\nMatrix B:\n");    
    // for (int i = 0; i < MAX; i++) {
    //     for (int j = 0; j < MAX; j++) 
    //         printf(1, "%d ", matB[i][j]);
    //     printf(1, "\n");
    // }

    pthread_t threads[MAX_THREAD];
    for (int i = 0; i < MAX_THREAD; i++) {   
        pthread_create(&threads[i],  multi, 0,  CLONE_THREAD | CLONE_VM );
    }
    for (int i = 0; i < MAX_THREAD; i++) 
        pthread_join(threads[i], 0);  

    // printf(1, "\nMultiplication of A and B\n");
    // for (int i = 0; i < MAX; i++) {
    //     for (int j = 0; j < MAX; j++) 
    //         printf(1, "%d ", matC[i][j]);
    //     printf(1, "\n");
    // }

    // printf(1, "\nSolution\n");
    // for (int i = 0; i < MAX; i++) {
    //     for (int j = 0; j < MAX; j++) 
    //         printf(1, "%d ", matSolution[i][j]);
    //     printf(1, "\n");
    // }


    for(int i=0;i<MAX;i++)
        for(int j=0;j<MAX; j++)
            if(matSolution[i][j] != matC[i][j]){
                printf(1, "Multithreaded Matrix multiplication: failed\n");
                return;
            }

    printf(1, "Multithreaded Matrix multiplication: ok\n");
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
    // printStartTest("TEST_JOIN_01", "Creating 25 threads, where each thread increments a global value.");
    printf(1, "crearing multiple threads and joining: test\n");
    int val = 0;
    for(int i=0;i<25;i++){
        pthread_t tid;
        int ret;
        ret = pthread_create(&tid, &test_join01_util, &val, CLONE_THREAD | CLONE_VM );
        if(ret <0){
            printEndTest("TEST_JOIN_01", "", 0);   
            return ;
        }
        pthread_join(tid, 0);
    }
    // if(val == 25) printEndTest("TEST_JOIN_01", "", 1);
    // else printEndTest("TEST_JOIN_01", "", 0);
    if(val == 25) printf(1, "crearing multiple threads and joining: ok\n");
    else printf(1, "crearing multiple threads and joining: failed\n");
    return;
}


struct arg{
    // int tid;
    pthread_t tid;
    int value;
};

void test_join02_util1(void *argv){
    sleep(10);
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
    // printStartTest("TEST_JOIN_02", "Creating 2 child threads. And joining a thread from another thread(Not a leader).");
    printf(1, "joining thread from another thread: test\n");
    pthread_t tid1, tid2;
    struct arg argument;
    if(pthread_create(&tid1, &test_join02_util1, &argument, CLONE_THREAD | CLONE_VM) < 0) return;
    argument.tid = tid1;
    if(pthread_create(&tid2, &test_join02_util2, &argument, CLONE_THREAD | CLONE_VM) < 0) return;
    pthread_join(tid2, 0);

    // printf(1, "value:%d\n", argument.value);
    if(argument.value == 100) printf(1, "joining thread from another thread: okay\n");
    else printf(1, "joining thread from another thread: failed\n");
    return;
}

void test_join03_util1(void *argv){
    sleep(10);
    exit();
}

void test_join03(void){
    // printStartTest("TEST_JOIN_03", "Joining a thread which is created without any CLONE flag.(i.e. forked)\n");
    printf(1, "joining thread/process created without any CLONE Flag: test\n");
    pthread_t tid;
    if(pthread_create(&tid, &test_join03_util1, 0, 0) < 0){
        printf(1, "Error: clone()\n");
        printEndTest("TEST_JOIN_03", "", 0);
        return;
    }
    int *retval;
    pthread_join(tid, (void**)&retval);
    printf(1, "joining thread/process created without any CLONE Flag: ok\n");
    return;
}

void test_fork_wait_01_util1(void *argv){
    int pid;
    if((pid = fork()) < 0){
        printf(1, "wait in thread for a process created by fork() in another thread: failed\n");
        return;
    }else if(pid == 0){
        sleep(50);
        exit();
    }
    exit();
}
void test_fork_wait_01_util2(void *argv){
    sleep(10);
    wait();
    // printf(1, "Wait finished for a process which was created by another thread\n");
    exit();
}


void test_fork_wait_01(void){
    // printStartTest("TEST_WAIT_01", "1.Create a new process using fork in a thread\n2.Wait for newly created thread in another thread within the same thread group as of called of fork()");
    printf(1, "wait in thread for a process created by fork() in another thread: test\n");
    pthread_t tid1, tid2;
    if(pthread_create(&tid1, &test_fork_wait_01_util1, 0, CLONE_THREAD | CLONE_VM) < 0){
        printf(1, "wait in thread for a process created by fork() in another thread: failed\n");
        return;
    }
    if(pthread_create(&tid2, &test_fork_wait_01_util2, 0, CLONE_THREAD | CLONE_VM) < 0){
        printf(1, "wait in thread for a process created by fork() in another thread: failed\n");
        return;
    }
    pthread_join(tid2,0);
    pthread_join(tid1,0);
    printf(1, "wait in thread for a process created by fork() in another thread: ok\n");
    return;
}

void test_CLONE_FILES_01_util01(void *argv){
    int fd = open("README", O_CREATE | O_RDWR);
    int *ptr = (int*)argv;
    *ptr = fd;
    exit();
}

void test_CLONE_FILES_01(void){
    // printStartTest("test_CLONE_FILES_01", "CLONE_FILES: opening a file in child thread");
    printf(1, "CLONE_FILES . opening a file in child thread and reading in parent: test\n");
    int fd;
    pthread_t tid;
    if(pthread_create(&tid, &test_CLONE_FILES_01_util01, &fd, CLONE_THREAD | CLONE_VM | CLONE_FILES) < 0){
        printf(1, "CLONE_FILES . opening a file in child thread and reading in parent: failed\n");
        return;
    }
    pthread_join(tid, 0);

    char str[8];
    int byteRead = read(fd, str, 8);
    if(byteRead > 0) printf(1, "CLONE_FILES . opening a file in child thread and reading in parent: ok\n");
    else printf(1, "CLONE_FILES . opening a file in child thread and reading in parent: failed\n");
}




void test_tgkill_01_util1(void *argv){
    int *ptr = (int*) argv;
    for(int i=0;i<20;i++){
        *ptr = i+1;
        sleep(5);
        // printf(1, "**\n");
    }
    exit();
}

void test_tgkill_01(){
    // printStartTest("TEST_TGKILL_01", "Leader of a thread group calling tgkill() to kill a thread");
    printf(1, "tgkill(). leader of the thread group calling tgkill(): test\n");
    int count = 0;
    pthread_t tid;
    if(pthread_create(&tid, &test_tgkill_01_util1, &count, CLONE_THREAD | CLONE_VM) < 0){
        printf(1, "tgkill(). leader of the thread group calling tgkill(): failed\n");
        return;
    }
    sleep(20);
    tgkill(getpid(), tid.tid);
    // printf(1, "\ncount:%d\n", count);
    if(count != 20) printf(1, "tgkill(). leader of the thread group calling tgkill(): ok\n");
    else printf(1, "tgkill(). leader of the thread group calling tgkill(): failed\n"); 
}

void test_tgkill_02_util1(void *argv){
    int *ptr = (int*) argv;
    for(int i=0;i<20;i++){
        *ptr = i+1;
        sleep(5);
        // printf(1, "**\n");
    }
    exit();
}

void test_tgkill_02_util2(void *argv){
    sleep(20);
    int *ptr = (int*)argv;
    tgkill(getpid(), *ptr);
    exit();
}

void test_tgkill_02(void){
    // printStartTest("TEST_TGKILL_02", "peer thread  of a thread group calling tgkill() to kill a thread");
    printf(1, "peer thread calling tgkill() to kill sibiling thread: test\n");
    int count = 0;
    pthread_t tid1, tid2;
    if(pthread_create(&tid1, &test_tgkill_02_util1, &count, CLONE_THREAD | CLONE_VM) < 0){
        printf(1, "peer thread calling tgkill() to kill sibiling thread: failed\n");
        return;
    }
    if(pthread_create(&tid2, &test_tgkill_02_util2, &tid1, CLONE_THREAD | CLONE_VM) < 0){
        printf(1, "peer thread calling tgkill() to kill sibiling thread: failed\n");
        return;
    }
    pthread_join(tid2, 0);
    // printf(1, "\ncount:%d\n", count);
    if(count != 20) printf(1, "peer thread calling tgkill() to kill sibiling thread: ok\n");
    else printf(1, "peer thread calling tgkill() to kill sibiling thread: failed\n");  
}

void test_tkill_01_util01(void *argv){
    int *ptr = (int*) argv;
    for(int i=0;i<10;i++){
        *ptr = i+1;
        sleep(50);
        printf(1, "**\n");
    }
    exit();
}

void test_tkill_01(){
    printStartTest("TEST_TGKILL_01", "Leader of a thread group calling tgkill() to kill a thread");
    int count = 0;
    pthread_t tid;
    if(pthread_create(&tid, &test_tkill_01_util01, &count, CLONE_THREAD | CLONE_VM) < 0){
        printEndTest("TEST_TGKILL_01", "Error clone()", 0);
        return;
    }
    sleep(300);
    tkill(tid.tid);
    // printf(1, "\ncount:%d\n", count);
    if(count != 10) printEndTest("TEST_TGKILL_01", "Thread Killed using tgkill\n", 1);
    else printEndTest("TEST_TGKILL_01", "Unable to kill a thread using tgkill\n", 0);    
}

void test_tkill_02_util1(void *argv){
    int *ptr = (int*) argv;
    for(int i=0;i<10;i++){
        *ptr = i+1;
        sleep(100);
        printf(1, "**\n");
    }
    exit();
}

void test_tkill_02_util2(void *argv){
    sleep(200);
    int *ptr = (int*)argv;
    tkill(*ptr);
    exit();
}

void test_tkill_02(void){
    printStartTest("TEST_TKILL_02", "peer thread  of a thread group calling tgkill() to kill a thread");
    int count = 0;
    pthread_t tid1, tid2;
    if(pthread_create(&tid1, &test_tkill_02_util1, &count, CLONE_THREAD | CLONE_VM) < 0){
        printEndTest("TEST_TKILL_02", "Error clone()", 0);
        return;
    }
    if(pthread_create(&tid2, &test_tkill_02_util2, &tid1, CLONE_THREAD | CLONE_VM) < 0){
        printEndTest("TEST_TKILL_02", "Error clone()", 0);
        return;
    }
    pthread_join(tid2, 0);
    // printf(1, "\ncount:%d\n", count);
    if(count != 10) printEndTest("TEST_TKILL_02", "Thread Killed using tgkill\n", 1);
    else printEndTest("TEST_TKILL_02", "Unable to kill a thread using tgkill\n", 0);    
}


void test_getMethods_01(){
    printStartTest("TEST_GETTER_METHODS", "test for methods like gettid(), getpid(), getppid()");
    printf(1, "gettid(): %d, getpid:%d, getppid():%d\n", gettid(), getpid(), getppid());
    printEndTest("TEST_GETTER_METHODS", "all getter methods working fine", 1);
}

void test_getMethods_02_util01(void *argv){
    // printChar(' ', 5); printf(1, "Cloned process: "); printInfo();
    exit();
}

void test_getMethods_02_util02(void *argv){
    printChar(' ', 5); printf(1, "Cloned process: "); printInfo();
    printChar(' ', 5); printf(1, "Calling fork in thread\n");
    int pid;
    pid = fork();
    if(pid == 0){
        // sleep(100);
        printChar(' ', 15); printf(1, "In forked process from a thread: "); printInfo();
        printChar(' ', 15); printf(1, "Creating thread with CLONE_THREAD flag:\n");
        pthread_t t1, t2;
        printChar(' ', 20);pthread_create(&t1, &test_getMethods_02_util01, 0, CLONE_THREAD | CLONE_VM);
        pthread_join(t1, 0);
        printChar(' ', 20);pthread_create(&t2, &test_getMethods_02_util01, 0, CLONE_THREAD | CLONE_VM);
        pthread_join(t2, 0);
        // sleep(100);
        exit();
    }else{
        wait();
    }
    exit();
}

void test_getMethods_02(){
    printStartTest("TEST_GETTER_METHODS", "test for methods - gettid(), getpid(), getppid() with clone() calls");
    printf(1, "Starting process: ");printInfo();
    printf(1, "Creating thread with CLONE_THREAD flag:\n");
    pthread_t t1,t2, t3;
    pthread_create(&t1, &test_getMethods_02_util01, 0, CLONE_THREAD | CLONE_VM);
    pthread_join(t1, 0);
    pthread_create(&t2, &test_getMethods_02_util01, 0, CLONE_THREAD | CLONE_VM);
    pthread_join(t2, 0);
    pthread_create(&t3, &test_getMethods_02_util02, 0, CLONE_THREAD | CLONE_VM);
    pthread_join(t3, 0);
    printf(1, "\nForking another process: ");
    int pid;
    if((pid = fork()) == 0){
        // sleep(50);
        printInfo();
        exit();
    }else{
        wait();
    }   
}

void test_CLONE_FS_01_util01(void *argv){
    // printInfo();
    // int dirp;
    mkdir("newdir");
    chdir("newdir");
    int dirp = getcwdi();
    int *ptr = (int*)argv;
    *ptr = dirp;
    
    
    // printf(1, "getcwdi in child:%d\n", getcwdi());
    // sleep(100);
    exit();
}

void test_CLONE_FS_01(){
    // printStartTest("TEST_CLONE_FS_01", "clone() with CLONE_FS flag enabled");
    printf(1, "clone() with CLONSE_FS flag: test\n");
    // printInfo();
    // printf(1, "getcwdi in parent:%d\n", getcwdi());
    pthread_t tid;
    int dirp;
    if(pthread_create(&tid, &test_CLONE_FS_01_util01, (void*)&dirp, CLONE_THREAD | CLONE_VM | CLONE_FS) < 0){
        printf(1, "clone() with CLONSE_FS flag: failed\n");
        return;
    }
    int *retval;
    
    pthread_join(tid, (void**)&retval);
    // printf(1, "getcwdi in parent after join:%d\n", getcwdi());
    // printf(1, "dirp:%d\n", dirp);
    if(getcwdi() == dirp) printf(1, "clone() with CLONSE_FS flag: ok\n");
    else printf(1, "clone() with CLONSE_FS flag: failed\n");
    return;
}

void test_CLONE_FS_02_util01(void *argv){
    // printInfo();
    mkdir("newdir");
    chdir("newdir");
    int dirp = getcwdi();
    int *ptr = (int*)argv;
    *ptr = dirp;
    exit();
}

void test_CLONE_FS_02(){
    
    printf(1, "clone() without CLONE_FS flag: test\n");
    pthread_t tid;
    int dirp;
    if(pthread_create(&tid, &test_CLONE_FS_02_util01, &dirp, CLONE_THREAD | CLONE_VM) < 0){
        printf(1, "clone() without CLONE_FS flag: failed\n");
        return;
    }
    pthread_join(tid, 0);
    if(dirp != getcwdi()) printf(1, "clone() without CLONE_FS flag: ok\n");
    else printf(1, "clone() without CLONE_FS flag: failed\n");
    // printf(1, "getcwdi in parent after join:%d\n", getcwdi());
    
    return;
}


void deadlock_fun1(void *argv){
    sleep(100);
    // int tid;
    // int *ptr = (int*)argv;
    pthread_t thread = *((pthread_t*)argv);
    // tid = *ptr;

    int retval;
    pthread_join(thread, (void*)&retval);
}

void deadlock_fun2(void *argv){
    sleep(150);
    // int tid;
    // int *ptr = (int*)argv;
    // tid = *ptr;
    // pthread_t *thread = (pthread_t*)argv;
    pthread_t thread = *((pthread_t*)argv);
    int retval;
    pthread_join(thread, (void*)&retval);
}


void test_join_04(void){
    printf(1, "deadlock: two threads joining each other: test\n");
    pthread_t t1,t2;
    pthread_create(&t1, &deadlock_fun1, &t2, CLONE_THREAD | CLONE_VM);
    pthread_create(&t2, &deadlock_fun2, &t1, CLONE_THREAD | CLONE_VM);
    int **retval =0;
    pthread_join(t1, (void**)retval);
    // printf(1, "retval:%d\n", **retval);   
    pthread_join(t2,(void**)retval);
    // printf(1, "retval:%d\n", **retval);   
    printf(1, "deadlock: two threads joining each other: ok\n");
}

struct arg_t{
    int  cwdinum;
    int fd;
};

void test_CLONE_NOFLAGS_util01(void* argv){
    sleep(10);
    exit();
}



void test_CLONE_NOFLAGS(void){
    // printStartTest("TEST_CLONE_NO_FLAGS", "");
    printf(1, "joining a thread created with no clone flags: test\n");
    pthread_t thread;
    struct arg_t arg;
    if(pthread_create(&thread, &test_CLONE_NOFLAGS_util01, &arg, 0));
    int *retval;
    pthread_join(thread,(void**)&retval);
    printf(1, "joining a thread created with no clone falgs: ok\n");
    return;
}

void test_function(void *argv){
    // printInfo();
    sleep(10);
    exit();
}

void test_CLONE_VM_without_thread(){
    printf(1, "using CLONE_VM without CLONE_THREAD flag: test\n");
    pthread_t thread;
    int ret = pthread_create(&thread, &test_function, 0, CLONE_VM);
    if(ret < 0) printf(1, "using CLONE_VM without CLONE_THREAD flag: ok\n");
    else printf(1, "using CLONE_VM without CLONE_THREAD flag: failed\n");
}



void test_join_threadNotFound(){
    printf(1, "joining thread while thread with id not present : test\n");
    pthread_t t;
    pthread_create(&t, &test_function, 0, CLONE_THREAD | CLONE_VM);
    int **retval=0;
    if(pthread_join(t, (void**)retval) < 0) {
        printf(1, "Join Error\n");
        return;
    }
    int ret = pthread_join(t, (void**)retval) ;
    switch(ret){
        case ESRCH:
            printf(1, "joining thread while thread with id not present : ok\n");
            break;
        default:
            printf(1, "joining thread while thread with id not present : failed\n");

    }
}




int main(){



    
    

    
    
    
    // test_getMethods_01();
    // test_getMethods_02();
    

    //----------------
    test_CLONE_NOFLAGS();
    test_CLONE_FILES_01();
    test_CLONE_FS_01();
    test_CLONE_FS_02();
    
    test_tgkill_01();
    test_tgkill_02();
    test_join01();
    test_join02();
    test_join03();
    test_join_04();
    test_fork_wait_01();
    test_matrix_mul();
    test_pthread();
    test_CLONE_VM_without_thread();
    test_join_threadNotFound();

    

    exit();
}