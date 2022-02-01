#pragma once

#include <tabulatedcf.h>
#include <stdint.h>

#define cfSEEK_START  1
#define cfSEEK_END   2
#define cfSEEK_ZERO 3

typedef struct cfLexer {
    const wchar_t* rawContent;
    size_t currentIndex;
    wchar_t currentChar;
} _CFLEXER;

typedef struct cfToken {
    wchar_t* inner;
    enum {
        TOKEN_ASSIGN,
        TOKEN_RIGHTCBRACE,
        TOKEN_LEFTCBRACE,
        TOKEN_STRING,
        TOKEN_NUMBER,
        TOKEN_KEYWORD,
        TOKEN_IDENTIFIER
    } type;
} _CFTOKEN;

CFAPI(_CFTOKEN*) cfTokenCreate(int iType, wchar_t* cInner);

CFAPI(_CFLEXER*) cfLexerCreate(const wchar_t* lpcContent);
CFAPI(_CFTOKEN*) cfLexerGetNextToken(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerParseString(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerParseIdentifier(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerAdvanceWToken(_CFLEXER* lpLexer, _CFTOKEN* lpToken);

wchar_t cfLexerSeek(_CFLEXER*);
wchar_t cfLexerSeek1(_CFLEXER* lpLexer, int offset, int type);
void cfLexerSkipWhitespace(_CFLEXER* lpLexer);
wchar_t* cfLexerCurrentCharAString(_CFLEXER* lpLexer);
