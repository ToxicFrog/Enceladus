require "newplatform"

newplatform {
    name = "mingw32";
    description = "mingw32 linux-host windows-target cross compiler";
    gcc = {
        cc = "i686-w64-mingw32-gcc";
        cxx = "i686-w64-mingw32-g++";
        ar = "i686-w64-mingw32-ar";
        cppflags = premake.gcc.platforms.Native.cppflags;
    };
}

newplatform {
    name = "msys";
    description = "MSYS or mingw32-based windows-host native compiler";
    
    gcc = {
        cppflags = premake.gcc.platforms.Native.cppflags;
    };
}
