# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32 /usr/lib/gcc/i686-w64-mingw32/4.6/ )
#set(CMAKE_REQUIRED_INCLUDES $ENV{VSXU_DEPINST}/include )
#set(CMAKE_LIBRARY_PATH $ENV{VSXU_DEPINST}/lib )
#set(VSXU_BUILD_WINDOWS_32 1)

#message($ENV{VSXU_DEPINST})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
