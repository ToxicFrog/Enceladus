#ifndef ENCELADUS_INTS_H
#define ENCELADUS_INTS_H

// win32 doesn't have <stdint.h>, so we need to provide our own versions
// using the nonstandard win32 type names
// we also need to put an underscore in front of snprintf for some reason

#if defined(WIN32) || defined(_WIN32)
    #define snprintf _snprintf
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int8 uint8_t;
#else
    #include <stdint.h>
#endif

#endif