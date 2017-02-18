#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_PATH
# GLEW_LIBRARY
# 


IF (WIN32)
        FIND_PATH( GLEW_INCLUDE_DIRS
		GL/glew.h
		"C:/Mingw/include"
		DOC "The directory where GL/glew.h resides")
        FIND_LIBRARY( GLEW_LIBRARIES
		NAMES glew GLEW glew32 glew32s
		DOC "The GLEW library"
		CMAKE_FIND_ROOT_PATH_BOTH 
		)
		add_definitions(-DGLEW_STATIC=1)
ELSE (WIN32)
        FIND_PATH( GLEW_INCLUDE_DIRS GL/glew.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where GL/glew.h resides")
        FIND_LIBRARY( GLEW_LIBRARIES
		NAMES GLEW glew
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The GLEW library")
ENDIF (WIN32)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLEW DEFAULT_MSG GLEW_INCLUDE_DIRS GLEW_LIBRARIES )
