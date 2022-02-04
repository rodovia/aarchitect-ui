#include <libwebsockets.h>

struct PERVHOSTDATA__MINIMAL {
    struct lws_context* ctx;
    struct lws_vhost* vhost;
    const struct lws_protocols* protocol;
    lws_sorted_usec_list_t sul;

    struct lws_ring* ring;
    uint32_t tail;

    struct lws_client_connect_info cInfo;
    struct lws *client_wsi;

    int counter;
    char finished;
    char established;
    uint16_t retryCount;
};

struct MESSAGE {
    unsigned char* payload;
    size_t len;
};

void connSendMessage(struct PERVHOSTDATA__MINIMAL host, struct MESSAGE msg);
void connFreeMessage(void* msg);
