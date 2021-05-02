#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "pthread.c"

void fun(void* argv){
    printf(1, "fun() start\n");
    sleep(200);
    printf(1, "Exiting fun()\n");
    exit();
}

void fun2(void* argv){
    printf(1, "fun2() start\n");
    sleep(200);
    printf(1, "Exiting fun2()\n");
    exit();
}

int main(){
    // void (*funptr)(void*) = &fun;
    // int num = 10;
    // printf(1, "Calling thread_create()\n");
    // int join_ret;
    // int t1 = pthread_create((void*)funptr, (void*)&num, CLONE_THREAD | CLONE_VM);
    // thread_join(t1, &join_ret);
    // funptr = &fun2;
    // int t2= pthread_create((void*)funptr, &num, CLONE_THREAD | CLONE_VM);
    // pthread_join(t2, &join_ret);
    // printf(1, "Exiting Parent thread\n");
    exit();
}
