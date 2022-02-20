#ifndef _AAR_HAS_POSIX_THREADS
#error "pthread.h is required"
#endif

#include <pthread.h>
#include "threadingapi.h"

struct aarThreadHandle
{
    pthread_t inner;
    const char* name;
};

void aarCreateThread(const char* name, aarThreadRoutine routine, 
                    void* udData, int flags)
{
    if (flags == 0)
    {
        flags = PTHREAD_CREATE_JOINABLE;
    }

    pthread_t thrd;
    int status;
    status = pthread_create(&thrd, flags, routine, udData);

    
}
