project(plapihlp LANGUAGES C)

add_library(plapihlp SHARED
    src/io.c)
target_link_libraries(plapihlp PUBLIC duktape)