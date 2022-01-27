cmake_minimum_required(VERSION 3.19)
project(aarchitectUI)

set(BUILD_SHARED_LIBS ON)
set(ENABLE_CJSON_TEST NO CACHE BOOL "Whether to build cJSON tests (defaults to NO)" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")

set(libUI_DIR Libraries/thirdparty/libui)
set(SEARCH_LIBUI_DYNAMIC YES)
find_package(libUI)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/SimpleInstall")
add_subdirectory(Libraries/thirdparty/jansson-2.14)
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_CURRENT_LIST_DIR}/SimpleInstall/include")

add_subdirectory(Libraries/thirdparty/ini)
add_subdirectory(Libraries/tabulatedcf)

set(aarchitect_SOURCES src/main.c src/wSettings.c src/settings.c src/string.c)

add_executable(aarchitect ${aarchitect_SOURCES})
target_link_directories(aarchitect PUBLIC ${LIBUI_PATH})
target_compile_definitions(aarchitect PUBLIC ${LIBUI_DEFINES})
set(aarchitect_LIBS libui tabcf jasson)

if (MSVC)
    target_compile_options(aarchitect PRIVATE /utf-8)
endif()

target_link_libraries(aarchitect PRIVATE ${aarchitect_LIBS})
target_include_directories(aarchitect PUBLIC ${LIBUI_PATH} include Libraries/thirdparty/jansson-2.14/src)

install(TARGETS aarchitect RUNTIME DESTINATION ".")
install(FILES Libraries/thirdparty/libui/libui.dll DESTINATION "bin")
