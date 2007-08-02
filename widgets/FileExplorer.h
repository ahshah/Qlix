//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __FILEEXPLORER__
#define __FILEEXPLORER__
#include <QMenu>
#include <QDirModel>
#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <iostream>
#include <QContextMenuEvent>
#include <QFileSystemWatcher>
#include "FileNode.h"
#include "FileView.h"
#include "DirView.h"
#include "MtpThread.h"
#include "DeviceDirView.h"
#include "DeviceFileView.h"
using namespace std;

class FileExplorer : public QWidget
{
    Q_OBJECT
public:
    FileExplorer (QWidget* parent = NULL, MtpThread* thread = NULL);
    ~FileExplorer();

    FileView* GetFSFileView ();
    void* GetFSDirView ();
    void SetDeviceFileView (DeviceFileView* in_view);

signals:
    void RequestFile(const FileNode& temp, const string& path);

public slots:
    void EX (const QModelIndex& temp);
    void refreshDirView ();
    void refreshFileView ();

private:
//layout and device thread
    MtpThread* _mtpThread;
    QHBoxLayout* _layout;
    QSplitter* _splitter;
//models
    QDirModel* _dirModel;
    QDirModel* _fileModel;
//views
    DirView* _dirView;
    FileView* _fileView;
//selctionModels
    QItemSelectionModel* _dirSelection;
    QItemSelectionModel* _fileSelection;
//this helps to keeps us in sync with the filesystem
    QFileSystemWatcher _fsWatch;
    QString _currentDir;
//Device views
    DeviceFileView* _deviceFileView;
    DeviceDirView*  _deviceDirView;

    void setupDirModel ();
    void setupDirView ();
    void setupFileModel();
    void setupFileView ();
    void setupConnections ();
    void setupFsWatch();
};
#endif

