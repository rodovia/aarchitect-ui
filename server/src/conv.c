#include "conversions.h"

cn_cbor* ConvertUDCBOR(const struct aaruserdata* ud)
{
    if (ud == NULL)
        return NULL;

    cn_cbor* map = cn_cbor_map_create(NULL);
    cn_cbor_mapput_string(map, "name", 
                        cn_cbor_string_create(ud->username, NULL), NULL);
    if (ud->avatar != NULL)
    {
        cn_cbor_mapput_string(map, "avatar", 
                        cn_cbor_data_create((const uint8_t*) ud->avatar, 
                        ARRAYLEN(ud->avatar), NULL), NULL);
    }

    return map;
}
