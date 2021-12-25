extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>
#include <argh.h>

int main(int, char* argv[])
{
    argh::parser cmdl(argv);

    
    return EXIT_SUCCESS;
}
