#include <event_teller.h>

eteller_callback __callback = nullptr;

eteller_callback etellGetCallback() {
	return __callback;
}

void etellSetCallback(eteller_callback callback) {
	__callback = callback;
}

int etellTriggerCallback(void* data) {
	if (__callback == nullptr) {
		return etellErrorCALLBACK_NULL;
	}

	return __callback(data);
}
