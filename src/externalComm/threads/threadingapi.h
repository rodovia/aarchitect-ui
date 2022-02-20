#pragma once

typedef struct aarThreadHandle aarThreadHandle;
typedef void (*aarThreadRoutine)(void*);

void aarCreateThread(const char* name, aarThreadRoutine routine,
                        void* udData, int flags);
void aarJoinThread(aarThreadHandle* handle);
aarThreadHandle* aarGetCurrentThread();

