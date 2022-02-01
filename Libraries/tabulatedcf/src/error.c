#include <tabulatedcf.h>
#include <stdlib.h>
#ifdef PTHREADS_COMPAT
#    include <pthread.h>
#    include <sys/types.h> /* pthread_key_t */
#endif

#if defined(WIN32_COMPAT) && defined(_MSC_VER)
_THREAD_LOCAL int __StatusCode = 0;
#elif defined(PTHREADS_COMPAT)
pthread_key_t __StatusCode = 0;
#endif

CFAPI(int) cfSetLastError(int code) {
#if defined(WIN32_COMPAT) && defined(_MSC_VER)
    __StatusCode = code;
    return 0;
#elif defined(PTHREADS_COMPAT)
    return pthread_setspecific(__StatusCode, (void*) code);
#endif
}

CFAPI(int) cfGetLastError() {
#if defined(WIN32_COMPAT) && defined(_MSC_VER)
    return __StatusCode;
#elif defined(PTHREADS_COMPAT)
    return *(int*)pthread_getspecific(__StatusCode);
#endif
}

#ifdef PTHREADS_COMPAT
CFAPI(int) cfiCreateThreadKey(pthread_key_t* key) {
    return pthread_key_create(key, NULL); // MAY TODO: a covariant with a custom destructor
}

CFAPI(int) cfiElimThreadKey(pthread_key_t* key) {
    return pthread_key_delete(key);
}

CFAPI(void) cfStart() {
    pthread_key_t* pKey = malloc(sizeof(pthread_key_t));
    cfiCreateThreadKey(pKey);
    __StatusCode = *pKey;
}

CFAPI(void) cfEnd() {
    cfiElimThreadKey(__StatusCode);
}
#else
CFAPI(void) cfStart() {

}

CFAPI(void) cfEnd() {

}
#endif
