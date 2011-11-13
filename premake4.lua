solution "enceladus"
    configurations { "Debug", "Release" }
    location ("build/"..os.get())
    
    configuration "Debug"
        flags "Symbols"
    
    configuration "Release"
        flags "Optimize"
    
    configuration "linux"
        defines { "LUA_USE_LINUX" }
        linkoptions { "-rdynamic" }
        links { "dl" }
        
    configuration "macosx"
        defines { "LUA_USE_MACOSX" }
    
    configuration "windows"
        defines { "_WIN32", "LUA_WIN" }

    -- if we're using Make, we're probably using gcc or compatible
    -- these do not default to C99 mode
    configuration "gmake"
        buildoptions { "-std=c99" }

    project "Lua-5.1"
        kind "StaticLib"
        language "C"
        files "src/lua/*.c"
        includedirs "src/lua"
        
        configuration "windows"
            kind "SharedLib"
            targetname "lua5.1"
            targetextension ".dll"
            implibprefix ""
            implibextension ".lib"
        
    project "enceladus"
        kind "ConsoleApp"
        language "C"
        files "src/*.c"
        includedirs { "src", "src/lua" }
        links { "m", "Lua-5.1" }

        configuration { "windows", "gmake" }
            linkoptions "../../lua5.1.dll"
