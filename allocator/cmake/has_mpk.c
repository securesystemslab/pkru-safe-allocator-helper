#include <sys/syscall.h>

int main()
{
    int pkey = syscall(SYS_pkey_alloc, 0, 0);
    return pkey == -1;
}
