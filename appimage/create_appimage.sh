#!/bin/bash
APPNAME=VSXu
APPVERSION=0.6.1
APPDIR=$APPNAME.AppDir

SRCDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/../"

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
  BLACKLISTED_FILES=$(cat ../../excludelist | sed '/^\s*$/d' | sed '/^#.*$/d')
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


mkdir -p build-appimage/$APPDIR/usr/
mkdir -p $SRCDIR/out/

#Copy AppRun etc..
cp AppRun build-appimage/$APPDIR/
cp $APPNAME.desktop build-appimage/$APPDIR/
cp $APPNAME.png build-appimage/$APPDIR/

pushd build-appimage
cmake -DCMAKE_INSTALL_PREFIX=$PWD/$APPDIR/usr $SRCDIR || exit -1
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

GLIBC_VERSION=`find . -name *.so -or -name *.so.* -or -type f -executable  -exec readelf -s '{}' 2>/dev/null \; | sed -n 's/.*@GLIBC_//p'| awk '{print $1}' | sort --version-sort | tail -n 1`
$SRCDIR/appimage/appimagetool $PWD/$APPDIR $SRCDIR/out/$APPNAME-$APPVERSION.glibc$GLIBC_VERSION.AppImage
popd

