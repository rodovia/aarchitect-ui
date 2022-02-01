#include <tabulatedcf.h>
#include <tabulatedcf_int.h>
#include <stdlib.h>
#include <wchar.h>

int main() {
    int status;

    cfStart();

    FILE* fHandle = fopen("test.tbc", "r");
    long fSize = cfFileSize(fHandle);
    wchar_t* lpstrContent = (wchar_t*) malloc(sizeof(wchar_t) * fSize);
    status = cfReadFile(NULL, fHandle, lpstrContent);
    printf("lpstrContent: %s, sizeof: %d\n", lpstrContent, sizeof(lpstrContent));

    if (status != -1) {
        status = cfGetLastError();
        printf("cfReadFile: %d", status);
        return 1;
    }

    _CFLEXER* lpLexer = cfLexerCreate(lpstrContent);
    _CFTOKEN* lpToken = NULL;

    while((lpToken = cfLexerGetNextToken(lpLexer)) != NULL) {
        printf("TOKEN(%d, %s)\n", lpToken->type, lpToken->inner);
    }

    cfEnd();
    return 0;
}
