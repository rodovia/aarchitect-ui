#define LWS_WITH_NETWORK // CMake Tools for some reason refuses to define this macro.
#include <libwebsockets.h>
#include <aarchitect.h>
#include <aarchitect/messagepass.h>
#include <uv.h>
#include "connection.h"

static uv_loop_t* loop;
uv_mutex_t* asyncMutex;
uv_async_t* asyncHandler;
static const uint32_t BackoffRetries[] = { 1000, 2000, 3000, 4000, 5000 };

static const lws_retry_bo_t retrytable = 
{
    .retry_ms_table = BackoffRetries,
    .retry_ms_table_count = LWS_ARRAY_SIZE(BackoffRetries),
    .conceal_count = LWS_ARRAY_SIZE(BackoffRetries),

    .secs_since_valid_ping = 3,
    .secs_since_valid_hangup = 10,

    .jitter_percent = 20
};

static struct CONNECTIONSTATE 
{
    lws_sorted_usec_list_t sul;
    struct lws* wsi;
    uint16_t retryCount;

    struct lws_ring* ring;
} cst;

static struct lws_context* ctx;
static int interrupted, port = 443, ssl_conn = LCCSCF_USE_SSL;

struct lws_context* GetWebsocketContext()
{
    return ctx;
}

uv_mutex_t* GetAsyncMutex()
{
    return asyncMutex;
}

uv_async_t* GetAsyncHandle()
{
    return asyncHandler;
}

/* Does the actual connection */
static void ConnectClientServer(lws_sorted_usec_list_t* sul) 
{
    aarUserConfiguration* lpuCfg = malloc(sizeof(aarUserConfiguration));
    aarGetSettings(lpuCfg);

    aarPrettyLog("Starting connection to server %s:%i as \"%s\"", 
        lpuCfg->server.host, lpuCfg->server.port, lpuCfg->username);
    struct PERVHOSTDATA__MINIMAL* cst = lws_container_of(sul, struct PERVHOSTDATA__MINIMAL, sul);
    struct lws_client_connect_info inf;

    // memset(&inf, 0, sizeof(inf));   

    cst->cInfo.context = cst->ctx;
    cst->cInfo.port = lpuCfg->server.port;
    cst->cInfo.address = lpuCfg->server.host;
    cst->cInfo.path = "/";
    cst->cInfo.host = cst->cInfo.address;
    cst->cInfo.origin = cst->cInfo.address;
    cst->cInfo.ssl_connection = ssl_conn;
    cst->cInfo.pwsi = &cst->client_wsi;
    cst->cInfo.retry_and_idle_policy = &retrytable;
    cst->cInfo.userdata = cst;
    cst->cInfo.ssl_connection = LCCSCF_ACCEPT_TLS_DOWNGRADE_REDIRECTS |
                                LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;


    cst->cInfo.protocol = "aarchitect-chat";
    cst->cInfo.pwsi = &cst->client_wsi;

    if (!lws_client_connect_via_info(&cst->cInfo))
    {
        aarPrettyLog("Unable to connect. Scheduling a retry");
        if (lws_retry_sul_schedule(ctx, 0, sul, &retrytable,
                ConnectClientServer, &cst->retryCount)) 
        {
            aarPrettyLog("Connection failed after 5 retries");
            interrupted = 1;
            free(lpuCfg);
        }
    }
}

static void DefAsyncProc(uv_async_t* aHnd)
{
    fprintf(stdout, "DefAsyncProc: %p", aHnd);
    aarAsyncMessageInfo* minfo = (aarAsyncMessageInfo*) aHnd->data;

    switch(minfo->type)
    {
    case MSG_SEND_CHMESSAGE:
        struct lws_vhost* vhs = lws_get_vhost_by_name(ctx, "default");     
        struct lws_protocols* aarproto = lws_vhost_name_to_protocol(vhs, "aarchitect-chat");

        struct PERVHOSTDATA__MINIMAL* vhd = (struct PERVHOSTDATA__MINIMAL*) 
                                            lws_protocol_vh_priv_get(vhs, aarproto);

        connSendMessageCreatePayload(vhd, minfo->contenttbs);
        break;
    case MSG_AWAKE:
        if (asyncMutex == NULL)
        {
            asyncMutex = malloc(sizeof(uv_mutex_t));
            uv_mutex_init(asyncMutex);
        }
        break;
    }
    uv_mutex_unlock(asyncMutex);
}

