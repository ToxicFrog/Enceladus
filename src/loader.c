#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

// register a custom loader that require() will check before any others
// thus picking up packed libraries from the executable before files on disk
// there is one complication: files are often specified in package.path with
// a leading ./, but on the command line without one
// so, if we check a file and there's no match but it has a leading ./ or .\,
// we strip that and try again
const char loader_source[] =
    "local files = (...) "
    "table.insert(package.loaders, 1, function(module) "
    "    module = module:gsub('%.', '/') "
    "    for path in package.path:gmatch('[^;]+') do "
    "        path = path:gsub('%?', module) "
    "        if files[path] then "
    "            return files[path] "
    "        end "
    "        path = path:gsub('^%.[/\\\\]', '') "
    "        if files[path] then "
    "            return files[path] "
    "        end "
    "    end "
    "end)";

int registerLoader(lua_State * L)
{
    if (luaL_loadbuffer(L, loader_source, sizeof(loader_source)-1, "enceladus_loader") != 0)
        return 0;
        
    lua_insert(L, -2);
    if (lua_pcall(L, 1, 0, 0) != 0)
        return 0;
    
    return 1;
}
