#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void fun(void){
    printf(1, "In Child Thread. Executing function [sleep(200)]\n");
    sleep(200);
    printf(1, "Exiting Child Thread\n");
    exit();
}

int main(){
    void (*funptr)(void) = &fun;
    char *stack_ptr = (char*)malloc(4096);
    void *argv = 0;
    
    int clone_ret = clone((void*)funptr, (void*) argv,(void*)stack_ptr);
    if(clone_ret < 0)
    {
        printf(1, "clone failed");
        exit();
    }
    else if(clone_ret > 0)
    {
        int join_ret;
        if(join(clone_ret, (void*)&join_ret) < 0)
        {
            printf(1, "join filed\n");
        }else
        {    
            printf(1, "In Parent thread. After Join\n"); 
        }
    }
    
    exit();
}
