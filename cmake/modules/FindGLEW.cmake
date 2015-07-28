#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_PATH
# GLEW_LIBRARY
# 


IF (WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
                PATHS
                $ENV{DEPINSTALL}/include
                NO_DEFAULT_PATH
                add_definitions(-DGLEW_STATIC=1)
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARY
		NAMES glew32
	        PATHS
                $ENV{DEPINSTALL}/lib
                NO_DEFAULT_PATH
                add_definitions(-DGLEW_STATIC=1)
		DOC "The GLEW library")
ELSE (WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
                PATHS
                $ENV{DEPINSTALL}/lib
                NO_DEFAULT_PATH
                add_definitions(-DGLEW_STATIC=1)
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARY
		NAMES glew32
		PATHS
                $ENV{DEPINSTALL}/lib
                NO_DEFAULT_PATH
                add_definitions(-DGLEW_STATIC=1)
		DOC "The GLEW library")
ENDIF (WIN32)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLEW DEFAULT_MSG GLEW_INCLUDE_PATH GLEW_LIBRARY )
