#include <tabulatedcf/error.h>
#include <tabulatedcf/basic.h>
#include <stdlib.h>

#ifdef _WIN32
_THREAD_LOCAL int __StatusCode = 0;
#endif

CFAPI(int) cfSetLastError(int code) {
#if defined(_WIN32)
    __StatusCode = code;
    return 0;
#elif defined(HAS_POSIX)
    return pthread_setspecific(__StatusCode, (void*) code);
#endif
}

CFAPI(int) cfGetLastError() {
#if defined(_WIN32)
    return __StatusCode;
#elif defined(HAS_POSIX)
    return *(int*)pthread_getspecific(__StatusCode);
#endif
}

#ifdef HAS_POSIX
CFAPI(int) cfiCreateThreadKey(pthread_key_t* key) {
    return pthread_key_create(key, NULL); // MAY TODO: a covariant with a custom destructor
}

CFAPI(int) cfiElimThreadKey(pthread_key_t* key) {
    return pthread_key_delete(key);
}

CFAPI(void) inline cfStart() {
    pthread_key_t* pKey = malloc(sizeof(pthread_key_t));
    cfiCreateThreadKey(pKey);
    __StatusCode = *pKey;
}

CFAPI(void) inline cfEnd() {
    cfiElimThreadKey(__StatusCode);
}
#endif
