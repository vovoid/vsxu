#!/bin/bash
APPNAME=VSXu
APPDIR=$APPNAME.AppDir

SRCDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

function copy_deps {
  pushd $1
  PWD=$(readlink -f .)
  FILES=$(find . -type f -executable -or -name *.so.* -or -name *.so | sort | uniq )
  for FILE in $FILES ; do
    ldd "${FILE}" | grep "=>" | awk '{print $3}' | xargs -I '{}' echo '{}' >> DEPSFILE
  done
  DEPS=$(cat DEPSFILE | sort | uniq)
  for FILE in $DEPS ; do
    if [ -e $FILE ] && [[ $(readlink -f $FILE)/ != $PWD/* ]] ; then
      echo copying $FILE
      cp -v --parents -rfL $FILE ./ || true
    fi
  done
  rm -f DEPSFILE
  popd
}

# Delete blacklisted files
delete_blacklisted()
{
  pushd $1
  BLACKLISTED_FILES=$(cat $SRCDIR/appimage/excludelist | sed '/^\s*$/d' | sed '/^#.*$/d')
  echo $BLACKLISTED_FILES
  for FILE in $BLACKLISTED_FILES ; do
    FOUND=$(find . -xtype f -name "${FILE}" 2>/dev/null)
    if [ ! -z "$FOUND" ] ; then
      echo "Deleting blacklisted ${FOUND}"
      rm -f "${FOUND}"
    fi
  done

  # Do not bundle developer stuff
  rm -rf usr/include || true
  rm -rf usr/lib/cmake || true
  rm -rf usr/lib/pkgconfig || true
  find . -name '*.la' | xargs -i rm {}
  popd
}


mkdir -p build/$APPDIR/usr/

#Copy AppRun etc..
cp $SRCDIR/appimage/AppRun build/$APPDIR/
cp $SRCDIR/appimage/$APPNAME.desktop build/$APPDIR/
cp $SRCDIR/appimage/$APPNAME.png build/$APPDIR/

cd build
cmake -DCMAKE_INSTALL_PREFIX=$PWD/$APPDIR/usr .. || exit -1
make -j`nproc` || exit -1
make install || exit -1

#Copy deps - Running multiple times to catch all dependencies
copy_deps $APPDIR
copy_deps $APPDIR
copy_deps $APPDIR

#Delete blacklisted libs
mv $APPDIR/usr/lib/x86_64-linux-gnu/pulseaudio/* $APPDIR/usr/lib/x86_64-linux-gnu/
rm -v $APPDIR/usr/lib/x86_64-linux-gnu/lib{xcb,drm,X,xkb,xshm}*.so.*
#rm -vrf $APPDIR/usr/lib/x86_64-linux-gnu/mesa/
delete_blacklisted $APPDIR

$SRCDIR/appimage/appimagetool $PWD/$APPDIR $APPNAME.AppImage
