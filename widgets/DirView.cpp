//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#include "DirView.h"
DirView::DirView (QWidget* parent = NULL) : QTreeView(parent)
{
    setupMenus();
}

void DirView::setupMenus (void)
{
    _transferDirAct = new QAction("Transfer Folder To Device", this);
    connect (_transferDirAct, SIGNAL(triggered()), this, SLOT(transferDir()));
    insertAction(NULL, _transferDirAct);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void DirView::setDeviceFileView(DeviceFileView* in_devfileview)
{
    _deviceFileView = in_devfileview;
    connect (this,
             SIGNAL(GetCurrentDeviceDirectory (DirNode**)),
             _deviceFileView,
             SLOT(CurrentDirectory(DirNode**)));

    connect (this,
             SIGNAL(TransferToDevice (const QString&, DirNode*)),
             _deviceFileView, 
             SLOT(TransferToDevice(const QString&, DirNode*)));

    connect (this,
             SIGNAL(TransferSystemDirectory(const QString&)),
             _deviceFileView, 
             SLOT(TransferSystemDirectory(const QString&)));
}

//public slots:
void DirView::CurrentFSDirectory(QString* in_filename)
{
    QDirModel* tempModel = dynamic_cast<QDirModel*> (model());
    QItemSelectionModel* selected = selectionModel();
    QModelIndexList selectedList = selected->selectedRows();
    if (selectedList.size() < 1)
    {
        (*in_filename) = "/";
        return;
    }
    QModelIndex idx = selectedList[0];

    QString fpath = tempModel->filePath(idx);
    cout << "Current file path is: " << fpath.toStdString() << endl;
    (*in_filename) = fpath;
}

//private slots:
void DirView::transferDir ( void )
{
    QString tempLoc;
    CurrentFSDirectory(&tempLoc);
    if (tempLoc == "/")
        return;
    cout << "transfer folder to device slot called:" << tempLoc.toStdString() <<  endl;
    emit(TransferSystemDirectory(tempLoc));
}

