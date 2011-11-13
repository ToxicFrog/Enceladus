#ifndef ENCELADUS_INTS_H
#define ENCELADUS_INTS_H

// win32 doesn't have <stdint.h>, so we need to provide our own versions
// using the nonstandard win32 type names

#ifndef WIN32
    #include <stdint.h>
#else
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int8 uint8_t;
#endif

#endif