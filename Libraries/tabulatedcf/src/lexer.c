#include <tabulatedcf_int.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>

CFAPI(_CFLEXER*) cfLexerCreate(const wchar_t* lpcContent) {
    _CFLEXER* lpLexer = calloc(1, sizeof(_CFLEXER));
    lpLexer->rawContent = lpcContent;
    lpLexer->currentIndex = 0;
    lpLexer->currentChar = lpcContent[lpLexer->currentIndex];

    return lpLexer;
}

CFAPI(_CFTOKEN*) cfTokenCreate(int iType, wchar_t* cInner) {
    _CFTOKEN* token = calloc(1, sizeof(_CFTOKEN));
    token->type = iType;
    token->inner = cInner;

    return token;
}

size_t cfLexerAdvance(_CFLEXER* lpLexer) {
    if (lpLexer->currentChar != L'\0' && lpLexer->currentIndex < wcslen(lpLexer->rawContent)) {
        lpLexer->currentIndex++;
        lpLexer->currentChar = lpLexer->rawContent[lpLexer->currentIndex];
    }

    return lpLexer->currentIndex;
}

void cfLexerSkipWhitespace(_CFLEXER* lpLexer) {
    while (lpLexer->currentChar == L' ' || lpLexer->currentChar == L'\n') {
        cfLexerAdvance(lpLexer);
    }
}

CFAPI(_CFTOKEN*) cfLexerGetNextToken(_CFLEXER* lpLexer) {
    while (lpLexer->currentChar != '\0' && lpLexer->currentIndex < wcslen(lpLexer->rawContent)) {
        if (lpLexer->currentChar == L' ' || lpLexer->currentChar == L'\n') {
            cfLexerSkipWhitespace(lpLexer);
        }

        if (iswalnum(lpLexer->currentChar)) {
            return cfLexerParseIdentifier(lpLexer);
        }

        if (lpLexer->currentChar == L'"') {
            return cfLexerParseString(lpLexer);
        }

        switch(lpLexer->currentChar) {
        case L'=':
            if (cfLexerSeek(lpLexer) == L'>') {
                return cfLexerAdvanceWToken(lpLexer, cfTokenCreate(TOKEN_ASSIGN, ">"));
            }
            break;
        case L'{':
            return cfLexerAdvanceWToken(lpLexer, cfTokenCreate(TOKEN_LEFTCBRACE, "{"));
        case L'}':
            return cfLexerAdvanceWToken(lpLexer, cfTokenCreate(TOKEN_RIGHTCBRACE, "}"));
        case L'>':
            cfLexerAdvance(lpLexer);
            break;
        }
    }

    return NULL;
}

wchar_t cfLexerSeek(_CFLEXER* lpLexer) {
    return cfLexerSeek1(lpLexer, 1, cfSEEK_ZERO);
}

wchar_t cfLexerSeek1(_CFLEXER* lpLexer, int offset, int type) {
    size_t position;

    switch(type) {
        case cfSEEK_START:
            position = offset;
            break;
        case cfSEEK_END:
            /* Making it `(...) + offset` would only bring memory violations */
            position = (strlen(lpLexer->rawContent) - 1) - offset;
            break;
        case cfSEEK_ZERO:
            position = lpLexer->currentIndex + offset;
            break;
    }

    return lpLexer->rawContent[position];
}

_CFTOKEN* cfLexerAdvanceWToken(_CFLEXER* lpLexer, _CFTOKEN* lpToken) {
    cfLexerAdvance(lpLexer);
    return lpToken;
}

_CFTOKEN* cfLexerParseIdentifier(_CFLEXER* lpLexer) {
    cfLexerAdvance(lpLexer);
    wchar_t* value = calloc(1, sizeof(wchar_t));
    value[0] = '\0';

    while (isalnum(lpLexer->currentChar)) {
        char* t = cfLexerCurrentCharAString(lpLexer);
        value = realloc(value, (wcslen(value) + wcslen(t) + 1) * sizeof(wchar_t));
        wcscat(value, t);
        cfLexerAdvance(lpLexer);
    }

    return cfTokenCreate(TOKEN_IDENTIFIER, value);
}

_CFTOKEN* cfLexerParseString(_CFLEXER* lpLexer) {
    /* Skip quotes */
    cfLexerAdvance(lpLexer);
    wchar_t* value = calloc(1, sizeof(wchar_t));
    value[0] = '\0';

    while (lpLexer->currentChar != '"') {
        char* t = cfLexerCurrentCharAString(lpLexer);
        value = realloc(value, (wcslen(value) + wcslen(t) + 1) * sizeof(wchar_t));
        wcscat(value, t);
        cfLexerAdvance(lpLexer);
    }

    return cfTokenCreate(TOKEN_STRING, value);
}

wchar_t* cfLexerCurrentCharAString(_CFLEXER* lpLexer) {
    wchar_t* str = calloc(2, sizeof(wchar_t));
    str[0] = lpLexer->currentChar;
    str[1] = L'\0';

    return str;
}
