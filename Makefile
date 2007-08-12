#############################################################################
# Makefile for building: Qlix
# Generated by qmake (2.01a) (Qt 4.2.3) on: Sat Aug 11 03:21:46 2007
# Project:  Qlix.pro
# Template: app
# Command: /usr/bin/qmake -unix -o Makefile Qlix.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
LEX           = flex
YACC          = yacc
DEFINES       = -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT  $(DEFINES)
CXXFLAGS      = -pipe -g -Wall -W -D_REENTRANT  $(DEFINES)
LEXFLAGS      = 
YACCFLAGS     = -d
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -Iwidgets -Imtp -I/usr/local/include -I. -I.
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS)  -L/usr/lib -L/usr/local/lib -L/usr/lib -L/lib -lmtp -lQtGui -lQtCore -lpthread
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -sf
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		mtp/DirNode.cpp \
		mtp/MtpDevice.cpp \
		mtp/MtpFS.cpp \
		mtp/MtpPortal.cpp \
		mtp/MtpThread.cpp \
		widgets/DeviceConsole.cpp \
		widgets/DeviceDirModel.cpp \
		widgets/DeviceDirView.cpp \
		widgets/DeviceExplorer.cpp \
		widgets/DeviceFileView.cpp \
		widgets/DirView.cpp \
		widgets/FileExplorer.cpp \
		widgets/FileView.cpp \
		widgets/QlixMainWindow.cpp moc_MtpFS.cpp \
		moc_MtpThread.cpp \
		moc_DeviceConsole.cpp \
		moc_DeviceDirModel.cpp \
		moc_DeviceDirView.cpp \
		moc_DeviceExplorer.cpp \
		moc_DeviceFileModel.cpp \
		moc_DeviceFileView.cpp \
		moc_DirFilter.cpp \
		moc_DirView.cpp \
		moc_FileExplorer.cpp \
		moc_FileView.cpp \
		moc_QlixMainWindow.cpp
OBJECTS       = main.o \
		DirNode.o \
		MtpDevice.o \
		MtpFS.o \
		MtpPortal.o \
		MtpThread.o \
		DeviceConsole.o \
		DeviceDirModel.o \
		DeviceDirView.o \
		DeviceExplorer.o \
		DeviceFileView.o \
		DirView.o \
		FileExplorer.o \
		FileView.o \
		QlixMainWindow.o \
		moc_MtpFS.o \
		moc_MtpThread.o \
		moc_DeviceConsole.o \
		moc_DeviceDirModel.o \
		moc_DeviceDirView.o \
		moc_DeviceExplorer.o \
		moc_DeviceFileModel.o \
		moc_DeviceFileView.o \
		moc_DirFilter.o \
		moc_DirView.o \
		moc_FileExplorer.o \
		moc_FileView.o \
		moc_QlixMainWindow.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		Qlix.pro
QMAKE_TARGET  = Qlix
DESTDIR       = 
TARGET        = Qlix

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: Qlix.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf
	$(QMAKE) -unix -o Makefile Qlix.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
qmake:  FORCE
	@$(QMAKE) -unix -o Makefile Qlix.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/Qlix1.0.0 || $(MKDIR) .tmp/Qlix1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/Qlix1.0.0/ && $(COPY_FILE) --parents mtp/BmpStructs.h mtp/CommandCodes.h mtp/DirNode.h mtp/FileNode.h mtp/Icon.h mtp/MtpDevice.h mtp/MtpDir.h mtp/MtpFS.h mtp/MtpPortal.h mtp/MtpThread.h mtp/types.h widgets/DeviceConsole.h widgets/DeviceDirModel.h widgets/DeviceDirView.h widgets/DeviceExplorer.h widgets/DeviceFileModel.h widgets/DeviceFileView.h widgets/DirFilter.h widgets/DirView.h widgets/FileExplorer.h widgets/FileView.h widgets/QlixMainWindow.h .tmp/Qlix1.0.0/ && $(COPY_FILE) --parents main.cpp mtp/DirNode.cpp mtp/MtpDevice.cpp mtp/MtpFS.cpp mtp/MtpPortal.cpp mtp/MtpThread.cpp widgets/DeviceConsole.cpp widgets/DeviceDirModel.cpp widgets/DeviceDirView.cpp widgets/DeviceExplorer.cpp widgets/DeviceFileView.cpp widgets/DirView.cpp widgets/FileExplorer.cpp widgets/FileView.cpp widgets/QlixMainWindow.cpp .tmp/Qlix1.0.0/ && (cd `dirname .tmp/Qlix1.0.0` && $(TAR) Qlix1.0.0.tar Qlix1.0.0 && $(COMPRESS) Qlix1.0.0.tar) && $(MOVE) `dirname .tmp/Qlix1.0.0`/Qlix1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/Qlix1.0.0


