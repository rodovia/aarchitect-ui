#pragma once

#define ARRAYLEN(t) (sizeof(t) / sizeof(t[0]))
#include "message.h"
#include <cn-cbor/cn-cbor.h>

cn_cbor* ConvertUDCBOR(const struct aaruserdata* ud);
