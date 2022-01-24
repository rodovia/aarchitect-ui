typedef struct cfLexer {
    const char* rawContent;
    size_t currentIndex;
    char currentChar;
} _CFLEXER;

typedef struct cfToken {
    char inner;
    int type; // TODO ENUM
}

_CFLEXER* cfLexerCreate(const char* lpcContent);
size_t cfLexerAdvance(_CFLEXER* lpLexer)
void cfLexerSkipWhitespace(_CFLEXER* lpLexer);
_CFTOKEN* cfLexerGetNextToken(_CFLEXER* lpLexer);

