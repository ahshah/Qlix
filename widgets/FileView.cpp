#include "FileView.h"
//signals

FileView::FileView (QWidget* parent = NULL) : QTreeView(parent)
{
    setupMenus();
}

void FileView::setupMenus (void)
{
    _transferFileAct = new QAction("Transfer To Device", this);
    connect (_transferFileAct, SIGNAL(triggered()), this, SLOT(transferFile()));
    insertAction(NULL, _transferFileAct);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void FileView::setDeviceFileView(DeviceFileView* in_devfileview)
{
    _deviceFileView = in_devfileview;
    connect (this,
             SIGNAL(TransferToDevice (const QString&, DirNode*)),
             _deviceFileView, 
             SLOT(TransferToDevice(const QString&, DirNode*)));
}

QString FileView::CurrentDirectory()
{
    QDirModel* tempModel = dynamic_cast<QDirModel*> (model());
    QModelIndex idx = rootIndex();
    if (!idx.isValid() )
    {
        return "/";
    }
    return tempModel->filePath(idx);
}

void FileView::CurrentSelection (QList<QString>* listout)
{
    QDirModel* tempModel = dynamic_cast<QDirModel*> (model());
    QItemSelectionModel* selectedModel = selectionModel();
    QModelIndexList selectedList = selectedModel->selectedRows();
    if (selectedList.size() < 1)
        return;

    for (int i = 0; i < selectedList.size(); i++)
    {
        QString fpath = tempModel->filePath(selectedList[i]);
        listout->push_back(fpath);
    }
}

void FileView::transferFile ( void )
{
    QList<QString> fileList;
    CurrentSelection(&fileList);
    if (_deviceFileView == NULL)
        return;
    DirNode* CurrentDirectory =_deviceFileView->CurrentDirectory();

    if (CurrentDirectory == NULL)
    {
        cout << "No directory selected" << endl;
        return;
    }
    cout <<"Got current directory.. it is: " << CurrentDirectory->GetName() << endl;
    for (int i = 0; i < fileList.size(); i++)
    {
        emit TransferToDevice(fileList[i], CurrentDirectory);
        //cout<< "attempt to transfer" << fileList[i].toStdString() << endl;
    }
}
