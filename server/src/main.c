#include <stdio.h>
#include <signal.h>
#include "protocol.h"
#include "config.h"

static struct lws_protocols protocols[] = {
    LWS_PLUGIN_PROTOCOL_AARCHITECT_SERVER,
    LWS_PROTOCOL_LIST_TERM
};

static int interrupted = 0;

static const struct lws_protocol_vhost_options pvo_ops = {
        NULL,
        NULL,
        "options",		/* pvo name */
        (void *)1      /* pvo value */
};

static const struct lws_protocol_vhost_options pvo_ops_interrupt = {
        NULL,
        NULL,
        "interrupted",		/* pvo name */
        (void *)0          /* pvo value */
};

static const struct lws_protocol_vhost_options pvo = {
        NULL,		/* "next" pvo linked-list */
        &pvo_ops,	/* "child" pvo linked-list */
        "aarchitect-chat",	/* protocol name we belong to on this vhost */
        ""		/* ignored */
};

static void sigint_hnd(int signal)
{
    interrupted = 1;
}
#ifdef _WIN32
#include <windows.h>

void lwsLog(int level, const char* line)
{
    HANDLE hCon = GetStdHandle(STD_ERROR_HANDLE);
    switch(level)
    {
    case LLL_INFO:
        SetConsoleTextAttribute(hCon, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        break;
    case LLL_ERR:
        SetConsoleTextAttribute(hCon, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        break;
    case LLL_WARN:
        SetConsoleTextAttribute(hCon, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        break;
    case LLL_NOTICE:
        SetConsoleTextAttribute(hCon, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        break;
    case LLL_USER:
        SetConsoleTextAttribute(hCon, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        break;
    }

    fprintf(stderr, line);
    // reset back to white
    SetConsoleTextAttribute(hCon, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
}
#endif
int main(int argc, const char** argv) 
{
    fwprintf(stdout, L"Servidor de destino do aarchitect --> versão %s. \n"
        "Rodoviajw. Todos os direitos reservados.\n\n", AARSERVER_VERSION_STRING);

    const char* p;
    char* host = "localhost";
    int port = 9192;

    lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_USER, 
#   ifdef _WIN32
        lwsLog
#   else
        NULL
#   endif
    );
    
    if ((p = lws_cmdline_option(argc, argv, "-port")))
        port = atoi(p);

    if ((p = lws_cmdline_option(argc, argv, "-host")))
        host = p;

    struct lws_context_creation_info info;
    struct lws_context *context;

    signal(SIGINT, sigint_hnd);

    memset(&info, 0, sizeof(info));
    info.port = port;
    info.iface = host;
    info.protocols = protocols;
    info.pvo = &pvo;

    context = lws_create_context(&info);
    if (!context)
    {
        lwsl_err("The context failed to be created");
        return -1;
    }

    lwsl_user("Starting server...");
    while (!interrupted)
    {
        if (lws_service(context, 0))
            interrupted = 1;
    }

    lws_context_destroy(context);
    return 0;
}