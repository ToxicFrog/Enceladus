require "mingw32"

solution "enceladus"
    configurations { "Debug", "Release" }
    platforms { "native", "mingw32", "msys" }
    location "build"
    
    configuration "Debug"
        flags "Symbols"
    
    configuration "Release"
        flags "Optimize"
    
    configuration { "gmake" }
        buildoptions { "-std=c99" }
    
    configuration { "gmake", "not mingw32", "not msys" }
        linkoptions { "-rdynamic" }
            
    configuration { "gmake", "mingw32 or msys" }
        targetextension ".exe"
        defines "WIN32"
        
    project "enceladus"
        kind "ConsoleApp"
        language "C"
        files { "src/*.c", "src/lua/*.c" }
        includedirs { "src", "src/lua" }
        links "m"
