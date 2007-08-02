//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DEVICEDIRVIEW__
#define __DEVICEDIRVIEW__
#include <QTreeView>
#include <QAction>
#include "DirNode.h"
#include "DeviceDirModel.h"
#include "MtpThread.h"
#include <iostream>
using namespace std;

class DeviceDirView : public QTreeView
{
    Q_OBJECT
public:
    DeviceDirView (QWidget*, MtpThread*);
    void setupMenus (void);
    void reset ( void );
private slots:
    void transferFolder ( void );
    void createFolder ( void );
    void deleteFolder ( void );

private:
    QAction* _transferFolderAct;
    QAction* _deleteFolderAct;
    QAction* _createFolderAct;
    QModelIndexList selectedDirectories();
    MtpThread* _thread;
};

#endif
