#!/bin/bash
#This script removes svn folders to prepare for an eventual debian release

#First we grab the svn global version
SVN_REVISION=`svnversion -n`
MODCHECK=`echo $SVN_REVISION | sed 's/[0-9]*//g'`
if [ -n "$MODCHECK" ] && [ "$1" != "stale" ] ; then 
	echo "Cannot release a locally modified SVN copy: $SVN_REVISION"
	exit 1
else
	echo "Building stale release with local modifications: $SVN_REVISION"
fi

#now we create a release tar file while we will clean up later
TARFILENAME="qlix-release-($SVN_REVISION).tar.gz"
if [ -e ../$TARFILENAME ] ; then
  echo "Stale release tarfile exists, removing."
  rm ../$TARFILENAME
fi
tar czf ../$TARFILENAME *
cd ..
if [ -d release ] ; then
  echo "Release folder exists- deleting."
  rm -rf release
  mkdir release
else
  echo "Release folder does not exist."
  mkdir release
fi

cd release
tar xf ../qlix-release-($SVN_REVISION).tar.gz
rm tags
rm out
rm -rf .svn
rm -rf mtp/.svn
rm -rf widgets/.svn
rm -rf modeltest/.svn
rm -rf pixmaps/.svn
rm -rf pixmaps/ActionBar/.svn
rm -rf pixmaps/TreeView/.svn

eval "cat widgets/QlixMainWindow.cpp | sed 's/.arg(\"SVN\")/.arg(\"$SVN_REVISION\")/' > widgets/QlixMainWindow.cpp"
