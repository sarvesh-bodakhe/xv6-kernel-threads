// Memory layout

#define EXTMEM  0x100000    // Address:128KB Size(from 0):1MB       // Start of extended memory

#define PHYSTOP 0xE000000   // Address:28MB, size(from 0):224MB             // Top physical memory

#define DEVSPACE 0xFE000000 // Address:508MB size(from DEVSPACE to 4GB):32MB
                            // Other devices are at high addresses

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x80000000 //Address:256MB  size(from 0 to KERNBASE):2GB
                            // First kernel virtual address

#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked
                                    // Size: 2GB+1MB

#define V2P(a) (((uint) (a)) - KERNBASE)
#define P2V(a) ((void *)(((char *) (a)) + KERNBASE))

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts
