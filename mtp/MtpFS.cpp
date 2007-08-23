//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License
#include "MtpFS.h"
MtpFS::MtpFS( LIBMTP_folder_t* in_root, LIBMTP_mtpdevice_t* in_dev) : QObject(NULL)
{
    if (!in_dev)
        return;
    _mtpRoot = in_root;
    _device = in_dev;
    _storage = _device->storage;
    _root = new DirNode(in_root, this);
    _root->SortDirectories();
    _root->CreateMapping(_dirMap);
    _root->SortDirectories();
    qDebug() << "FS internal storage is: " << _storage;
    qDebug() <<"Device storage is: " << _device->storage<<endl;
    qDebug() << "Initial total size reported:" << _storage->MaxCapacity<< endl;
    qDebug() << "Initial Free size reported:" << _storage->FreeSpaceInBytes<< endl;
    _console = NULL;
}

DirNode* MtpFS::GetRoot()
{
    return (_root);
} 
 
void MtpFS::SetRootName (const QString& in_rootname)
{
        _root->SetName(in_rootname);
}

count_t MtpFS::GetSortedOrder(uint32_t in_id)
{
    DirNode* temp = _dirMap[in_id];
    if (temp) 
        return temp->GetSortedOrder();
    else
        return 0;
}

DirNode* MtpFS::GetDirectory (uint32_t in_id)
{
    DirNode* temp = _dirMap[in_id];
    if (!temp)
        return NULL;
   return temp;
}

QString MtpFS::GetDirectoryByName(uint32_t in_id)
{
    DirNode* temp = GetDirectory(in_id);
    if (temp == NULL)
        return "";
    return temp->GetName();
}


DirNode* MtpFS::GetDirectory (DirNode* in_parent, uint32_t in_index)
{
    if (!in_parent)
        return NULL;
    if (in_parent == _root)
        return in_parent->GetSubDirectory(in_index);
    DirNode* parent =_dirMap[in_parent->GetID()];
    DirNode* temp   = parent->GetSubDirectory(in_index);
    return temp;
}

void MtpFS::InsertFileList (LIBMTP_file_t* in_rootFile)
{
    for (int i = 0; in_rootFile; i++)
    {
        FileNode temp(in_rootFile, i);
//            qDebug() << "File node's parent is:" << temp.GetParent();
        DirNode* parent =  _dirMap[temp.GetParentID()];
        if (!parent)
        {
            qDebug() << "Fatal error, map out of sync"<< endl;
            in_rootFile = in_rootFile->next;
            continue;
        }
        index_t newindex = parent->GetFileCount();
        temp.SetHeight(newindex);
        parent->AddFile(temp);
        in_rootFile = in_rootFile->next;
    }
}

count_t MtpFS::GetRootFileCount()
{
    return _root->GetFileCount();
}

void MtpFS::AddFile (const FileNode& in_file)
{
    DirNode* tempDir = _dirMap[in_file.GetParentID()];
    if (!tempDir)
        return;
    tempDir->AddFile(in_file);
}

bool MtpFS::FindRootImage( uint32_t* idOut)
{
    if (_root->FindFile("devicon.fil", idOut) || _root->FindFile("DevIcon.fil", idOut ))
    {
        return true;
    }
    return false;
}

bool MtpFS::DirectoryExists(uint32_t parent, const QString& name)
{
    DirNode* parentDir = GetDirectory(parent);
    if (parentDir == NULL)
        return false;
    return parentDir->DirectoryExists(name);
}

void MtpFS::DeleteFolder(uint32_t folder_id)
{
    DirNode* subDir = _dirMap[folder_id];
    if (!subDir)
        assert(false);
    DirectoryIDList temp;
    subDir->GetSubTree(&temp);
    DirNode* parent = subDir->GetParentDir();
    parent->DeleteSubDirectory(folder_id);
    for (count_t i = 0; i < temp.size(); i++)
    {
        _dirMap[temp[i]] = NULL;
    }

}

void MtpFS::DeleteFile (const FileNode& in_file)
{
    uint32_t parentid = in_file.GetParentID();
    DirNode* tempDir = _dirMap[parentid];
    if (!tempDir)
        return;
    tempDir->DeleteFile(in_file.GetID());
}

void MtpFS::DeleteDirectoryEntry(uint32_t ID)
{
    _dirMap[ID] =NULL;
}

int MtpFS::AddFolder(uint32_t folderID, uint32_t parentID)
{
    DirNode* parent = GetDirectory(parentID);
    if (parent == NULL)
    {
        qDebug() << "no valid parent"; 
        exit(-1);
    }
    int height = parent->GetSortedOrder();
//    qDebug() <<"Adding folder ID: " << folderID;
    LIBMTP_folder_t* begin = LIBMTP_Get_Folder_List(_device);
    LIBMTP_folder_t* mtpChild = LIBMTP_Find_Folder(begin, folderID);

//    qDebug() << "Found child to be: " << mtpChild->name; 
    DirNode* child = new DirNode(mtpChild, this, height);
    child->SetParent(parent);

    parent->AddSortedSubDirectory(child);
    _dirMap[folderID] = child;

    return child->GetSortedOrder(); 
}

void MtpFS::setConsole(DeviceConsole* in_console)
{
    connect(this, SIGNAL(updateProgress(unsigned int)),
            in_console, SLOT(UpdateProgressBar(unsigned int)), Qt::QueuedConnection);

    connect(this, SIGNAL(updateDeviceStats(quint64, quint64)),
            in_console, SLOT(UpdateDeviceStats(quint64, quint64)), Qt::QueuedConnection);
    updateConsole();

}

int MtpFS::ProgressWrapper(uint64_t const sent, uint64_t const total, void const * const data) 
{
    const MtpFS * const temp = static_cast<const MtpFS* const> (data);
    MtpFS * const filesystem = const_cast<MtpFS* const > (temp);
    filesystem->ProgressFunc(sent, total);
    return 0;
}

void MtpFS::updateConsole()
{
    qDebug() << "Total size reported:" << (long long)_device->storage->MaxCapacity<< endl;
    qDebug() << "Free size reported:" << (long long)_device->storage->FreeSpaceInBytes<< endl;
    if (_device->storage)
        emit updateDeviceStats(quint64((_device->storage->MaxCapacity)), (quint64((_device->storage->FreeSpaceInBytes))));
    else
        qDebug() << "No storage";
}

//private:
void MtpFS::ProgressFunc(uint64_t const sent, uint64_t const total)
{
    count_t done = ((float)sent/(float)total)*100;
    emit updateProgress(done);
}
