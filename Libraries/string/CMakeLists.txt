cmake_minimum_required(VERSION 3.19)
project("string" LANGUAGES C)

add_library("string" STATIC src/wstr.c)
target_include_directories("string" PRIVATE "../../include")
include(CheckPIESupported)

check_pie_supported(LANGUAGES C)
set_target_properties("string" PROPERTIES POSITION_INDEPENDENT_CODE YES)

