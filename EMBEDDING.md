Vovoid VSXu Embedding Guide for CMake
=====================================
©2015 Vovoid Media Technologies AB, Sweden

This guide will offer the complete solution to build VSXu, and will
help you with linking to it in your own CMake project.

VSXu is designed as a standard library in Unix/Linux, thus it provides shared objects in
/usr/lib/vsxu (or simlar) and include files for compilation in /usr/include.

However, whereas you can install it like that, it is preferable to not do so while developing
against it because of conflicting versions, ongoing development etc. So this is the complete
way to build your projects against a free-floating VSXu build.

If you have already installed vsxu into /usr/* you can skip steps 2 and 3 of this document.

## 1. Checking out VSXu sources
We assume you have not created ~/vsxu-dev yet.

    $ mkdir ~/vsxu-dev
    $ cd ~/vsxu-dev
    $ mkdir ~/vsxu-dev/vsxu
    $ cd ~/vsxu-dev/vsxu
    $ git init
    $ git remote add origin git://github.com/vovoid/vsxu.git

In this case, we want to build the development branch, 0.6.0. If you want to build current stable,
just excahge "0.6.0" with "master" in the git commands below.

    $ git fetch origin 0.6.0
    $ git checkout -b 0.6.0 FETCH_HEAD

## 2. Using the helper build scripts

While you can build VSXu inside the source directory - the easiest method
in the long run is to make use of a helper repository - vsxu-build.

    $ mkdir ~/vsxu-dev/build
    $ cd ~/vsxu-dev/build
    $ git init
    $ git remote add origin git://github.com/vovoid/vsxu-build.git
    $ git fetch origin 0.6.0
    $ git checkout -b 0.6.0 FETCH_HEAD

The most common case during development is to build the 64-bit Linux version.

    $ cd ~/vsxu-dev/build/linux64
    $ ./build-normal

If you want a version with debug symbols:

    $ cd ~/vsxu-dev/build/linux64
    $ ./build-debug

If you want to link to VSXu statically:

    $ cd ~/vsxu-dev/build/linux64
    $ ./build-static

If you want to link to VSXu statically with debug symbols:

    $ cd ~/vsxu-dev/build/linux64
    $ ./build-static-debug

These build scripts install locally into:

    ~/vsxu-dev/build/linux64/install

and for the static version:

    ~/vsxu-dev/build/linux64/install_static

This means you can have a dynamic build and a static build at the same time.
However you can not have both with debug symbols and without at the same time.

## 3. Understanding pkg-config

Now, if you look in

    ~/vsxu-dev/build/linux64/install/lib/pkgconfig/

there should be a standard pkgconfig file. However, since pkgconfig
normally just looks in /usr/lib/ etc. we need to tell pkgconfig the path to our pkgconfig file.

However, first verify that there is no pre-existing VSXu installed, this can cause problems.

    $ pkg-config --list-all|grep vsxu

This command should return 0 rows.

Add this to your ~/.bash_aliases or ~/.bashrc:

I prefer to have mine in .bash_aliases because it is a file that is not there by default, at least not in debian distros.

    $ echo "export PKG_CONFIG_PATH='/home/jaw/vsxu-dev/build/linux64/install/lib/pkgconfig'" >> ~/.bash_aliases
    $ source ~/.bash_aliases

(If you use a different shell than bash, consult your manual)

Now pkg-config should be able to find "libvsxu":

    $ pkg-config --list-all|grep vsxu

The result usually looks something like:

    libvsxu                          libvsxu - VSXu is an OpenGL-based modular programming environment, mainly to visualize music and create graphic effects in real-time.

Now we are ready to build our own project, but first we need a CMake script to help us find and include VSXu.

## 4. CMake find script

For convenience, we just list the contents of the file here.

FindVSXu.cmake listing:

    # Find VSXU
    #
    # This module defines
    #  VSXU_FOUND - whether the qsjon library was found
    #  VSXU_LIBRARIES - the vsxu library
    #  VSXU_INCLUDE_DIRS - the include path of the vsxu library
    #

    if (VSXU_INCLUDE_DIRS AND VSXU_LIBRARIES)

      # Already in cache
      set (VSXU_FOUND TRUE)

    else (VSXU_INCLUDE_DIRS AND VSXU_LIBRARIES)

      if (NOT WIN32)
        # use pkg-config to get the values of VSXU_INCLUDE_DIRS
        # and VSXU_LIBRARY_DIRS to add as hints to the find commands.
        include (FindPkgConfig)
        pkg_check_modules (VSXU libvsxu)
      endif (NOT WIN32)

      find_library (VSXU_LIBRARIES
        NAMES
        libvsxu_engine libvsxu_engine_graphics libvsxu_engine_audiovisual
        PATHS
        ${VSXU_LIBRARY_DIRS}
        ${LIB_INSTALL_DIR}
        ${KDE4_LIB_DIR}
      )

      find_path (VSXU_INCLUDE_DIRS
        NAMES
        vsxu_platform.h
        PATHS
        ${VSXU_INCLUDE_DIRS}
        ${INCLUDE_INSTALL_DIR}
        ${KDE4_INCLUDE_DIR}
      )

      include(FindPackageHandleStandardArgs)
      find_package_handle_standard_args(VSXu DEFAULT_MSG VSXU_LIBRARIES VSXU_INCLUDE_DIRS)

      if ( UNIX AND NOT APPLE )
        set ( VSXU_LIBRARIES "${VSXU_LIBRARIES} ${VSXU_LDFLAGS}" CACHE INTERNAL "")
      endif ()

    endif (VSXU_INCLUDE_DIRS AND VSXU_LIBRARIES)


This file goes into your project directory, in a folder called "cmake/"
So if your project folder is "~/my_project/", this goes into
"~/my_project/cmake/FindVSXu.cmake"

## 5. Your own cmake file

In your CMakeLists.txt (~/my_project/CMakeLists.txt), add the following:

    SET( CMAKE_MODULE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
    find_package(VSXu REQUIRED)

This will call the script above, and it will inquiry pkg-config for paths.

Add to your include_directories clause like so:

    include_directories(
        ${VSXU_INCLUDE_DIRS}
        ...other include dirs...
    )

And to your link_directories like this:

    link_directories(
        ${VSXu_LIBRARY_DIRS}
        ...other library dirs...
    )


And to your target_link_libraries

    target_link_libraries(
        my_project
        ${VSXU_LIBRARIES}
        ... other libraries ...
    )

If your external project is a VSXu module, add this:

    install(TARGETS my_project DESTINATION ${VSXU_LIBRARY_DIRS}/plugins COMPONENT engine)

The build your project in your own source folder like so:

    $ mkdir ~/my_project/build
    $ cd ~/my_project/build
    $ cmake ..
    $ make -j4

## 6. Linking to VSXu statically

You need to steer the PKG_CONFIG_PATH environment variable (see section 3 above) over to the static install
directory of vsxu.

I usually do this in a build script so I only have the dynamic version in my ~/.bash_aliases

Here is an example from a build script which also links against SDL2:

    export PKG_CONFIG_PATH=~/vsxu-dev/build/linux64/install_static/lib/pkgconfig:~/sdl2/lib/pkgconfig

Then you need to add the -DVSXU_STATIC flag to your add_definitions section in your CMakeLists.txt.
Here is an example:

    add_definitions(
      -DVSXU_STATIC
    )

## 7. Calling/Embedding VSXu Engine from your own code

There is a helper class for this, vsx_engine_helper (~/vsxu-dev/vsxu/engine/include/vsx_engine_helper.h)
If you need multiple instances of a state, use vsx_engine_helper_pool.

Here is a small example of how to use this class:

    // create module list and initialize the module list singleton
    vsx_module_list_abs* module_list = vsx_module_list_factory_create();
    vsx_engine_helper* engine_helper = new vsx_engine_helper( "my_state", module_list );





