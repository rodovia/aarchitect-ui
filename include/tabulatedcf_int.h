#pragma once

#define cfSEEK_START  1
#define cfSEEK_END   2
#define cfSEEK_ZERO 3

typedef struct cfLexer {
    const char* rawContent;
    size_t currentIndex;
    char currentChar;
} _CFLEXER;

typedef struct cfToken {
    char* inner;
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

_CFTOKEN* cfTokenCreate(int iType, char* cInner);

_CFLEXER* cfLexerCreate(const char* lpcContent);
_CFTOKEN* cfLexerGetNextToken(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerParseString(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerParseIdentifier(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerAdvanceWToken(_CFLEXER* lpLexer, _CFTOKEN* lpToken);

char cfLexerSeek(_CFLEXER*);
char cfLexerSeek1(_CFLEXER* lpLexer, int offset, int type);
void cfLexerSkipWhitespace(_CFLEXER* lpLexer);
char* cfLexerCurrentCharAString(_CFLEXER* lpLexer);
