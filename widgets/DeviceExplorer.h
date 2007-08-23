//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DEVICEEXPLORER__
#define __DEVICEEXPLORER__
#include <QDir>
#include <QDirModel>
#include <QTreeView>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QAction>
#include <queue>
#include "DeviceDirModel.h"
#include "DeviceFileModel.h"
#include "DeviceDirView.h"
#include "DeviceFileView.h"
#include "DeviceConsole.h"
#include "MtpPortal.h"
#include "MtpDevice.h"
#include "DirNode.h"
#include "MtpThread.h"
#include "CommandCodes.h"
#include "FileView.h"

class DeviceExplorer : public QWidget
{
    Q_OBJECT
public:
    DeviceExplorer (QWidget* parent = NULL);
    ~DeviceExplorer();
    void ConnectDevice ( void );
    void DisconnectDevice ( void );
    void SetDeviceConsole (DeviceConsole* in_console);
    void SetFSFileView(FileView* in_fsfileview);
    DeviceFileView* GetDeviceFileView ( void );
    DeviceDirView* GetDeviceDirView ( void );

public slots:

    void EXB (const QModelIndex& temp);
    void sort (int idx);
    void ConnectComplete ( MtpFS* in_fs); // fix this
    void ResetModelRootImage();

private:

    MtpThread _mtpThread;
    DeviceConsole* _console;
    DeviceDirModel* _dirModel;
    DeviceFileModel* _fileModel;
    DeviceDirView* _dirView;
    DeviceFileView* _fileView;
    FileView* _fsFileView;
    QHBoxLayout* _layout;
    QSplitter* _splitter;
    MtpFS*   _filesystem;

    void pullDeviceImage(uint32_t id, const QString& loc);
    void setupDirView ( void );
    void setupFileView ( void );
    void setupConnections();
    void init();
};
#endif
