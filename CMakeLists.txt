cmake_minimum_required(VERSION 3.19)
project(aarchitectUI)

# add_subdirectory(Libraries/Lua)
# add_subdirectory(scripthost)

set(libUI_DIR Libraries/libui)
set(SEARCH_LIBUI_DYNAMIC YES)
find_package(libUI)
add_executable(aarchitect src/main.cc src/atribstring.cc)

target_link_directories(aarchitect PUBLIC ${LIBUI_PATH})
target_link_libraries(aarchitect libui)
target_include_directories(aarchitect PUBLIC ${LIBUI_PATH})
target_compile_definitions(aarchitect PUBLIC ${LIBUI_DEFINES})
