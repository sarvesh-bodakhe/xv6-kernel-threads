// Test fot kernel threads
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int stack[4096] __attribute__ ((aligned (4096)));
int stack2[4096];


void fun(void){
    printf(1, "Hi sarvesh. IN function fun()\n");
    // return 0;
}


int main(){
    printf(1, "in thread_test.c  pid:%d  \n", getpid());
    // printf(1, "stack2:%d\n", stack2);
    void (*funptr)(void) = &fun;
    // int thread_id = thread_create((void*)funptr, (void*)0);
    // if(thread_id < 0){
    //     printf(1, "Thread creation failed\n");
    // }else if(thread_id==0){
    //     printf(1, "in child thread\n");
    // }else{
    //     printf(1, "in parent thread\n");
    // }
    // printf(1, "after thread code. Exiting!!!\n");

    
    // printf(1, "\n\n\n");
    // int x = 0;
    // int stack = 1234;
    
    
    
    char *oldbrk = sbrk(4096);
    // printf(1, "oldbrk:%p\n", oldbrk);
    char *newbrk = sbrk(0);
    printf(1, "oldbrk:%p newbrk:%p\n",oldbrk, newbrk);
    char *stack_ptr = (char*)malloc(4096);
    printf(1, "stackptr for new thread:0x%p\n", stack_ptr);
    void *argv = 0;

    printf(1,"Calling clone with: funptr:%p argv:%p stack_ptr:%p\n", (void*)funptr, (void*)argv, (void*)stack_ptr);
    int clone_ret = clone((void*)funptr, (void*) argv,(void*)stack_ptr);
     if(clone_ret < 0){
         printf(1, "clone failed");
     }else if(clone_ret == 0){
         printf(1, "in thread\n");
     }else if(clone_ret > 0){
        printf(1, "in parent. therad_id=%d\n", clone_ret);
     }


    exit();
}
