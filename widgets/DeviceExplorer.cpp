//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License
#include "DeviceExplorer.h"
//public:
    DeviceExplorer::DeviceExplorer (QWidget* parent ) : QWidget (parent), _mtpThread(parent)
    {
        init(); 
        setupDirView();
        setupFileView();
        setupConnections();

        _fileView->setContentsMargins(-10,0,-10,0);
        _dirView->setContentsMargins(-10,0,-10,0);
        _layout->setSpacing(0);

        QList<int> sizeList;
        sizeList.push_back(300);
        sizeList.push_back(500);
        _splitter->setSizes(sizeList);
        _layout->addWidget(_splitter);
        _filesystem = NULL;
    }

    DeviceExplorer::~DeviceExplorer() { }

    void DeviceExplorer::ConnectDevice ( void )
    {
        MtpCommandConnect* connect = new MtpCommandConnect();
        _mtpThread.IssueCommand(connect);
    }

    void DeviceExplorer::DisconnectDevice ( void )
    {
        MtpCommandDisconnect* disc = new MtpCommandDisconnect();
        _filesystem = NULL;
        _mtpThread.IssueCommand(disc);
        _dirModel->Unset();
        _fileModel->Unset();
        _dirView->reset();
        _fileView->reset();
    }

    void DeviceExplorer::SetDeviceConsole (DeviceConsole* in_console)
    {
        _console = in_console;
        connect(
        &_mtpThread,
        SIGNAL (NewDeviceImage()),
        _console,
        SLOT (resetRootImage()),
        Qt::QueuedConnection
        );
    }
    
    void DeviceExplorer::SetFSFileView(FileView* in_fsfileview)
    {
        _fileView->setFileView(in_fsfileview);
    }

    DeviceFileView* DeviceExplorer::GetDeviceFileView ( void ) 
    {
        return _fileView;
    }

    DeviceDirView* DeviceExplorer::GetDeviceDirView ( void )
    {
        return _dirView;
    }

//public slots:

    void DeviceExplorer::EXB (const QModelIndex& temp)
    {
        if (!temp.isValid() )
            return;

        DirNode* dir = static_cast<DirNode*> (temp.internalPointer());
        if (dir)
            _fileModel->SetFolder(dir->GetID());
        _fileView->reset();
    }

    void DeviceExplorer::sort (int idx)
    {
        _fileModel->sort(idx);
    }

    void DeviceExplorer::ConnectComplete ( MtpFS* in_fs) // fix this
    {
        cout << "Connect complete updating structures" << endl;
        uint32_t rootImageId = 0;
        if (in_fs->FindRootImage(&rootImageId))
        {
            cout << "Found device icon!" << endl;
            string tempLoc = QDir::tempPath().toStdString();
            tempLoc += "DevIcon.png";
            pullDeviceImage(rootImageId, tempLoc);
        }
        _filesystem = in_fs;
        _filesystem->setConsole(_console);
        _dirModel->Set(in_fs);
        _fileModel->Set(in_fs);
        _fileModel->SetFolder(0);
        _dirView->reset();
        _fileView->reset();
    }

    void DeviceExplorer::ResetModelRootImage()
    {
        if (!_console)
            return;
        _dirModel->SetRootImage(_console->GetDeviceImage());
        _dirView->reset();
    }

//private:
    void DeviceExplorer::pullDeviceImage(uint32_t id, const string& loc)
    {
        MtpCommandGetFile* _cmd = new MtpCommandGetFile(id, loc);
        _cmd->IsRootImage = true;
        _mtpThread.IssueCommand(_cmd);
    }

    void DeviceExplorer::setupDirView ( void )
    {
        _dirModel = new DeviceDirModel(this);
        _dirView = new DeviceDirView(this, &_mtpThread);
        _dirView->setModel(_dirModel);
        _dirView->setAlternatingRowColors(true);
        _dirView->header()->hide();
        _splitter->addWidget(_dirView);
    }

    void DeviceExplorer:: setupFileView ( void )
    {
        _fileModel = new DeviceFileModel(this);
        _fileModel->SetFolder(0);
        _fileView = new DeviceFileView (this, &_mtpThread);
        _fileView->setModel(_fileModel);
        _splitter->addWidget(_fileView);
    }
    void DeviceExplorer::setupConnections()
    {
        qRegisterMetaType<index_t>("index_t");
        connect(
        &_mtpThread, SIGNAL(DirectoryAdded(DirNode*, index_t)),
        _dirModel, SLOT(DirectoryAdded(DirNode*,index_t)),
        Qt::QueuedConnection);

        connect(
        &_mtpThread, SIGNAL(DirectoryRemoved(bool, index_t)),
        _dirModel, SLOT(DirectoryRemoved(bool, index_t)), 
        Qt::QueuedConnection);

        connect(
        &_mtpThread, SIGNAL(FileRemoved(bool, index_t, index_t)),
        _fileModel, SLOT(FileRemoved(bool, index_t, index_t)),
        Qt::QueuedConnection);

        
        connect(
        &_mtpThread, SIGNAL(FileTransferDone(bool)),
        _fileModel, SLOT(FileTransferDone(bool)),
        Qt::QueuedConnection);

        connect(
        _dirView->selectionModel(),  SIGNAL(currentChanged(const QModelIndex&,  const QModelIndex& )),
        this, SLOT(EXB (const QModelIndex&)));

        connect(
        _dirView->selectionModel(),  SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex& )),
        this,  SLOT(EXB (const QModelIndex&)));

        connect(
        _fileView->header(), SIGNAL(sectionClicked (int)),
        this, SLOT(sort(int)));

        connect(
        &_mtpThread, SIGNAL (ConnectDone(MtpFS*)),
        this, SLOT   (ConnectComplete(MtpFS*)),
        Qt::QueuedConnection);

        connect(&_mtpThread, SIGNAL (NewDeviceImage()),
        this, SLOT (ResetModelRootImage()), 
        Qt::QueuedConnection);
    }

    void DeviceExplorer::init()
    {
        _layout = new QHBoxLayout(this);
        _splitter = new QSplitter(this);
    }

