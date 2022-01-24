#include <tabulatedcf_int.h>
#include <stdlib.h>
#include <string.h>

_CFLEXER* cfLexerCreate(const char* lpcContent) {
    _CFLEXER* lexer = calloc(1, sizeof(_CFLEXER));
    lexer->rawContent = lpcContent;
    lexer->currentIndex = 0;
    lexer->currentChar = lpcContent[lexer->currentIndex];

    return lexer;
}

size_t cfLexerAdvance(_CFLEXER* lpLexer) {
    if (lpLexer->currentChar != '\0' && lpLexer->currentIndex < strlen(lexer->rawContent)) {
        lpLexer->currentIndex++;
        lpLexer->currentChar = lpLexer->rawContent[lexer->currentIndex];
    }

    return lpLexer->currentIndex;
}

void cfLexerSkipWhitespace(_CFLEXER* lpLexer) {
    while (lpLexer->currentChar == ' ' || lpLexer->currentChar == (char) 10) {
        cfLexerAdvance(lpLexer);
    }
}

_CFTOKEN* cfLexerGetNextToken(_CFLEXER* lpLexer) {
    while (lpLexer->currentChar != '\0' && lpLexer->currentIndex < strlen(lpLexer->rawContent)) {
        if (lpLexer->currentChar == ' ' || lpLexer->currentChar == (char) 10) {
            cfLexerSkipWhitespace(lpLexer);
        }

        switch(lexer->currentChar) {
        case '=':
            if (cfLexerSeek(lpLexer) == '>') {
                return cfTokenCreate(/* cfToken_ASSIGN */ 0, '>');
            }

            return cfTokenCreate(/*cfTOKEN_EQUALS*/ 0, '=');
        case '"':
            // cfLexerParseString(lpLexer);
            break;
        }
    }
}

// _CFTOKEN* cfLexerParseString(_CFLEXER*);

// _CFTOKEN* cfTokenCreate(int type, char tChar);
// char cfTokenChar(_CFTOKEN*);
