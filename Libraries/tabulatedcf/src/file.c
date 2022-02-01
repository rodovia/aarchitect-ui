#include <tabulatedcf.h>
#include <stdio.h>

CFAPI(long) cfFileSize(FILE* fpHandle) {
    long ofSize, retSize;
    if (fpHandle == NULL) {
        cfSetLastError(cfError_NULL_POINTER);
        return -1;
    }

    ofSize =  ftell(fpHandle);
    fseek(fpHandle, 0, SEEK_END);
    retSize = ftell(fpHandle);
    fseek(fpHandle, ofSize, SEEK_SET);

    return retSize;
}

CFAPI(int) cfReadFile(const char* fName, FILE* fpHandle, wchar_t* lpstrContent) {
    long fSize;
    int elemCount;

    if (fpHandle == NULL) {
        cfSetLastError(cfError_NULL_POINTER);
        return -1;
    }

    fSize = cfFileSize(fpHandle);
    if (fSize < 0)
        return -1;
    elemCount = fread(lpstrContent, sizeof(wchar_t), fSize, fpHandle);
    return elemCount;
}

CFAPI(int) cfWriteToFile(FILE* fpHandle, const char* content, size_t size) {
    int elemCount;
    if (fpHandle == NULL) {
        cfSetLastError(cfError_NULL_POINTER);
        return -1;
    }

    elemCount = fwrite(content, sizeof(char), size, fpHandle);
    return elemCount;
}


