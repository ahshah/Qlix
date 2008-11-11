#!/bin/bash
#This script removes svn folders to prepare for an eventual debian release
if [ -e ../qlix-release.tar.gz ]
then
  echo "Stale release tarfile exists, removing."
fi
tar czf ../qlix-release.tar.gz  *
cd ..

if [ -d release ]
then
  echo "Release folder exists- deleting."
  rm -rf release
  mkdir release
else
  echo "Release folder does not exist."
  mkdir release
fi

cd release
tar xf ../qlix-release.tar.gz
rm tags
rm out
rm -rf .svn
rm -rf mtp/.svn
rm -rf widgets/.svn
rm -rf modeltest/.svn
rm -rf pixmaps/.svn
rm -rf pixmaps/ActionBar/.svn
rm -rf pixmaps/TreeView/.svn
