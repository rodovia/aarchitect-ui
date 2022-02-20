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

typedef struct cfSyntaxTree
{  
    enum
    { 
        PITEM_OBJECT = 0x1a1,
        PITEM_KV,
        PITEM_VALUE,
        PITEM_STRING,
        PITEM_COMPOUND
    } type;

    union
    {
        struct 
        {
            wchar_t* key;
            wchar_t* value;
        } keyvalue;

        struct 
        {
            wchar_t* objectName;
            struct cfSyntraxTree** values;
            size_t* sizeofValues;
        } object;

        wchar_t* valueString;
        struct
        {
            struct cfSyntaxTree* compoundStruct;
            size_t compoundSize;
        } compound;
    };
} _CFABSSYNTAXTREE;

typedef struct cfParser
{
    _CFLEXER* lexer;
    _CFTOKEN* currentToken;
} _CFPARSER;

#ifdef __cplusplus
extern "C" {
#endif

_CFPARSER* cfCreateParser(_CFLEXER* lexer);
void cfParserEat(_CFPARSER* parser, int tokenType);
_CFABSSYNTAXTREE* cfParserParse(_CFPARSER* parser);
_CFABSSYNTAXTREE* cfParserParseObject(_CFPARSER* parser);
_CFABSSYNTAXTREE* cfParserParseAssign(_CFPARSER* parser);

#define X(a) CFAPI(a);

MANYCFAPI(
    _CFTOKEN* cfTokenCreate(
        int iType, 
        wchar_t* inner
    ),
    _CFLEXER* cfLexerCreate(
        const wchar_t* lpcContent
    ),
    _CFTOKEN* cfLexerGetNextToken(
        _CFLEXER* lpLexer
    )
);

#undef X

_CFTOKEN* cfLexerParseString(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerParseIdentifier(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerAdvanceWToken(_CFLEXER* lpLexer, _CFTOKEN* lpToken);

wchar_t cfLexerSeek(_CFLEXER*);
wchar_t cfLexerSeek1(_CFLEXER* lpLexer, int offset, int type);
void cfLexerSkipWhitespace(_CFLEXER* lpLexer);
wchar_t* cfLexerCurrentCharAString(_CFLEXER* lpLexer);

#ifdef __cplusplus
} /* extern "C" */
#endif 
