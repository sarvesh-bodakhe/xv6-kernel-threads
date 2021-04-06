// Test fot kernel threads
//

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int stack[4096] __attribute__ ((aligned (4096)));
int stack2[4096];


void fun(void){
    printf(1, "***********************Hi. In function passed to clone: fun()\n");
    exit();
    // return 0;
}


int main(){
    
    void (*funptr)(void) = &fun;
    
    // char *oldbrk = sbrk(4096);
    // printf(1, "oldbrk:%p\n", oldbrk);
    // char *newbrk = sbrk(0);
    // printf(1, "oldbrk:%p newbrk:%p\n",oldbrk, newbrk);

    sbrk(4096);
    char *stack_ptr = (char*)malloc(4096);

    // printf(1, "stackptr for new thread:0x%p\n", stack_ptr);
    void *argv = 0;

    // printf(1,"Calling clone with: funptr:%p argv:%p stack_ptr:%p\n", (void*)funptr, (void*)argv, (void*)stack_ptr);
    int clone_ret = clone((void*)funptr, (void*) argv,(void*)stack_ptr);
    // printf(1,"#####################################clone_ret:%d\n", clone_ret);
    if(clone_ret < 0){
         printf(1, "clone failed");
        //  exit();
     }else if(clone_ret == 0){
         printf(1, "$$$$$in thread(%d)$$$$$$$$\n", getpid());
     }else if(clone_ret > 0){
        sleep(200);
        printf(1, "in parent(%d).##################################### therad_id=%d\n",getpid(), clone_ret);
        
     }
    
    printf(1,"Exiting thread.c\n");

    exit();
}
