require "mingw32"

solution "enceladus"
    configurations { "Debug", "Release" }
    platforms { "native", "mingw32" }
    
    configuration "Debug"
        flags "Symbols"
    
    configuration "Release"
        flags "Optimize"
    
    configuration { "linux" }
        buildoptions { "-std=c99" }
    
        configuration { "linux", "not mingw32" }
            linkoptions { "-rdynamic" }
            
        configuration { "linux", "mingw32" }
            targetextension ".exe"
        
    project "enceladus"
        kind "ConsoleApp"
        language "C"
        files { "src/*.c", "src/lua/*.c" }
        includedirs { "src", "src/lua" }
        links "m"
