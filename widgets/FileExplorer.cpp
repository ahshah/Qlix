//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

//public:
#include "FileExplorer.h"
FileExplorer::FileExplorer (QWidget* parent, MtpThread* in_thread) : QWidget (parent), _fsWatch(this)
{
    _deviceFileView = NULL;
     _deviceDirView  = NULL;
    _mtpThread = in_thread;
    _layout = new QHBoxLayout(this);
    _splitter = new QSplitter(this);

    setupDirModel();
    setupFileModel();
    setupConnections();
    setupFsWatch();

    _fileView->setContentsMargins(0,-5,0,-5);
    _dirView->setContentsMargins(0,-5,0,-5);
    _layout->setSpacing(0);


    QList<int> sizeList;
    sizeList.push_back(300);
    sizeList.push_back(500);
    _splitter->setSizes(sizeList);


    _layout->addWidget(_splitter);

    
}

FileExplorer::~FileExplorer()
{
}        

FileView* FileExplorer::GetFSFileView ()
{
    return _fileView;
}

void* FileExplorer::GetFSDirView ()
{
    return _dirView;
}

void FileExplorer::SetDeviceFileView (DeviceFileView* in_view)
{
    if (_fileView)
        _fileView->setDeviceFileView(in_view);

    if (_dirView)
        _dirView->setDeviceFileView(in_view);
}

//public slots:
void FileExplorer::EX (const QModelIndex& temp)
{
    _fileModel->refresh();
    QString tempLoc = _dirModel->filePath(temp);
    _fsWatch.removePath(_currentDir);
    _fsWatch.addPath(tempLoc);
    _currentDir = tempLoc;
    _fileView->setRootIndex(_fileModel->index(tempLoc));
}

void FileExplorer::refreshDirView ()
{
    //QModelIndex _currentPathIndex = _dirModel->index(_currentDir);
    _dirModel->refresh();
   // _dirSelection->setCurrentIndex(_currentPathIndex, QItemSelectionModel::ClearAndSelect |QItemSelectionModel::Rows );
}

void FileExplorer::refreshFileView ()
{
    _fileModel->refresh();
    _fileView->setRootIndex(_fileModel->index(_currentDir));
}

//private:
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

void FileExplorer::setupDirModel ()
{
    _dirModel = new QDirModel();
    _dirModel->setFilter(QDir::AllDirs| QDir::NoDotAndDotDot);
    setupDirView();
}

void FileExplorer::setupDirView ()
{
    _dirView  = new DirView(this);
    _dirView->setModel(_dirModel);
    _dirView->setAlternatingRowColors(true);
    
    QHeaderView* tempHeader = _dirView->header();
    tempHeader->hideSection(1);
    tempHeader->hideSection(2);
    tempHeader->hideSection(3);
    tempHeader->hide();
    tempHeader->resizeSection(0, 175);
    tempHeader->resizeSection(1,50);

    _dirSelection = _dirView->selectionModel();
    _splitter->addWidget(_dirView);
}

void FileExplorer::setupFileModel()
{
    _fileModel = new QDirModel();
    _fileModel->setFilter(QDir::Files);
    setupFileView();
}

void FileExplorer::setupFileView ()
{
    _fileView = new FileView(this);
    _fileView->setModel(_fileModel);
    _fileSelection = _fileView->selectionModel();
    QHeaderView* tempHeader = _fileView->header();
    tempHeader->hideSection(1);
    tempHeader->hideSection(2);
    tempHeader->hideSection(3);
    tempHeader->hide();
    _fileView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    _fileView->setAlternatingRowColors(true);
    _fileView->setRootIsDecorated(false);
    _splitter->addWidget(_fileView);
}

void FileExplorer::setupConnections ()
{
    connect (_dirSelection, 
             SIGNAL(currentChanged(const QModelIndex&,  const QModelIndex& )),
             this,
             SLOT(EX(const QModelIndex&)));
    
    connect (_dirSelection, 
             SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex& )),
             this, 
             SLOT(EX (const QModelIndex& temp)));

    connect (&_fsWatch,
             SIGNAL(directoryChanged(const QString&)),
             this,
             SLOT(refreshDirView(void)));

    connect (&_fsWatch,
             SIGNAL(directoryChanged(const QString&)),
             this,
             SLOT(refreshFileView(void)));
}

void FileExplorer::setupFsWatch()
{
    QModelIndex _currentPathIndex = _dirModel->index( QString("/mnt/media/Documents/Music"));
    _dirSelection->setCurrentIndex(_currentPathIndex, QItemSelectionModel::ClearAndSelect |QItemSelectionModel::Rows );
    QModelIndexList temp = _dirSelection->selectedRows();
    _currentDir = QDir::currentPath();
    _fsWatch.addPath(_currentDir);
}

/*
void transferSlot()
{
    if (!_fileSelection->hasSelection() )
        return;
    QModelIndexList _selected = _fileSelection->selectedRows();
    cout << "has selection of size:" <<_selected.size() <<endl;
    for (int i =0; i < _selected.size(); i++)
    {
        QFileInfo temp = _fileModel->fileInfo(_selected[i]);
        cout << "transfering file: " << temp.absoluteFilePath().toStdString() << endl;
        emit TransferFileToDevice(temp);
    }
}

void TransferToPc(const FileNode& in_file)
{
    const char* tempName = (in_file.GetName()).c_str();
    QString name(tempName);

    QModelIndexList selectedDirectories = _dirSelection->selectedRows();
    QModelIndex currentDir = selectedDirectories[0];
    QString path = _dirModel->filePath(currentDir);
    QFileInfo pathInfo(path + "/" + name);
    if (pathInfo.exists())
    {
        cout << "Ignoring transfer request, file exists already.. " << endl;
        return;
    }
    emit RequestFile(in_file, pathInfo.absoluteFilePath().toStdString());
}


    connect (_dirSelection, 
             SIGNAL(currentColChanged(const QModelIndex&, const QModelIndex& )),
             _fileSelection,
             SIGNAL(currentColChanged(const QModelIndex&, const QModelIndex& )));
    connect (_dirView, SIGNAL(expanded(const QModelIndex& )) ,
             _fileView, SLOT(expand(const QModelIndex&)));

    connect (_dirView, SIGNAL(collapsed(const QModelIndex& )) ,
             _fileView, SLOT(collapse(const QModelIndex&)));
*/
