#!/bin/sh

##########################################################################
# compile.sh - Unix/X11 configuration script
# $Date: 2007/07/01 09:46:46 $
# $Revision: 1.15 $
#
# This is a minimalist configuration script for GLFW, which is used to
# determine the availability of certain features.
#
# This script is not very nice at all (especially the Makefile generation
# is very ugly and hardcoded). Hopefully it will be cleaned up in the
# future, but for now it does a pretty good job.
##########################################################################

##########################################################################
# Check arguments
##########################################################################
silent=no
for arg in "$@"; do
{
  case "$arg" in
    # Silent?
    -q | -quiet | --quiet | --quie | --qui | --qu | --q \
    | -silent | --silent | --silen | --sile | --sil)
      silent=yes ;;
  esac;
}
done;


##########################################################################
# Misc.
##########################################################################

config_script=$0

# File descriptor usage:
# 0 standard input
# 1 file creation
# 2 errors and warnings
# 3 some systems may open it to /dev/tty
# 4 used on the Kubota Titan
# 5 compiler messages saved in config.log
# 6 checking for... messages and results
exec 5>./config.log
if [ "x$silent" = xyes ]; then
  exec 6>/dev/null
else
  exec 6>&1
fi

echo "\
This file contains any messages produced by compilers while
running $config_script, to aid debugging if $config_script makes a mistake.
" 1>&5


##########################################################################
# Default compiler settings
##########################################################################
if [ "x$CC" = x ]; then
  CC=cc
fi

CFLAGS=
LFLAGS=
LDFLAGS=
INCS=
LIBS="-lGL -lX11"


##########################################################################
# Compilation commands
##########################################################################
compile='$CC -c $CFLAGS conftest.c 1>&5'
link='$CC -o conftest $CFLAGS $LFLAGS conftest.c $LIBS 1>&5'


##########################################################################
# Check on what system we are running
##########################################################################
echo "Checking what kind of system this is... " 1>&6

case "x`uname 2> /dev/null`" in
xLinux)
  CFLAGS="$CFLAGS -Dlinux"
  LDFLAGS="-shared"
  echo " Linux" 1>&6
  ;;
xDarwin)
  CFLAGS="$CFLAGS"
  LDFLAGS="-flat_namespace -undefined suppress"
  echo " Mac OS X" 1>&6
  ;;
*)
  LDFLAGS="-shared -soname libglfw.so"
  echo " Generic Unix" 1>&6
  ;;
esac

echo " " 1>&6


##########################################################################
# Check for X11 libs/include directories
##########################################################################
echo "Checking for X11 libraries location... " 1>&6

# X11R6 in /usr/X11/lib ?
if [ -r "/usr/X11/lib" ]; then
  LFLAGS="$LFLAGS -L/usr/X11/lib"
  INCS="-I/usr/X11/include"
  echo " X11 libraries location: /usr/X11/lib" 1>&6
# X11R/ in /usr/X11R7/lib ?
elif [ -r "/usr/X11R7/lib" ]; then
  LFLAGS="$LFLAGS -L/usr/X11R7/lib"
  INCS="-I/usr/X11R7/include"
  echo " X11 libraries location: /usr/X11R7/lib" 1>&6
# X11R6 in /usr/X11R6/lib ?
elif [ -r "/usr/X11R6/lib" ]; then
  LFLAGS="$LFLAGS -L/usr/X11R6/lib"
  INCS="-I/usr/X11R6/include"
  echo " X11 libraries location: /usr/X11R6/lib" 1>&6
# X11R5 in /usr/X11R5/lib ?
elif [ -r "/usr/X11R5/lib" ]; then
  LFLAGS="$LFLAGS -L/usr/X11R5/lib"
  INCS="-I/usr/X11R5/include"
  echo " X11 libraries location: /usr/X11R5/lib" 1>&6
# X11R6 in /opt/X11R6/lib (e.g. QNX)?
elif [ -r "/opt/X11R6/lib" ]; then
  LFLAGS="$LFLAGS -L/opt/X11R6/lib"
  INCS="-I/opt/X11R6/include"
  echo " X11 libraries location: /opt/X11R6/lib" 1>&6
# X11R6 in /usr/X/lib ?
elif [ -r "/usr/X/lib" ]; then
  LFLAGS="$LFLAGS -L/usr/X/lib"
  INCS="-I/usr/X/include"
  echo " X11 libraries location: /usr/X/lib" 1>&6