yaccclean:
lexclean:
clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


/usr/bin/moc-qt4:
	(cd $(QTDIR)/src/tools/moc && $(MAKE))

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_MtpFS.cpp moc_MtpThread.cpp moc_DeviceConsole.cpp moc_DeviceDirModel.cpp moc_DeviceDirView.cpp moc_DeviceExplorer.cpp moc_DeviceFileModel.cpp moc_DeviceFileView.cpp moc_DirFilter.cpp moc_DirView.cpp moc_FileExplorer.cpp moc_FileView.cpp moc_QlixMainWindow.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_MtpFS.cpp moc_MtpThread.cpp moc_DeviceConsole.cpp moc_DeviceDirModel.cpp moc_DeviceDirView.cpp moc_DeviceExplorer.cpp moc_DeviceFileModel.cpp moc_DeviceFileView.cpp moc_DirFilter.cpp moc_DirView.cpp moc_FileExplorer.cpp moc_FileView.cpp moc_QlixMainWindow.cpp
moc_MtpFS.cpp: mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/MtpFS.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) mtp/MtpFS.h -o moc_MtpFS.cpp

moc_MtpThread.cpp: mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		mtp/MtpThread.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) mtp/MtpThread.h -o moc_MtpThread.cpp

moc_DeviceConsole.cpp: mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/types.h \
		widgets/DeviceConsole.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DeviceConsole.h -o moc_DeviceConsole.cpp

moc_DeviceDirModel.cpp: mtp/MtpDevice.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		widgets/DeviceDirModel.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DeviceDirModel.h -o moc_DeviceDirModel.cpp

moc_DeviceDirView.cpp: mtp/DirNode.h \
		mtp/types.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		widgets/DeviceDirModel.h \
		mtp/MtpDevice.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpThread.h \
		widgets/DeviceDirView.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DeviceDirView.h -o moc_DeviceDirView.cpp

moc_DeviceExplorer.cpp: widgets/DeviceDirModel.h \
		mtp/MtpDevice.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		widgets/DeviceFileModel.h \
		widgets/DeviceDirView.h \
		mtp/MtpThread.h \
		widgets/DeviceFileView.h \
		widgets/FileView.h \
		widgets/DeviceExplorer.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DeviceExplorer.h -o moc_DeviceExplorer.cpp

moc_DeviceFileModel.cpp: mtp/MtpDevice.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		widgets/DeviceFileModel.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DeviceFileModel.h -o moc_DeviceFileModel.cpp

moc_DeviceFileView.cpp: widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/DeviceFileModel.h \
		widgets/DeviceFileView.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DeviceFileView.h -o moc_DeviceFileView.cpp

moc_DirFilter.cpp: widgets/DirFilter.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DirFilter.h -o moc_DirFilter.cpp

moc_DirView.cpp: mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/FileView.h \
		widgets/DeviceFileModel.h \
		widgets/DirView.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/DirView.h -o moc_DirView.cpp

moc_FileExplorer.cpp: mtp/FileNode.h \
		mtp/types.h \
		widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/DeviceFileModel.h \
		widgets/DirView.h \
		widgets/DeviceDirView.h \
		widgets/DeviceDirModel.h \
		widgets/FileExplorer.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/FileExplorer.h -o moc_FileExplorer.cpp

moc_FileView.cpp: mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/FileView.h \
		widgets/DeviceFileModel.h \
		widgets/FileView.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/FileView.h -o moc_FileView.cpp

moc_QlixMainWindow.cpp: widgets/FileExplorer.h \
		mtp/FileNode.h \
		mtp/types.h \
		widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/DeviceFileModel.h \
		widgets/DirView.h \
		widgets/DeviceDirView.h \
		widgets/DeviceDirModel.h \
		widgets/DeviceExplorer.h \
		widgets/QlixMainWindow.h \
		/usr/bin/moc-qt4
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) widgets/QlixMainWindow.h -o moc_QlixMainWindow.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean compiler_image_collection_clean compiler_moc_source_clean compiler_uic_clean 

####### Compile

main.o: main.cpp widgets/QlixMainWindow.h \
		widgets/FileExplorer.h \
		mtp/FileNode.h \
		mtp/types.h \
		widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/DeviceFileModel.h \
		widgets/DirView.h \
		widgets/DeviceDirView.h \
		widgets/DeviceDirModel.h \
		widgets/DeviceExplorer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

DirNode.o: mtp/DirNode.cpp mtp/DirNode.h \
		mtp/types.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DirNode.o mtp/DirNode.cpp

MtpDevice.o: mtp/MtpDevice.cpp mtp/MtpDevice.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MtpDevice.o mtp/MtpDevice.cpp

