//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DEVICEFILEVIEW__
#define __DEVICEFILEVIEW__
#include "FileView.h"
#include <QTreeView>
#include <QList>
#include <QAction>
#include "DeviceFileModel.h"
#include "MtpThread.h"
#include <QHeaderView>
class FileView;

class DeviceFileView : public QTreeView
{
    Q_OBJECT
public:
    DeviceFileView (QWidget* parent,  MtpThread* in_thread);
    void setupMenus (void);
    void setFileView (FileView* in_fileView);
    DirNode* CurrentDirectory();

signals:

public slots:
    void TransferToDevice(const QString& in_file, DirNode* directory, bool isTrack);
    void CreateDirectory(const QString&  in_file);
    void TransferSystemDirectory(const QString&  in_file); 

private slots:
    void transferFile ( void );
    void deleteFile ( void ) ;

private:
    QAction* _transferFileAct;
    QAction* _deleteFileAct;
    MtpThread* _thread;
    FileView* _FSfileView;
};
#endif
