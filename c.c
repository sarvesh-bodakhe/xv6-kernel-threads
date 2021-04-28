#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void fun(void *argv)
{
    printf(1, "fun() start*****************************************************************************\n");
    sleep(300);
    printf(1, "fun() start$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    sleep(200);
    printf(1, "Exiting fun()\n");
    exit();
}

void fun2(void *argv){
    printf(1, "fun() start============================================================================\n");
    printf(1, "Exiting fun()\n");
    exit();
}

int main()
{
    
    char* stack = malloc(4096);
    void *argv = 0;
    int tid = clone(&fun, argv, (void *)stack, CLONE_THREAD | CLONE_VM );
    sleep(100);
    // tkill(tid);
    join(tid, 0);
    
    // tid = clone(&fun2, argv, stack,CLONE_THREAD | CLONE_VM);
    // join(tid, 0);
    printf(1, "Clone successful######################################################################\n");
    
    printf(1, "Exiting Parent thread\n");
    exit();
}
