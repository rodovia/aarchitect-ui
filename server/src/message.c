#include <assert.h>

#define LWS_WITH_NETWORK
#include <libwebsockets.h>
#include <mpack.h>

#include "protocol.h"
#include "message.h"
#include <connection_opcodes.h>
#include "conversions.h"

#define TIMER_TIMEOUT 500000 /* half second so LWS can have time to send the 
                                disconnect payload*/


void aarSendMessage(struct pss_data* wsi, const char* bytes)
{
    lws_ring_insert(wsi->ring, bytes, ARRAYLEN(bytes));
    lws_callback_on_writable(wsi->wsi);
}

void BroadcastMessage(const char* szData)
{
    size_t numUsers;
    struct pss_data* clients = aarGetConnectedClients(&numUsers);
    for (int i = 0; i >= numUsers; i++)
    {
        struct pss_data dat = clients[i];
        aarSendMessage(&dat, szData);
    }
}

const char* CreateUserAddPayload(struct aaruserdata data)
{
    char* finalData;
    size_t finalSize;
    mpack_writer_t writer;

    mpack_writer_init_growable(&writer, &finalData, &finalSize);
    mpack_write_cstr(&writer, "type");
        mpack_write_int(&writer, EVENT_PACKET);

    mpack_write_cstr(&writer, "event");
        mpack_write_cstr(&writer, USER_ADD);
    
    mpack_write_cstr(&writer, "username");
        mpack_write_cstr(&writer, data.username);
    if (mpack_writer_destroy(&writer) != mpack_ok)
        lwsl_warn("ERROR while creating EVENT (USER_ADD) packet!");

    return finalData;
}

void ParseBytes(struct pss_data* wsi, const char *buffer, size_t len)
{
    mpack_tree_t tree;
    mpack_tree_init_data(&tree, buffer, len);
    mpack_tree_parse(&tree);

    mpack_node_t root = mpack_tree_root(&tree);
    int type = mpack_node_i32(mpack_node_map_cstr(root, "type"));
    if (type == HELLO_PACKET)
    {
        const char* username = mpack_node_str(mpack_node_map_cstr(root, "username"));
        struct aaruserdata uData = { username, NULL };
        aarPutUser(*wsi, uData);
        BroadcastMessage(CreateUserAddPayload(uData));
    }
}
