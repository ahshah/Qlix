BUILD_VER=`cat BUILD`
echo \
"If you are not a developer for Qlix please do not use this build script as \n \
it will mess up te build versioning system and will make it difficult to \n \
report bugs.\n \
Instead use the following sequence: \n \
\$ qmake -makefile \n \
\$ make \n \
\$ ./qlix \n"

echo "CURRENT BUILD VERSION: " $BUILD_VER
make
if [ $? -eq 0 ]
then
  echo "Build successfull- incrementing build version" 
  let "BUILD_VER += 1"
else
  echo "Unsuccessful build"
fi


echo "enum { BUILD_VERSION = " $BUILD_VER " };" > build.h
echo $BUILD_VER > BUILD
