#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

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
    void (*funptr)(void*) = &fun;
    int num = 10;
    printf(1, "Calling thread_create()\n");
    int join_ret;
    int t1 = thread_create((void*)funptr, (void*)&num);
    thread_join(t1, &join_ret);
    funptr = &fun2;
    int t2= thread_create((void*)funptr, &num);
    thread_join(t2, &join_ret);
    printf(1, "Exiting Parent thread\n");
    exit();
}
