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
	/usr/include
	/usr/local/include
	/sw/include
	/opt/local/include
	DOC "The directory where pthread.h resides")
FIND_LIBRARY(
	PTHREAD_LIBRARY
	NAMES pthread
	PATHS
	/usr/lib32
	/usr/lib64
	/usr/lib
	/usr/local/lib64
	/usr/local/lib
	/sw/lib
	/opt/local/lib
	DOC "The Pthread library"
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PTHREAD DEFAULT_MSG PTHREAD_INCLUDE_PATH PTHREAD_LIBRARY )