else
  # TODO: Detect and report X11R7 in /usr/lib
  echo " X11 libraries location: Unknown (assuming linker will find them)" 1>&6
fi
echo " " 1>&6
CFLAGS="$CFLAGS $INCS"


##########################################################################
# Check if we are using GNU C
##########################################################################
echo "Checking whether we are using GNU C... " 1>&6
echo "$config_script: checking whether we are using GNU C" >&5

cat > conftest.c <<EOF
#ifdef __GNUC__
  yes;
#endif
EOF

if { ac_try='$CC -E conftest.c'; { (eval echo $config_script: \"$ac_try\") 1>&5; (eval $ac_try) 2>&5; }; } | egrep yes >/dev/null 2>&1; then
  use_gcc=yes
else
  use_gcc=no
fi
rm -f conftest*

echo " Using GNU C: ""$use_gcc" 1>&6
if [ "x$use_gcc" = xyes ]; then
  CC=gcc
fi
echo " " 1>&6


##########################################################################
# Check for X11 RandR availability
##########################################################################
echo "Checking for X11 RandR support... " 1>&6
echo "$config_script: Checking for X11 RandR support" >&5
has_xrandr=no

cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

int main() {; return 0;}
EOF

if { (eval echo $config_script: \"$compile\") 1>&5; (eval $compile) 2>&5; }; then
  rm -rf conftest*
  has_xrandr=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

echo " X11 RandR extension: ""$has_xrandr" 1>&6
if [ "x$has_xrandr" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_XRANDR"
  LIBS="$LIBS -lXrandr"
fi
echo " " 1>&6


##########################################################################
# Check for X11 VidMode availability
##########################################################################
if [ "x$has_xrandr" != xyes ]; then

  echo "Checking for X11 VidMode support... " 1>&6
  echo "$config_script: Checking for X11 VidMode support" >&5
  has_xf86vm=no

  cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

#if defined(__APPLE_CC__)
#error Not supported under Mac OS X
#endif

int main() {; return 0;}
EOF

  if { (eval echo $config_script: \"$compile\") 1>&5; (eval $compile) 2>&5; }; then
    rm -rf conftest*
    has_xf86vm=yes
  else
    echo "$config_script: failed program was:" >&5
    cat conftest.c >&5
  fi
  rm -f conftest*

  echo " X11 VidMode extension: ""$has_xf86vm" 1>&6
  if [ "x$has_xf86vm" = xyes ]; then
    CFLAGS="$CFLAGS -D_GLFW_HAS_XF86VIDMODE"
    LIBS="$LIBS -lXxf86vm -lXext"
  fi
  echo " " 1>&6

fi


##########################################################################
# Check for pthread support
##########################################################################
echo "Checking for pthread support... " 1>&6
echo "$config_script: Checking for pthread support" >&5
has_pthread=no

cat > conftest.c <<EOF
#include <pthread.h>
int main() {pthread_t posixID; posixID=pthread_self(); return 0;}
EOF

# Try -pthread (most systems)
CFLAGS_THREAD="-pthread"
CFLAGS_OLD="$CFLAGS"
CFLAGS="$CFLAGS $CFLAGS_THREAD"
LIBS_OLD="$LIBS"
LIBS="$LIBS -pthread"
if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_pthread=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi

# Try -lpthread 
if [ "x$has_pthread" = xno ]; then
  CFLAGS_THREAD="-D_REENTRANT"
  CFLAGS="$CFLAGS_OLD $CFLAGS_THREAD" 
  LIBS="$LIBS_OLD -lpthread"
  if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
    rm -rf conftest*
    has_pthread=yes
  else
    echo "$config_script: failed program was:" >&5
    cat conftest.c >&5
  fi
fi

# Try -lsocket (e.g. QNX)
if [ "x$has_pthread" = xno ]; then
  CFLAGS="$CFLAGS_OLD" 
  LIBS="$LIBS_OLD -lsocket"
  if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
    rm -rf conftest*
    has_pthread=yes
  else
    echo "$config_script: failed program was:" >&5
    cat conftest.c >&5
  fi
fi

echo " pthread support: ""$has_pthread" 1>&6
if [ "x$has_pthread" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_PTHREAD"
else
  LIBS="$LIBS_OLD"
fi
echo " " 1>&6


##########################################################################
# Check for sched_yield support
##########################################################################
if [ "x$has_pthread" = xyes ]; then

  echo "Checking for sched_yield support... " 1>&6
  echo "$config_script: Checking for sched_yield support" >&5
  has_sched_yield=no

  LIBS_OLD="$LIBS"

  cat > conftest.c <<EOF
#include <pthread.h>
int main() {sched_yield(); return 0;}
EOF

  if { (eval echo $config_script: \"$compile\") 1>&5; (eval $compile) 2>&5; }; then
    has_sched_yield=yes
  else
    echo "$config_script: failed program was:" >&5
    cat conftest.c >&5
  fi

  if [ "x$has_sched_yield" = xno ]; then
    LIBS="$LIBS_OLD -lrt"
    if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
      has_sched_yield=yes
    else
      echo "$config_script: failed program was:" >&5
      cat conftest.c >&5
      LIBS="$LIBS_OLD"
    fi
  fi

  rm -f conftest*

  echo " sched_yield: ""$has_sched_yield" 1>&6
  if [ "x$has_sched_yield" = xyes ]; then
    CFLAGS="$CFLAGS -D_GLFW_HAS_SCHED_YIELD"
  fi
  echo " " 1>&6

fi


##########################################################################
# Check for glXGetProcAddressXXX availability
##########################################################################
echo "Checking for glXGetProcAddress support... " 1>&6
echo "$config_script: Checking for glXGetProcAddress support" >&5
has_glXGetProcAddress=no
has_glXGetProcAddressARB=no
has_glXGetProcAddressEXT=no

# glXGetProcAddress check
cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
int main() {void *ptr=(void*)glXGetProcAddress("glFun"); return 0;}
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_glXGetProcAddress=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

# glXGetProcAddressARB check
cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
int main() {void *ptr=(void*)glXGetProcAddressARB("glFun"); return 0;}
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_glXGetProcAddressARB=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

# glXGetProcAddressEXT check
cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
int main() {void *ptr=(void*)glXGetProcAddressEXT("glFun"); return 0;}
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_glXGetProcAddressEXT=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

echo " glXGetProcAddress extension:    ""$has_glXGetProcAddress" 1>&6
echo " glXGetProcAddressARB extension: ""$has_glXGetProcAddressARB" 1>&6
echo " glXGetProcAddressEXT extension: ""$has_glXGetProcAddressEXT" 1>&6
if [ "x$has_glXGetProcAddress" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_GLXGETPROCADDRESS"
fi
if [ "x$has_glXGetProcAddressARB" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_GLXGETPROCADDRESSARB"
fi
if [ "x$has_glXGetProcAddressEXT" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_GLXGETPROCADDRESSEXT"
fi
echo " " 1>&6


##########################################################################
# Check for dlopen support
##########################################################################
echo "Checking for dlopen support... " 1>&6
echo "$config_script: Checking for dlopen support" >&5
has_dlopen=no

cat > conftest.c <<EOF
#include <dlfcn.h>
int main() {void *l=dlopen("libGL.so",RTLD_LAZY|RTLD_GLOBAL); return 0;}
EOF

# First try without -ldl
if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_dlopen=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi

# Now try with -ldl if the previous attempt failed
if [ "x$has_dlopen" = xno ]; then
  LIBS_OLD="$LIBS"
  LIBS="$LIBS -ldl"
  if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
    rm -rf conftest*
    has_dlopen=yes
  else
    echo "$config_script: failed program was:" >&5
    cat conftest.c >&5
  fi
  if [ "x$has_dlopen" = xno ]; then
    LIBS="$LIBS_OLD"
  fi
fi
rm -f conftest*

echo " dlopen support: ""$has_dlopen" 1>&6
if [ "x$has_dlopen" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_DLOPEN"
fi
echo " " 1>&6


##########################################################################
# Check for sysconf support
##########################################################################
echo "Checking for sysconf support... " 1>&6
echo "$config_script: Checking for sysconf support" >&5
has_sysconf=no

cat > conftest.c <<EOF
#include <unistd.h>
#ifndef _SC_NPROCESSORS_ONLN
#ifndef _SC_NPROC_ONLN
#error Neither _SC_NPROCESSORS_ONLN nor _SC_NPROC_ONLN available
#endif
#endif
int main() {long x=sysconf(_SC_ARG_MAX); return 0; }
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_sysconf=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

echo " sysconf support: ""$has_sysconf" 1>&6
if [ "x$has_sysconf" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_SYSCONF"
fi
echo " " 1>&6


##########################################################################
# Check for sysctl support
##########################################################################
echo "Checking for sysctl support... " 1>&6
echo "$config_script: Checking for sysctl support" >&5
has_sysctl=no

cat > conftest.c <<EOF
#include <sys/types.h>
#include <sys/sysctl.h>
#ifdef CTL_HW
#ifdef HW_NCPU
  yes;
#endif
#endif
EOF

if { ac_try='$CC -E conftest.c'; { (eval echo $config_script: \"$ac_try\") 1>&5; (eval $ac_try) 2>&5; }; } | egrep yes >/dev/null 2>&1; then
  has_sysctl=yes
fi
rm -f conftest*

echo " sysctl support: ""$has_sysctl" 1>&6
if [ "x$has_sysctl" = xyes ]; then
  CFLAGS="$CFLAGS -D_GLFW_HAS_SYSCTL"
fi
echo " " 1>&6


##########################################################################
# Post fixups
##########################################################################
if [ "x$use_gcc" = xyes ]; then
  CFLAGS_SPEED="-c -I. -I.. $CFLAGS -O3 -ffast-math -Wall"
  CFLAGS="-c -I. -I.. $CFLAGS -Os -Wall"
  CFLAGS_LINK="$INCS -O3 -ffast-math -Wall"
else
  CFLAGS_SPEED="-c -I. -I.. $CFLAGS -O"
  CFLAGS="-c -I. -I.. $CFLAGS -O"
  CFLAGS_LINK="$INCS -O"
fi
CFLAGS_LINK="-I../include $CFLAGS_LINK"
LFLAGS_LINK="../lib/x11/libglfw.a $LFLAGS -lGLU $LIBS -lm"


##########################################################################
# Create Makefiles
##########################################################################

# ./lib/x11/Makefile.x11
MKNAME='./lib/x11/Makefile.x11'
echo "Creating ""$MKNAME""..." 1>&6
echo " " 1>&6
echo "$config_script: Creating ""$MKNAME""..." >&5
echo "##########################################################################" >$MKNAME
echo "# Automatically generated Makefile for GLFW" >>$MKNAME
echo "##########################################################################" >>$MKNAME
echo "CC           = $CC" >>$MKNAME
echo "CFLAGS       = $CFLAGS" >>$MKNAME
echo "CFLAGS_SPEED = $CFLAGS_SPEED" >>$MKNAME
echo "LDFLAGS      = $LDFLAGS" >>$MKNAME
echo "LFLAGS       = $LFLAGS" >>$MKNAME
echo "LIBS         = $LIBS" >>$MKNAME
echo " " >>$MKNAME
cat './lib/x11/Makefile.x11.in' >>$MKNAME

# ./examples/Makefile.x11
MKNAME='./examples/Makefile.x11'
echo "Creating ""$MKNAME""..." 1>&6
echo " " 1>&6
echo "$config_script: Creating ""$MKNAME""..." >&5
echo "##########################################################################" >$MKNAME
echo "# Automatically generated Makefile for GLFW" >>$MKNAME
echo "##########################################################################" >>$MKNAME
echo "CC     = $CC" >>$MKNAME
echo "CFLAGS = $CFLAGS_LINK" >>$MKNAME
echo "LFLAGS = $LFLAGS_LINK" >>$MKNAME
echo " " >>$MKNAME
cat './examples/Makefile.x11.in' >>$MKNAME


##########################################################################
# Create pkg-config template file
##########################################################################

# ./lib/x11/libglfw.pc.in
MKNAME="./lib/x11/libglfw.pc.in"
echo "Creating ""$MKNAME""..." 1>&6
echo " " 1>&6
echo "$config_script: Creating ""$MKNAME""..." >&5
cat > "$MKNAME" <<EOF
prefix=@PREFIX@
exec_prefix=@PREFIX@
libdir=@PREFIX@/lib
includedir=@PREFIX@/include

Name: GLFW
Description: A portable framework for OpenGL development
Version: 2.6.0
URL: http://glfw.sourceforge.net/
Libs: -L\${libdir} -lglfw $LFLAGS $LIBS -lm
Cflags: -I\${includedir} $CFLAGS_THREAD 
EOF

