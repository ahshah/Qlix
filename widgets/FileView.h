//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __FILEVIEWER__
#define __FILEVIEWER__
#include <QTreeView>
#include <QList>
#include <QAction>
#include <QFileInfo>
#include <QDirModel>
#include "MtpThread.h"
#include "DeviceFileView.h"
#include "DirNode.h"
#include <QtDebug>
class DeviceFileView;
class FileView : public QTreeView
{
    Q_OBJECT
public:
    FileView (QWidget*);
    void setupMenus (void);
    void setDeviceFileView(DeviceFileView* in_devfileview);
    QString CurrentDirectory();

signals:
   void TransferToDevice (const QString&, DirNode*, bool isTrack = false);

public slots:
    void CurrentSelection (QList<QString>* listout); 

private slots:
    void transferFile (void);
    void transferTrack (void);
private:
    QAction* _transferFileAct;
    QAction* _transferTrackAct;
    DeviceFileView* _deviceFileView;
    bool isMultimedia(const QString&);
};
#endif
