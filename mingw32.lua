require "newplatform"

newplatform {
    name = "mingw32";
    description = "mingw32 linux-host windows-target cross compiler";
    gcc = {
        cc = "i686-w64-mingw32-gcc";
        cxx = "i686-w64-mingw32-g++";
        ar = "i686-w64-mingw32-ar";
    };
}
