project(mpack LANGUAGES C)

add_library(mpack STATIC mpack.c)
target_include_directories(mpack PUBLIC ".")
