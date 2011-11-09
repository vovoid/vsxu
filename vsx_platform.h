// Inclusion of this file is only needed when building
//  engine 
//  engine_graphics
//  artiste
// NOT needed when using these libraries.
#ifndef VSX_PLATFORM_H
#define VSX_PLATFORM_H

#define PLATFORM_WINDOWS          0
#define PLATFORM_LINUX          1
#define PLATFORM_MACINTOSH      2
#define PLATFORM_WM6                  3
#define PLATFORM_ANDROID          4
#define PLATFORM_SOMETHINGELSE    1000

#define PLATFORM_FAMILY_WINDOWS 0
#define PLATFORM_FAMILY_UNIX 1
#define PLATFORM_FAMILY_OTHER 2


#ifndef PLATFORM
    #if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
        #define PLATFORM                                        PLATFORM_WINDOWS
        #define PLATFORM_NAME                                   "Windows"
        #define PLATFORM_FAMILY                                 PLATFORM_FAMILY_WINDOWS
        #define PLATFORM_SHARED_FILES                           vsx_string("")
        #define DIRECTORY_SEPARATOR                             "\\"
        #ifdef PLATFORM_SHARED_FILES_STL
          #define PLATFORM_SHARED_FILES_STLSTRING               std::string("")
        #endif
    #elif defined(__APPLE__) || defined(__MACH__)
        #define PLATFORM                                        PLATFORM_MACINTOSH
        #define PLATFORM_NAME                                   "Macintosh"
        #define PLATFORM_FAMILY                                 PLATFORM_FAMILY_UNIX
        #define PLATFORM_SHARED_FILES                           vsx_string("")
        #define DIRECTORY_SEPARATOR                             "/"
        #ifdef PLATFORM_SHARED_FILES_STL
          #define PLATFORM_SHARED_FILES_STLSTRING                         std::string("")
        #endif
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__CYGWIN__)
        #define PLATFORM                                        PLATFORM_LINUX
        #define PLATFORM_NAME                                   "Linux"
        #define PLATFORM_FAMILY                                 PLATFORM_FAMILY_UNIX
        #define PLATFORM_SHARED_FILES                           vsx_string(CMAKE_INSTALL_PREFIX)+vsx_string("/share/vsxu/")
        #define DIRECTORY_SEPARATOR                             "/"
        #ifdef PLATFORM_SHARED_FILES_STL
          #define PLATFORM_SHARED_FILES_STLSTRING               std::string(CMAKE_INSTALL_PREFIX)+std::string("/share/vsxu/")
        #endif
#else
        #define PLATFORM                                        PLATFORM_SOMETHINGELSE
        #define PLATFORM_NAME                                   "Something Else"
        #define PLATFORM_FAMILY                                 PLATFORM_FAMILY_OTHER
        #define PLATFORM_SHARED_FILES                           ""
    #endif

    /*
        How many bits is this system?
    */

    // Windows
    #if (defined(_WIN64) || defined(WIN64))
        #define PLATFORM_BITS                                   64
    // Macintosh
    #elif (defined(__LP64__) || defined(_LP64) || defined(__ppc64__))
        #define PLATFORM_BITS                                   64
    // Linux
    #elif (defined(__x86_64__) || defined(__64BIT__) || (__WORDSIZE == 64))
        #define PLATFORM_BITS                                   64
    #else
        #define PLATFORM_BITS                                   32
    #endif

    #ifdef vsx_string
      #undef vsx_string
    #endif

#endif

#endif
