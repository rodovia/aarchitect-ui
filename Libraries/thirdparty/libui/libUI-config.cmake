# poor config file as a blanket since libui does not support CMake anymore.
if (WIN32)
    set(FITTING_COMPILER ${CMAKE_CXX_COMPILER_ID})
else()
    set(FITTING_COMPILER ${CMAKE_C_COMPILER_ID})
endif()

message(${FITTING_COMPILER})

if (${SEARCH_LIBUI_DYNAMIC})
set(LIBUI_DEFINES libui_EXPORTS)
endif()

set(LIBUI_PATH Libraries/thirdparty/libui)
if (MSVC OR FITTING_COMPILER MATCHES "Clang")
    set(LIBUI_FILES ui)
else()
    set(LIBUI_FILES ui) # lib prefix
endif()

link_directories(${LIBUI_PATH})
