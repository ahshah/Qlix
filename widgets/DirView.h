//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DIRVIEWER__
#define __DIRVIEWER__
#include <QTreeView>
#include <QList>
#include <QAction>
#include <QFileInfo>
#include <QDirModel>
#include "MtpThread.h"
#include "DeviceFileView.h"
#include <QItemSelectionModel>
class DirView : public QTreeView
{
    Q_OBJECT
public:
    DirView (QWidget*);
    void setupMenus (void);
    void setDeviceFileView(DeviceFileView* );
signals:
    void GetCurrentDeviceDirectory(DirNode** path);
    void TransferToDevice(const QString&, DirNode*);
    void TransferSystemDirectory(const QString&);

public slots:
    void CurrentFSDirectory(QString* in_filename);
private slots:
    void transferDir ( void );
private:
    QAction* _transferDirAct;
    DeviceFileView* _deviceFileView;
};
#endif