static int DefWebsocketProc(struct lws* wsi, enum lws_callback_reasons reason,
    void* userdata, void* in, size_t len) 
{
    aarUserConfiguration* lpuCfg = malloc(sizeof(aarUserConfiguration));
    aarGetSettings(lpuCfg);

    struct PERVHOSTDATA__MINIMAL* vhd = (struct PERVHOSTDATA__MINIMAL*) 
                                        lws_protocol_vh_priv_get(lws_get_vhost(wsi),
                                                                 lws_get_protocol(wsi));
    const struct MESSAGE* msg;
    int m;

    switch(reason) 
    {
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
                         in ? (char *)in : "(null)");
            
            goto _AttemptRetry;
            break;
        case LWS_CALLBACK_RECEIVE:
            
            break;
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            aarPrettyLog("Established connection");
            connSendHelloPayload(vhd, lpuCfg);
            break;
        case LWS_CALLBACK_CLIENT_CLOSED:
            goto _AttemptRetry;
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE:
            msg = lws_ring_get_element(vhd->ring, &vhd->tail);
            /* lwsl_user("LWS_CALLBACK_CLIENT_WRITEABLE: (inull %d | sz %i | payload <conf>)",
                    msg == NULL, msg->len, msg->payload); */
            lwsl_user("msg: %p", msg);
            if (!msg)
                return 0;

            /* messages are expected to be sent through the aarSendMessage func,
               which encapsulates the raw buffer. */
            m = lws_write(wsi, msg->payload, msg->len, LWS_WRITE_BINARY);
            if (m < (int) msg->len)
            {
                aarPrettyLog("ERROR %d writing to websocket", m);
                return -1;
            }

            lws_ring_consume_single_tail(vhd->ring, &vhd->tail, 1);
            if (lws_ring_get_element(vhd->ring, &vhd->tail))
                lws_callback_on_writable(vhd->client_wsi);
            break;

        case LWS_CALLBACK_PROTOCOL_INIT:
            vhd = lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi), lws_get_protocol(wsi),
                                                sizeof(struct PERVHOSTDATA__MINIMAL));
            vhd->ctx = lws_get_context(wsi);
            vhd->protocol = lws_get_protocol(wsi);
            vhd->vhost = lws_get_vhost(wsi);
            vhd->ring = lws_ring_create(sizeof(struct MESSAGE), 8, NULL);
            ConnectClientServer(&vhd->sul);
            break;
        case LWS_CALLBACK_PROTOCOL_DESTROY:
            vhd->finished = 1;
            if (vhd->ring)
                lws_ring_destroy(vhd->ring);
            lws_sul_cancel(&vhd->sul);
        default:
            break;
    }
    
    return lws_callback_http_dummy(wsi, reason, userdata, in, len);

_AttemptRetry:
    vhd->established = 0;
    vhd->client_wsi = NULL;
    if (lws_retry_sul_schedule_retry_wsi(wsi, &vhd->sul, 
        ConnectClientServer, &vhd->retryCount))
    {
        aarPrettyLog("Connection failed after 5 retries");
        interrupted = 1;
    }
    return 0;
}

static const struct lws_protocols protocols[] = {
    { "aarchitect-chat", DefWebsocketProc, 0, 0, 0, NULL, 0 },
    LWS_PROTOCOL_LIST_TERM
};

int aarConnectClientBlocking(aarUserConfiguration* lpuCfg) {
    int n = 0;
    struct lws_context_creation_info info;
    lws_set_log_level(LLL_USER | LLL_INFO | LLL_WARN | LLL_ERR | LLL_NOTICE, NULL);
    memset(&info, 0, sizeof(info));

    info.connect_timeout_secs  = 20;
    info.options = LWS_SERVER_OPTION_PEER_CERT_NOT_REQUIRED; /* Allow selfsigned certs */
    info.port = CONTEXT_PORT_NO_LISTEN; /* no server being hosted here */
    info.protocols = protocols;
    info.fd_limit_per_thread = 4;

    ctx = lws_create_context(&info);
    if (!ctx)
    {
        aarPrettyLog("Failed to create LWS context.");
        // TODO error here
    }
    
    // lws_sul_schedule(ctx, 0, &cst.sul, ConnectClientServer, 1);
    while (n >= 0 && !interrupted)
        n = lws_service(ctx, 0);

    lws_context_destroy(ctx);
    return 0;
}

static void CreateConnectionLoop()
{
    uv_timer_t* timer;
    loop = malloc(sizeof(uv_loop_t));
    timer = malloc(sizeof(uv_timer_t));
    asyncHandler = malloc(sizeof(uv_async_t));
    asyncMutex = malloc(sizeof(uv_mutex_t));

    uv_loop_init(loop);
    uv_async_init(loop, asyncHandler, DefAsyncProc);
    uv_mutex_init(asyncMutex);

    uv_timer_init(loop, timer);
    uv_timer_start(timer, aarConnectClientBlocking, 0, 0);

    uv_run(loop, UV_RUN_NOWAIT);
}

uv_thread_t aarConnectClient(aarUserConfiguration* lpuCfg)
{
    uv_thread_t threadHandle;
    uv_thread_create(&threadHandle, CreateConnectionLoop, lpuCfg);

    return threadHandle;
}
