#ifndef RESOURCE_ALLOCATOR
#define RESOURCE_ALLOCATOR

#include <sys/resource.h>
#include <stdio.h>
#include <iostream>

const long REQUIRED_STACK_SIZE = 64L * 1024L * 1024L;

class ResourceAllocator
{
public:
    ResourceAllocator(){};
    ~ResourceAllocator(){};

    static void increaseStackSize()
    {
        struct rlimit rl;
        int result;
        result = getrlimit(RLIMIT_STACK, &rl);
        if (result == 0)
        {
            if (rl.rlim_cur < REQUIRED_STACK_SIZE)
            {
                rl.rlim_cur = REQUIRED_STACK_SIZE;
                result = setrlimit(RLIMIT_STACK, &rl);
                if (result != 0)
                {
                    std::cerr << "ERROR: Cannot increase STACK size" << std::endl;
                }
            }
        }
    }

    static long getStackSize()
    {
        struct rlimit rl;
        int result;
        result = getrlimit(RLIMIT_STACK, &rl);
        if (result == 0)
        {
            return rl.rlim_cur;
        }

        return -1;
    }

    void printSizeDifference()
    {
        std::cout << "TOTAL STACK SIZE: " << getStackSize() <<std::endl;
    }
};

#endif