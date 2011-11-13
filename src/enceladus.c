#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "toc.h"
#include "io.h"

char * embed(lua_State * L, int argc, char ** argv)
{
    // create name of output file - if we're invoked as
    //  enceladus.exe main.lua lib1.lua lib2.lua
    // the name of the output file is main.lua-enceladus.exe
    char * myname = strrchr(argv[0], '/');
    if (!myname) myname = strrchr(argv[0], '\\');
    
    if (!myname) myname = argv[0];
    else myname++;
    
    size_t namelen = strlen(argv[1]) + strlen(myname) + 2;
    char * outname = malloc(namelen);
    snprintf(outname, namelen, "%s-%s", argv[1], myname);
    
    // copy input file to output file
    size_t size;
    uint8_t * buf = loadfile(argv[0], &size);
    FILE * fout = fopen(outname, "wb");
    if (!fout)
    {
        perror(outname);
        return NULL;
    }
    
    fwrite(buf, size, 1, fout);
    fclose(fout);
    free(buf);
    
    if (!writeTOC(L, outname, argc-1, &(argv[1])))
        return NULL;
    
    return outname;
}

int main(int argc, char ** argv)
{
    lua_State * L = luaL_newstate();
    
    if (!readTOC(L, argv[0]))
    {
        if (argc < 2)
        {
            // display usage message and exit
            printf("This copy of enceladus has no embedded lua program to run\n");
            printf("Please embed one: enceladus main.lua lib.lua ...\n");
            return 1;
        }
        
        // parse command line and embed lua code, then exit
        char * outname = embed(L, argc, argv);
        if (outname)
        {
            printf("Embed successful. Output file: %s\n", outname);
        } else {
            fprintf(stderr, "Embed failed. Aborting.\n");
        }
        
        return 0;
    }
    
    // TOC read successful - we have an embedded lua program to run
    // initialize standard libraries
    luaL_openlibs(L);
    
    // grab the entry point
    // the name is on top of the stack and the table of packed files right
    // below, thanks to readTOC
    lua_gettable(L, -2); // toc{} main()
    
    // install our custom loader for the embedded libraries
    // FIXME
    
    // marshal arguments into lua
    lua_newtable(L); // toc{} main() arg{}
    for (int i = 0; i < argc; ++i)
    {
        lua_pushstring(L, argv[i]); // toc{} main() arg{} argv[i]
        lua_rawseti(L, -2, i); // toc{} main() arg{}
    }
    lua_setglobal(L, "arg"); // toc{} main()
    
    for (int i = 1; i < argc; ++i)
    {
        lua_pushstring(L, argv[i]); // toc{} main() argv[i]...
    }
    
    // call
    lua_getglobal(L, "debug"); // ... debug
    lua_getfield(L, -1, "traceback"); // ... debug debug.traceback
    lua_insert(L, 1); // debug.traceback ... debug
    lua_pop(L, 1); // debug.traceback ...
    
    if (lua_pcall(L, argc-1, 0, 1) != 0)
    {
        // error handling
        fprintf(stderr, "error: %s\n", lua_tostring(L, -1));
        return 1;
    }
    
    return 0;
}
