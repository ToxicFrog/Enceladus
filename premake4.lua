solution "enceladus"
    configurations { "Debug", "Release" }
    location ("build/"..os.get())
    
    configuration "Debug"
        flags "Symbols"
    
    configuration "Release"
        flags "Optimize"
    
    project "enceladus"
        kind "ConsoleApp"
        language "C"
        files { "src/*.c", "src/lua/*.c" }
        includedirs { "src", "src/lua" }
        links "m"

        configuration "linux"
            linkoptions { "-rdynamic" }
            defines { "LUA_USE_LINUX" }
            links { "dl" }
            
        configuration "macosx"
            defines { "LUA_USE_MACOSX" }
        
        configuration "windows"
            targetextension ".exe"
            defines "_WIN32"

        -- if we're using Make, we're probably using gcc or compatible
        -- these do not default to C99 mode
        configuration "gmake"
            buildoptions { "-std=c99" }
    
            
