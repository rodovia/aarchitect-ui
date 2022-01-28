cmake_minimum_required(VERSION 3.19)
project(aarchitectUI)

set(CMAKE_C_STANDARD 11)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/SimpleInstall")
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_CURRENT_LIST_DIR}/SimpleInstall/include")

# ==> DEPENDÊNCIAS <===
set(libUI_DIR libraries/thirdparty/libui)
set(SEARCH_LIBUI_DYNAMIC YES)
find_package(libUI)
add_subdirectory(libraries/thirdparty/jansson)
add_subdirectory(libraries/tabulatedcf)

set(aarchitect_SOURCES src/main.c src/wSettings.c src/settings.c src/string.c)
set(aarchitect_LIBS libui tabcf jasson)

add_executable(aarchitect ${aarchitect_SOURCES})
target_link_directories(aarchitect PUBLIC ${LIBUI_PATH})
target_compile_definitions(aarchitect PUBLIC ${LIBUI_DEFINES})

if (MSVC)
    target_compile_options(aarchitect PRIVATE /utf-8)
endif()

target_link_libraries(aarchitect PRIVATE ${aarchitect_LIBS})
target_include_directories(aarchitect PUBLIC ${LIBUI_PATH} include libraries/thirdparty/jansson/src)

install(TARGETS aarchitect RUNTIME DESTINATION ".")
install(FILES libraries/thirdparty/libui/libui.dll DESTINATION "bin")
