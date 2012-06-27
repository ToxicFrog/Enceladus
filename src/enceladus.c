#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "ints.h"
#include "toc.h"
#include "io.h"
#include "loader.h"
#include "staticlibs.h"

struct {
    char * outname;
    char * targetname;
} options;

char * embed(lua_State * L, int nlibs, char ** libs)
{
    // create name of output file - if we're invoked as
    //  enceladus.exe main.lua lib1.lua lib2.lua
    // the name of the output file is main.lua-enceladus.exe
    char * myname = options.outname;
    if (!myname)
    {
        myname = strrchr(options.targetname, '/');
        
        if (!myname) myname = strrchr(options.targetname, '\\');
        
        if (!myname) myname = options.targetname;
        else myname++;
    }
    
    char * outname = options.outname;
    if (!outname)
    {
      size_t namelen = strlen(libs[0]) + strlen(myname) + 2;
      outname = malloc(namelen);
      snprintf(outname, namelen, "%s-%s", libs[0], myname);
    }
    
    // copy input file to output file
    size_t size;
    uint8_t * buf = loadfile(options.targetname, &size);
    FILE * fout = fopen(outname, "wb");
    if (!fout)
    {
        perror(outname);
        return NULL;
    }
    
    fwrite(buf, size, 1, fout);
    fclose(fout);
    free(buf);
    
    if (!writeTOC(L, outname, nlibs, libs))
        return NULL;
    
    return outname;
}

void parse_args(int * argc, char *** argv)
{
    int i;
    options.targetname = (*argv)[0];
    options.outname = NULL;
    
    for (i = 1; i < *argc; ++i)
    {
        if (strcmp((*argv)[i], "-o") == 0)
        {
            options.outname = (*argv)[++i];
        } else if (strcmp((*argv)[i], "-t") == 0) {
            options.targetname = (*argv)[++i];
        } else {
            break;
        }
    }
    
    *argv = &((*argv)[i]);
    *argc = *argc - i;
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
        parse_args(&argc, &argv);
        
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
    
    // load any statically linked libraries
    enceladus_init_static_libs(L);
    
    // grab the entry point
    // the name is on top of the stack and the table of packed files right
    // below, thanks to readTOC
    lua_gettable(L, -2); // toc{} main()
    if (lua_type(L, -1) != LUA_TFUNCTION)
    {
        fprintf(stderr, "Error loading entry point: %s\n", lua_tostring(L, -1));
        return 1;
    }
    
    // install our custom loader for the embedded libraries
    lua_insert(L, -2);
    if (!registerLoader(L))
    {
        fprintf(stderr, "Error registering custom loader: %s\n", lua_tostring(L, -1));
        return 1;
    }
    
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
