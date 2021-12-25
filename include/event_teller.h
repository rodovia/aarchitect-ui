// === The event teller ===
#pragma once

typedef int (*eteller_callback)(void *);

extern eteller_callback __callback;

enum {
	etellErrorINVALID_ARGUMENT = 0x0fee3,
	etellErrorCALLBACK_NULL
};

int etellTriggerCallback(void* data);
eteller_callback etellGetCallback();
void etellSetCallback(eteller_callback callback);