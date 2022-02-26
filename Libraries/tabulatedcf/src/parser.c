#include <tabulatedcf_int.h>
#include <stdlib.h>
#include <assert.h>

static _CFABSSYNTAXTREE* cfCreateSyntaxTree(int type);
static void cfParserAdvance(_CFPARSER* parser);

_CFPARSER* cfCreateParser(_CFLEXER* lexer)
{
    _CFPARSER* pars = calloc(1, sizeof(_CFPARSER));
    pars->lexer = lexer;
    pars->currentToken = cfLexerGetNextToken(lexer);

    return pars;
}

static void cfParserAdvance(_CFPARSER* parser)
{
    parser->currentToken = cfLexerGetNextToken(parser->lexer);
}

static _CFABSSYNTAXTREE* cfCreateSyntaxTree(int type) 
{
    _CFABSSYNTAXTREE* ast = calloc(1, sizeof(_CFABSSYNTAXTREE));
    ast->type = type;

    return ast;
}

_CFABSSYNTAXTREE* cfParserParseObject(_CFPARSER* parser)
{
    _CFABSSYNTAXTREE* tree = cfCreateSyntaxTree(PITEM_OBJECT);
    if (parser->currentToken->type == TOKEN_STRING)
    {
        tree->object.objectName = parser->currentToken.inner;
    }
    cfParserAdvance(parser);

    if (parser->currentToken->type != TOKEN_LEFTCBRACE)
    {
        cfSetLastError(cfError_NOOBJ);
        return NULL;
    }

    _CFABSSYNTAXTREE* abs = malloc(sizeof(_CFABSSYNTAXTREE));
    while(parser->currentToken->type != TOKEN_RIGHTCBRACE && abs != NULL)
    {
        abs = cfParserParseAssign(parser);
        if (abs == NULL)
        {
            cfSetLastError(cfError_UNCLOBJ);
            return NULL;
        }
    }
}

_CFABSSYNTAXTREE* cfParserParseAssign(_CFPARSER* parser)
{
    _CFABSSYNTAXTREE* abs = cfCreateSyntaxTree(PITEM_KV);
    cfParserAdvance(parser);
    if (!(parser->currentToken->type == TOKEN_STRING || 
        parser->currentToken->type == TOKEN_IDENTIFIER))
    {
        cfSetLastError(cfError_NOASG);
        free(abs);
        return NULL;
    }

    abs->keyvalue.key = parser->currentToken->inner;
    cfParserAdvance(parser);
    assert(parser->currentToken->type == TOKEN_ASSIGN);
    cfParserAdvance(parser); /* skip the ASSIGN token */

    if (!(parser->currentToken->type == TOKEN_STRING || 
        parser->currentToken->type == TOKEN_IDENTIFIER))
    {
        cfSetLastError(cfError_NOASG);
        free(abs);
        return NULL;
    }

    abs->keyvalue.value = parser->currentToken->inner;
    return abs;
}

