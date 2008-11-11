BUILD_VER=`svn info | grep Revision | sed 's/Revision\:\ \([0-9]*\)/\1/'`
BUILD_VER=$((BUILD_VER + 1))
echo "CURRENT BUILD VERSION: " $BUILD_VER

if [ ! "$QLIXDEVELOPER" ]
then
  echo \
    "If you are not a developer for Qlix please do not use this build script as \n \
    it will mess up the build versioning system and will make it difficult to \n \
    report bugs.\n \
    Instead use the following sequence: \n \
    \$ qmake -makefile \n \
    \$ make \n \
    \$ ./qlix \n"
  exit
else
  echo "enum { BUILD_VERSION = $BUILD_VER  };" > build.h
  make
fi
