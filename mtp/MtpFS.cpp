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
    _console = NULL;
}

DirNode* MtpFS::GetRoot()
{
    return (_root);
} 
 
void MtpFS::SetRootName (const string& in_rootname)
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

string MtpFS::GetDirectoryByName(uint32_t in_id)
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
//            cout << "File node's parent is:" << temp.GetParent() << endl;
        DirNode* parent =  _dirMap[temp.GetParentID()];
        if (!parent)
        {
            cout << "Fatal error, map out of sync"<< endl;
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
    if (_root->FindFile("DevIcon.fil", idOut) )
    {
        return true;
    }
    return false;
}

bool MtpFS::DirectoryExists(uint32_t parent, const string& name)
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
        cout << "no valid parent" << endl; 
        exit(-1);
    }
    int height = parent->GetSortedOrder();
//    cout <<"Adding folder ID: " << folderID << endl;
    LIBMTP_folder_t* begin = LIBMTP_Get_Folder_List(_device);
    LIBMTP_folder_t* mtpChild = LIBMTP_Find_Folder(begin, folderID);

//    cout << "Found child to be: " << mtpChild->name << endl; 
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
    cout << "Total size reported:" << _storage->MaxCapacity<< endl;
    cout << "Free size reported:" << _storage->FreeSpaceInBytes<< endl;
    if (_storage)
        emit updateDeviceStats(quint64((_storage->MaxCapacity)), (quint64((_storage->FreeSpaceInBytes))));
    else
        cout << "No storage" << endl;
}

//private:
void MtpFS::ProgressFunc(uint64_t const sent, uint64_t const total)
{
    count_t done = ((float)sent/(float)total)*100;
    emit updateProgress(done);
}
