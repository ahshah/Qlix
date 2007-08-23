//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License
#include "DeviceFileView.h"
class FileView;

DeviceFileView::DeviceFileView (QWidget* parent = NULL, 
                MtpThread* in_thread = NULL ) : QTreeView(parent)
{
    _thread = in_thread;
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    QHeaderView* tempHeader = header();
    //tempHeader->setCascadingSectionResizes(true);
    tempHeader->setResizeMode(QHeaderView::Interactive);
    tempHeader->setClickable(true);

    setAllColumnsShowFocus(true);
    setViewportMargins(0,0,0,0);
    tempHeader->update();
    setupMenus();
}

void DeviceFileView::setupMenus (void)
{
    _transferFileAct = new QAction("Transfer File", this);
    connect (_transferFileAct, SIGNAL(triggered()), this, SLOT(transferFile()));
    insertAction(NULL, _transferFileAct);

    _deleteFileAct = new QAction("Delete File", this);
    connect (_deleteFileAct, SIGNAL(triggered()), this, SLOT(deleteFile()));
    insertAction(NULL, _deleteFileAct);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}


void DeviceFileView::reset()
{
    QTreeView::reset();
//    qDebug() << "Reset called, size is: " << columnWidth(0);
}
void DeviceFileView::setFileView (FileView* in_fileView)
{
   _FSfileView = in_fileView;
}

DirNode* DeviceFileView::CurrentDirectory ()
{
    DeviceFileModel* file_model = dynamic_cast<DeviceFileModel*>(model());
    return file_model->GetCurrentDirectory();
}

//public slots:

void DeviceFileView::TransferToDevice(const QString& in_file, DirNode* directory, bool isTrack)
{
//    qDebug() << "Device recevied transfer request to device. Here is the path: " << in_file.toStdString() ;
//    qDebug() << "To directory: " << directory->GetName() ;
    DirNode* currentDir = CurrentDirectory();
    if (currentDir == NULL)
        return;
//    qDebug() << "Selected dir for file transfer: " <<  currentDir->GetName() << " id: " << currentDir->GetID()  ;
    MtpCommandSendFile* cmd = new MtpCommandSendFile(in_file, currentDir->GetID(), isTrack);
    _thread->IssueCommand(cmd);
}

void DeviceFileView::CreateDirectory(const QString& in_name)
{
    return;
}


void DeviceFileView::TransferSystemDirectory(const QString& in_file)
{
    qDebug() << "Transfer directory to device. here is the path: " << in_file;
    DirNode* current = CurrentDirectory();
    if (current == NULL)
        return;
    MtpCommandTransferSystemFolder* _cmd = new MtpCommandTransferSystemFolder(QDir(in_file), current);
    _thread->IssueCommand(_cmd);
//    qDebug() << "CurrentDirectory returned: " << current ;
//    qDebug() << "Will attempt to to create directory here: " << current->GetName() ;
}

//private slots:
void DeviceFileView::transferFile ( void )
{
    qDebug() << "transfer file slot called " ;
    QItemSelectionModel* file_selection = selectionModel();
    if (!file_selection->hasSelection())
        return;

    QModelIndexList _selected = file_selection->selectedRows();
    qDebug() << "selected " << _selected.size() << " elements" ;
    DeviceFileModel* file_model = dynamic_cast<DeviceFileModel*>(model());
    if (file_model == NULL)
    {
        qDebug() << "File model was null" ;
        exit(1);
    }

    for (int i = 0; i < _selected.size(); i++)
    {
        QModelIndex temp = _selected[i];
        FileNode tempfile = file_model->FileFromIndex(&temp);
        qDebug() << "Attempt to transfer: " << tempfile.GetFileName() ;

        if (_FSfileView == NULL)
            return;
        QString FS_location= _FSfileView->CurrentDirectory();
        QString fullPath = FS_location + "/" + tempfile.GetFileName();
        //qDebug() << "here: " << fullPath;

        QFileInfo tempFileInfo(fullPath);
        tempFileInfo.setCaching(false);
        if (tempFileInfo.exists())
        {
            qDebug() << "File exists! ignored: " << tempFileInfo.filePath();
            continue;
        }

        MtpCommandGetFile* tempCmd = new MtpCommandGetFile(tempfile.GetID(), fullPath);
        if (_thread)
        {
            qDebug() << "Issuing command" ;
            _thread->IssueCommand(tempCmd);
        }
        continue;
    }
}

