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

    _transferTrackAct = new QAction("Transfer Track To Device", this);
    connect (_transferTrackAct, SIGNAL(triggered()), this, SLOT(transferTrack()));
    insertAction(NULL, _transferTrackAct);

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
        qDebug() << "No directory selected";
        return;
    }
    qDebug() <<"Got current directory.. it is: " << QString(CurrentDirectory->GetName().c_str());
    for (int i = 0; i < fileList.size(); i++)
    {
        emit TransferToDevice(fileList[i], CurrentDirectory);
        //qDebug()<< "attempt to transfer" << fileList[i].toStdString();
    }
}

void FileView::transferTrack ( void )
{
    QList<QString> fileList;
    CurrentSelection(&fileList);
    if (_deviceFileView == NULL)
        return;
    DirNode* CurrentDirectory = _deviceFileView->CurrentDirectory();

    if (CurrentDirectory == NULL)
    {
        cout << "No directory selected" << endl;
        return;
    }

    cout <<"Got current directory.. it is: " << CurrentDirectory->GetName() << endl;
    for (int i = 0; i < fileList.size(); i++)
    {
        if (!isMultimedia(fileList[i]))
        {
            qDebug() << "Is not a multimedia file, try sending file instead";
            continue;
        }

        emit TransferTrackToDevice(fileList[i], CurrentDirectory);
        cout<< "attempt track transfer" << fileList[i].toStdString() << endl;
    }
}

bool FileView::isMultimedia(const QString& in_path)
{
    QFileInfo temp(in_path);
    QString upper_suffix = temp.suffix();
    if (!temp.exists())
        return false;

    if (upper_suffix == "MP3")
        return true;
    if (upper_suffix == "OGG")
        return true;
    return false;
}
