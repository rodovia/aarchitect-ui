cmake_minimum_required(VERSION 3.19)

add_executable(tabcf-test src/main.c)
target_link_libraries(tabcf-test PRIVATE tabcf)
target_include_directories(tabcf-test PUBLIC "../../../include")

