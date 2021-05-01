struct stat;
struct rtcdate;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

int clone(void(*fun)(void*),void *argv, void *stack, int flags);
int tkill(int tid);
int gettid(void);
int getppid(void);
int join(int thread_id, void *join_ret);
int tgkill(int tgid, int tid);
int printOpenFiles(void);
// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);

int thread_create(void (*fun)(void*), void *argv);
int thread_join(int thread_id, void *thread_return);


#define CLONE_THREAD    1
#define CLONE_VM        2
#define CLONE_PARENT    4
#define CLONE_FS        8
#define CLONE_FILES     16
// #define CLONE_
// #define CLONE_
