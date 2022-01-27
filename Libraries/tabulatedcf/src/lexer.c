#include <tabulatedcf_int.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

_CFLEXER* cfLexerCreate(char const* lpcContent) {
    _CFLEXER* lpLexer = calloc(1, sizeof(_CFLEXER));
    lpLexer->rawContent = lpcContent;
    lpLexer->currentIndex = 0;
    lpLexer->currentChar = lpcContent[lpLexer->currentIndex];

    return lpLexer;
}

_CFTOKEN* cfTokenCreate(int iType, char* cInner) {
    _CFTOKEN* token = calloc(1, sizeof(_CFTOKEN));
    token->type = iType;
    token->inner = cInner;

    return token;
}

size_t cfLexerAdvance(_CFLEXER* lpLexer) {
    if (lpLexer->currentChar != '\0' && lpLexer->currentIndex < strlen(lpLexer->rawContent)) {
        lpLexer->currentIndex++;
        lpLexer->currentChar = lpLexer->rawContent[lpLexer->currentIndex];
    }

    return lpLexer->currentIndex;
}

void cfLexerSkipWhitespace(_CFLEXER* lpLexer) {
    while (lpLexer->currentChar == ' ' || lpLexer->currentChar == '\n') {
        cfLexerAdvance(lpLexer);
    }
}

_CFTOKEN* cfLexerGetNextToken(_CFLEXER* lpLexer) {
    while (lpLexer->currentChar != '\0' && lpLexer->currentIndex < strlen(lpLexer->rawContent)) {
        if (lpLexer->currentChar == ' ' || lpLexer->currentChar == '\n') {
            cfLexerSkipWhitespace(lpLexer);
        }

        if (lpLexer ->currentChar) {
            return cfLexerParseString(lpLexer);
        }

        switch(lpLexer->currentChar) {
        case '=':
            if (cfLexerSeek(lpLexer) == '>') {
                return cfLexerAdvanceWToken(lpLexer, cfTokenCreate(TOKEN_ASSIGN, ">"));
            }
            break;
        case '{':
            return cfLexerAdvanceWToken(lpLexer, cfTokenCreate(TOKEN_LEFTCBRACE, "{"));
        case '}':
            return cfLexerAdvanceWToken(lpLexer, cfTokenCreate(TOKEN_RIGHTCBRACE, "}"));
        case '>':
            cfLexerAdvance(lpLexer);
            break;
        }
    }
}

char cfLexerSeek(_CFLEXER* lpLexer) {
    return cfLexerSeek1(lpLexer, 1, cfSEEK_ZERO);
}

char cfLexerSeek1(_CFLEXER* lpLexer, int offset, int type) {
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
    /* Skip quotes */
    cfLexerAdvance(lpLexer);
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (isalnum(lpLexer->currentChar)) {
        char* t = cfLexerCurrentCharAString(lpLexer);
        value = realloc(value, (strlen(value) + strlen(t) + 1) * sizeof(char));
        strcat(value, t);
        cfLexerAdvance(lpLexer);
    }

    cfLexerAdvance(lpLexer);
    return cfTokenCreate(TOKEN_STRING, value);
}

_CFTOKEN* cfLexerParseString(_CFLEXER* lpLexer) {
    /* Skip quotes */
    cfLexerAdvance(lpLexer);
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (lpLexer->currentChar != '"') {
        char* t = cfLexerCurrentCharAString(lpLexer);
        value = realloc(value, (strlen(value) + strlen(t) + 1) * sizeof(char));
        strcat(value, t);
        cfLexerAdvance(lpLexer);
    }

    cfLexerAdvance(lpLexer);
    return cfTokenCreate(TOKEN_IDENTIFIER, value);
}

char* cfLexerCurrentCharAString(_CFLEXER* lpLexer) {
    char* str = calloc(2, sizeof(char));
    str[0] = lpLexer->currentChar;
    str[1] = '\0';

    return str;
}