MtpFS.o: mtp/MtpFS.cpp mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MtpFS.o mtp/MtpFS.cpp

MtpPortal.o: mtp/MtpPortal.cpp mtp/MtpPortal.h \
		mtp/types.h \
		mtp/MtpDevice.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MtpPortal.o mtp/MtpPortal.cpp

MtpThread.o: mtp/MtpThread.cpp mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MtpThread.o mtp/MtpThread.cpp

DeviceConsole.o: widgets/DeviceConsole.cpp widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/types.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DeviceConsole.o widgets/DeviceConsole.cpp

DeviceDirModel.o: widgets/DeviceDirModel.cpp widgets/DeviceDirModel.h \
		mtp/MtpDevice.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DeviceDirModel.o widgets/DeviceDirModel.cpp

DeviceDirView.o: widgets/DeviceDirView.cpp widgets/DeviceDirView.h \
		mtp/DirNode.h \
		mtp/types.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		widgets/DeviceDirModel.h \
		mtp/MtpDevice.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpThread.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DeviceDirView.o widgets/DeviceDirView.cpp

DeviceExplorer.o: widgets/DeviceExplorer.cpp widgets/DeviceExplorer.h \
		widgets/DeviceDirModel.h \
		mtp/MtpDevice.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		mtp/MtpFS.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		widgets/DeviceFileModel.h \
		widgets/DeviceDirView.h \
		mtp/MtpThread.h \
		widgets/DeviceFileView.h \
		widgets/FileView.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DeviceExplorer.o widgets/DeviceExplorer.cpp

DeviceFileView.o: widgets/DeviceFileView.cpp widgets/DeviceFileView.h \
		widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileModel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DeviceFileView.o widgets/DeviceFileView.cpp

DirView.o: widgets/DirView.cpp widgets/DirView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/FileView.h \
		widgets/DeviceFileModel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DirView.o widgets/DirView.cpp

FileExplorer.o: widgets/FileExplorer.cpp widgets/FileExplorer.h \
		mtp/FileNode.h \
		mtp/types.h \
		widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/DeviceFileModel.h \
		widgets/DirView.h \
		widgets/DeviceDirView.h \
		widgets/DeviceDirModel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o FileExplorer.o widgets/FileExplorer.cpp

FileView.o: widgets/FileView.cpp widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/types.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		mtp/FileNode.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/DeviceFileModel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o FileView.o widgets/FileView.cpp

QlixMainWindow.o: widgets/QlixMainWindow.cpp widgets/QlixMainWindow.h \
		widgets/FileExplorer.h \
		mtp/FileNode.h \
		mtp/types.h \
		widgets/FileView.h \
		mtp/MtpThread.h \
		mtp/MtpFS.h \
		mtp/DirNode.h \
		mtp/MtpDir.h \
		widgets/DeviceConsole.h \
		mtp/Icon.h \
		mtp/BmpStructs.h \
		mtp/CommandCodes.h \
		mtp/MtpPortal.h \
		mtp/MtpDevice.h \
		widgets/DeviceFileView.h \
		widgets/DeviceFileModel.h \
		widgets/DirView.h \
		widgets/DeviceDirView.h \
		widgets/DeviceDirModel.h \
		widgets/DeviceExplorer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o QlixMainWindow.o widgets/QlixMainWindow.cpp

moc_MtpFS.o: moc_MtpFS.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_MtpFS.o moc_MtpFS.cpp

moc_MtpThread.o: moc_MtpThread.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_MtpThread.o moc_MtpThread.cpp

moc_DeviceConsole.o: moc_DeviceConsole.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DeviceConsole.o moc_DeviceConsole.cpp

moc_DeviceDirModel.o: moc_DeviceDirModel.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DeviceDirModel.o moc_DeviceDirModel.cpp

moc_DeviceDirView.o: moc_DeviceDirView.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DeviceDirView.o moc_DeviceDirView.cpp

moc_DeviceExplorer.o: moc_DeviceExplorer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DeviceExplorer.o moc_DeviceExplorer.cpp

moc_DeviceFileModel.o: moc_DeviceFileModel.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DeviceFileModel.o moc_DeviceFileModel.cpp

moc_DeviceFileView.o: moc_DeviceFileView.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DeviceFileView.o moc_DeviceFileView.cpp

moc_DirFilter.o: moc_DirFilter.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DirFilter.o moc_DirFilter.cpp

moc_DirView.o: moc_DirView.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_DirView.o moc_DirView.cpp

moc_FileExplorer.o: moc_FileExplorer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_FileExplorer.o moc_FileExplorer.cpp

moc_FileView.o: moc_FileView.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_FileView.o moc_FileView.cpp

moc_QlixMainWindow.o: moc_QlixMainWindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_QlixMainWindow.o moc_QlixMainWindow.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

