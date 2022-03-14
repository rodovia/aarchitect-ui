project(aarsynch)

set(synch_SOURCES)
if (WIN32)
    list(APPEND synch_SOURCES
        src/win/pipes.c
        src/win/process.c
    )
endif()

add_library(synch STATIC ${synch_SOURCES})
target_include_directories(synch PUBLIC ../../include)