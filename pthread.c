#include "types.h"
// #include "stat.h"
#include "user.h"
#include "param.h"
// #include "pthread.h"
// #include "pthread.h"

typedef struct  pthread_t{
    int tid;
    char* stack;
}pthread_t;

int pthread_create( pthread_t *thread, void(*fun)(void*), void *argv, int flags){
    // char *stack = malloc(4096);
    thread->stack = malloc(4096);
    int tid = clone(fun ,argv, thread->stack, flags);
    thread->tid = tid;
    return tid;
}


#define EINVAL 1    //Another thread is already waiting to join with this thread.
#define ESRCH 2     // NO such thread
#define EDEADLK 3   // Deadlock

int pthread_join(pthread_t thread, void **retval){

    int joinret =  join(thread.tid, retval);
    free(thread.stack);
    return joinret;
}