void DeviceFileView::deleteFile ( void ) 
{
    QItemSelectionModel* file_selection = selectionModel();
    if (!file_selection->hasSelection())
        return;

    QModelIndexList _selected = file_selection->selectedRows();
    file_selection->clearSelection();
    DeviceFileModel* file_model = dynamic_cast<DeviceFileModel*>(model());
    // gotta delete in reverse because otherwise the heights don't get set correctly 
    for (int i = _selected.size()-1; i >= 0; i--) 
    {
        QModelIndex temp = _selected[i];
        FileNode tempfile = file_model->FileFromIndex(&temp);
        qDebug() << "Attempt to delete file: " << tempfile.GetFileName() ;
        qDebug() << "With height: " << tempfile.GetHeight() ;
        MtpCommandDelete* _cmd = new MtpCommandDelete(tempfile.GetParentID(), tempfile.GetID());
        _thread->IssueCommand(_cmd);
    }
}


/*
 *signals:
    void TransferToPc(const FileNode& tempFile);

private slots:

    void transferToPcSlot()
    {
        if (!_fileSelection->hasSelection())
            return;
        QModelIndexList _selected = _fileSelection->selectedRows();
        qDebug() << "selected " << _selected.size() << " elements" ;
        for (int i = 0; i < _selected.size(); i++)
        {
            QModelIndex temp = _selected[i];
            FileNode tempfile = _fileModel->FileFromIndex(&temp);
            emit TransferToPc(tempfile);
        }
    }

    void deleteFileOnDevice()
    {
        if (!_fileSelection->hasSelection())
            return;
        QModelIndexList _selected = _fileSelection->selectedRows();
        qDebug() << "selected " << _selected.size() << " elements" ;
        for (int i = 0; i < _selected.size(); i++)
        {
            QModelIndex temp = _selected[i];
            FileNode tempfile = _fileModel->FileFromIndex(&temp);
            MtpCommandDeleteFile* _cmd = new MtpCommandDeleteFile(tempfile);
            _mtpThread.IssueCommand(_cmd);
        }
    }

    void deleteComplete(MtpUpdateDelete* in_update)
    {
        FileNode temp = in_update->File;
        delete in_update;

        _dirModel->DeleteFile(temp);
        _dirView->reset();
        _fileView->reset();
    }

    void PullFile(const FileNode& in_file, const string& in_path)
    {
        uint32_t tempid = in_file.GetID();
        MtpCommandGetFile* temp = new MtpCommandGetFile(tempid, in_path);
        _mtpThread.IssueCommand(temp);
    }
    void TransferFileToDevice(const QFileInfo& incoming)
    {
        FileNode temp(incoming, _fileModel->CurrentFolder());
        MtpCommandSendFile* sndFile = new MtpCommandSendFile(temp);
        _mtpThread.IssueCommand(sndFile);
    }
    
    void TranferFileToDeviceComplete (MtpUpdateTransfer* in_update)
    {
        qDebug() << "Transfer File to device complete!" ;
        if (in_update->Success)
        {
            _fileModel->AddFile(in_update->File);
            _fileView->reset();
        }
        delete in_update;
    }

    void PullDeviceImage(uint32_t in_id, const string& in_path)
    {
        MtpCommandGetFile* temp = new MtpCommandGetFile(in_id, in_path);
        temp->IsRootImage = true;
        _mtpThread.IssueCommand(temp);
    }



*/

