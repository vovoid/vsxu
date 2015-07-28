#
# Try to find GLEW library and include path.
# Once done this will define
#
# PTHREAD_FOUND
# PTHREAD_INCLUDE_PATH
# PTHREAD_LIBRARY
#


FIND_PATH(
	PTHREAD_INCLUDE_PATH
        pthread.h
        PATHS
        /usr/x86_64-w64-mingw32/include
        NO_DEFAULT_PATH
	DOC "The directory where pthread.h resides")
FIND_LIBRARY(
	PTHREAD_LIBRARY
	NAMES pthread
	PATHS
	/usr/x86_64-w64-mingw32/lib
        NO_DEFAULT_PATH
	DOC "The Pthread library"
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PTHREAD DEFAULT_MSG PTHREAD_INCLUDE_PATH PTHREAD_LIBRARY )
