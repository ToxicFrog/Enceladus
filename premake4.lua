solution "enceladus"
    configurations { "Debug", "Release" }
    location ("build/"..os.get())
    targetdir "bin"
    
    configuration "Debug"
        flags "Symbols"
        targetsuffix "-dbg"
    
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
        targetdir ("lib/"..os.get())
        language "C"
        files "src/lua/*.c"
        includedirs "src/lua"
    
    if os.get() == "windows" then
        project "Lua-5.1-dll"
            kind "SharedLib"
            language "C"
            files "src/lua/*.c"
            includedirs "src/lua"
            targetprefix ""
            targetname "lua5.1"
            targetextension ".dll"
            implibdir ("lib/"..os.get())
            implibextension ".lib"
    end
        
    project "enceladus-lib"
        kind "StaticLib"
        targetdir ("lib/"..os.get())
        language "C"
        files "src/*.c"
        includedirs { "src", "src/lua" }

    project "enceladus"
        kind "ConsoleApp"
        language "C"
        links { "enceladus-lib", "m", "Lua-5.1" }

        configuration "windows"
            targetname "enceladus-static"

    if os.get() == "windows" then
        project "enceladus-dll"
            kind "ConsoleApp"
            language "C"
            links { "enceladus-lib", "m", "Lua-5.1-dll" }
            targetname "enceladus"
    end
