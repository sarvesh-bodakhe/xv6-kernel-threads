#include "types.h"
// #include "stat.h"
#include "user.h"
#include "param.h"
// #include "pthread.h"
// #include "pthread.h"

typedef int pthread_t;

int pthread_create( pthread_t *thread, void(*fun)(void*), void *argv, int flags){
    char *stack = malloc(4096);
    int tid = clone(fun ,argv, stack, flags);
    *thread = tid;
    return tid;
}

int pthread_join(pthread_t thread, void **retval){
    return join(thread, retval);
}