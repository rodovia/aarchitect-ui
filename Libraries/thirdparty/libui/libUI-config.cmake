# poor config file as a blanket since libui does not support CMake anymore.

if (${SEARCH_LIBUI_DYNAMIC})
set(LIBUI_DEFINES libui_EXPORTS)
endif()

set(LIBUI_PATH Libraries/thirdparty/libui)
if (MSVC)
    set(LIBUI_FILES libui)
else()
    set(LIBUI_FILES ui) # lib prefix
    link_directories(${LIBUI_PATH})
endif()
