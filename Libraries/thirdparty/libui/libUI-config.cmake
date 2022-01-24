# poor config file as a blanket since libui does not support CMake anymore.

if (${SEARCH_LIBUI_DYNAMIC})
set(LIBUI_DEFINES libui_EXPORTS)
endif()

set(LIBUI_PATH Libraries/thirdparty/libui)
set(LIBUI_FILES libui)
