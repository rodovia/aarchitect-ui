project(aarExtensionHost)

set(BUILD_CLI OFF CACHE BOOL "" FORCE)
add_subdirectory(../libraries/thirdparty/gravity "gravity")
add_subdirectory(../libraries/thirdparty/argparse "argparse")

add_executable(exthost src/main.cc src/io.c src/options.cc)
target_link_libraries(exthost gravityapi)
