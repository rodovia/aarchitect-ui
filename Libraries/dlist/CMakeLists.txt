cmake_minimum_required(VERSION 3.19)
project(DList LANGUAGES C VERSION 1.0)

set(LIBTYPE STATIC)
if (BUILD_SHARED_LIBS)
set(LIBTYPE SHARED)
endif()

add_library(dlist ${LIBTYPE} src/list.c)
target_include_directories(dlist PUBLIC "../../include")
if(LIBTYPE EQUAL STATIC)
    target_compile_definitions(dlist PRIVATE DLIST_EXPORT_SYMBOLS=1 PUBLIC DLIST_IMPORT_SYMBOLS=1)
endif()