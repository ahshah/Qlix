//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#include "DirNode.h"

DirNode::DirNode(LIBMTP_folder_t* begin, MtpFS* in_fs ) :_currentDir(begin)
{
    _fileSystem = in_fs;
   _parent = NULL;
    for(count_t i = 0; begin; i++)
    {
        //cout << "creating:  "<< i << ": "<< begin->name << endl;
        DirNode* tempNode = new DirNode(begin, _fileSystem, i);
        tempNode->SetParent(this);
        _subDirectories.push_back(tempNode);
        begin = begin->sibling;
    }
}

DirNode::DirNode (LIBMTP_folder_t* in_folder,MtpFS* in_fs, count_t in_height)
        :_currentDir(in_folder, in_height)
{
    _fileSystem = in_fs;
    discoverChildren(in_folder);
}

DirNode::~DirNode ( void )
{
    for (count_t i = 0; i < _subDirectories.size(); i++)
    {
        delete _subDirectories[i];
        _subDirectories[i] = NULL;
    }
}

void DirNode::GetSubTree(DirectoryIDList* in)
{
    for (count_t i = 0; i < _subDirectories.size(); i++)
    {
        _subDirectories[i]->GetSubTree(in);
    }
    in->push_back(GetID());
}

count_t DirNode::GetFileCount()
{
    return _files.size();
}

count_t DirNode::GetSortedOrder() 
{
    return _currentDir.SortedOrder; 
}

string DirNode::GetName() const 
{
    return _currentDir.Name;
}

count_t DirNode::GetID () const
{ 
    return _currentDir.FolderId;
}

DirNode* DirNode::GetParentDir() 
{
    return _parent; 
}

count_t DirNode::SubDirectoryCount() 
{ 
    return _subDirectories.size(); 
}

void DirNode::SetName(const string& in_str) 
{ 
    _currentDir.Name = in_str;
}

void DirNode::SetSortedOrder(count_t in)
{
    _currentDir.SortedOrder = in;
}

void DirNode::AddSubDirectory( DirNode* in ) 
{
    _subDirectories.push_back(in);  
}

void DirNode::AddSortedSubDirectory (DirNode* in)
{
    _subDirectories.push_back(in);
    shallowDirectorySort();
}

FileNode DirNode::GetFile(count_t idx)
{
    return _files[idx];
}

DirNode* DirNode::GetSubDirectory (count_t in_index)
{
    if (in_index >= _subDirectories.size() )
       return NULL;
    return _subDirectories[in_index];
}

bool DirNode::DirectoryExists(const string& name)
{
    vector<DirNode*>::iterator iter;
    for (iter = _subDirectories.begin(); iter != _subDirectories.end(); iter++)
    {
        DirNode* current = (*iter);
        if (name == current->GetName())
        {
            return true;
        }
    }
    return false;
}

void DirNode::CreateMapping (map <uint32_t, DirNode*>& in_map )
{
//         cout << "Created mapping for: " << GetName() << endl;
    in_map[GetID()] = this;
    for (count_t i = 0; i < _subDirectories.size(); i++)
    {
        (_subDirectories[i])->CreateMapping(in_map); 
    }
}

void DirNode::SetParent (DirNode* in_parent)
{
    _parent = in_parent;
}

void DirNode::AddFile(const FileNode& temp)
{
    _files.push_back(temp);
}

void DirNode::SortDirectories()
{
    vector<DirNode*>::iterator iter;
    sort (_subDirectories.begin(), _subDirectories.end(), DirNode::compareDirNames);
    for (count_t i = 0; i < _subDirectories.size(); i++)
    {
        _subDirectories[i]->SetSortedOrder(i);
    }
    for (count_t i = 0; i< _subDirectories.size(); i++)
    {
        _subDirectories[i]->SortDirectories();
    }
}

bool DirNode::FileExists(const string& in_name)
{
    for (count_t i = 0; i < _files.size(); i++)
    {
        if (_files[i].GetName() == in_name)
        {
            FileNode duplicate = _files[i];
            cout <<"Current directory: " << GetName() << endl;
            cout <<"Entered name: " << in_name << endl;
            cout << "Duplicate name: "<< duplicate.GetName() << endl;
            cout << "Duplicate ID: "<< duplicate.GetID() << endl;
            return true;
        }
    }
    return false;
}

bool DirNode::FindFile(const string& in_name, uint32_t* id)
{
    for (count_t i = 0; i < _files.size(); i++)
    {
        if (_files[i].GetName() == in_name)
        {
            (*id) = _files[i].GetID();
            return true;
        }
    }
    for (count_t i = 0; i < _subDirectories.size(); i++)
    {
        uint32_t tempid;
        if(_subDirectories[i]->FindFile(in_name, &tempid))
        {
            (*id) = tempid;
            return true;
        }
    }
    return false;
}

void DirNode::DeleteSubDirectory(index_t id)
{
    vector<DirNode*>::iterator iter = _subDirectories.begin();
    for (iter = _subDirectories.begin(); iter != _subDirectories.end(); iter++)
    {
        DirNode* current = (*iter);
        if (id == current->GetID())
        {
            _subDirectories.erase(iter);
            delete current;
            return;
        }
    }
    cout << "error trying to delete under:" << GetName() << " ID to be deleted: " << id << endl;
}




void DirNode::DeleteFile (index_t id)
{
    vector<FileNode>::iterator iter = _files.begin();
    for (iter = _files.begin(); iter != _files.end(); iter++)
    {
        if (id == (*iter).GetID())
        {
            _files.erase(iter);
            return;
        }
    }
    cout << "Big error no file founde under this folder:" << GetName() << endl;
}

void DirNode::SortFiles (FileSortType sortType)
{
    _lastFileSort = sortType;
    switch (sortType)
    {
        case SortByNameUp:
            sortFileNamesUp();
            break;

        case SortByNameDown:
            sortFileNamesDown();
            break;

        case SortBySizeUp:
            sortSizeUp();
            break;

        case SortBySizeDown:
            sortSizeDown();
            break;

        case SortByTypeUp:
            break;

        case SortByTypeDown:
            break;

        default:
            sortFileNamesUp();
     }
}



//private:
void DirNode::discoverChildren(LIBMTP_folder_t* in_folder)
{
    if (!in_folder)
        return;
    in_folder = in_folder->child;
    for (count_t i = 0; in_folder; i++)
    {
        DirNode* temp = new DirNode(in_folder, _fileSystem, i);
        temp->SetParent(this);
        _subDirectories.push_back( temp );
        in_folder = in_folder->sibling;
    }
}

void DirNode::deleteAllSubDirectories()
{
}

void DirNode::shallowDirectorySort()
{
    vector<DirNode*>::iterator iter;
    sort (_subDirectories.begin(), _subDirectories.end(), DirNode::compareDirNames);
    for (count_t i = 0; i < _subDirectories.size(); i++)
    {
        _subDirectories[i]->SetSortedOrder(i);
    }
}

void DirNode::sortFileNamesUp()
{
    sort (_files.begin(), _files.end(), DirNode::compareFileNames);
    for (index_t i = 0; i < _files.size(); i++)
    {
        FileNode temp = _files[i];
        temp.SetHeight(i);
        _files[i] = temp;
    }
}

void DirNode::sortFileNamesDown()
{
    sort (_files.rbegin(), _files.rend(), DirNode::compareFileNames);
    for (index_t i = 0; i < _files.size(); i++)
    {
        FileNode temp = _files[i];
        temp.SetHeight(i);
        _files[i] = temp;
    }
}

void DirNode::sortSizeUp()
{
    sort (_files.begin(), _files.end(), DirNode::compareFileSizes);
    for (index_t i = 0; i < _files.size(); i++)
    {
        FileNode temp = _files[i];
        temp.SetHeight(i);
        _files[i] = temp;
    }
}

void DirNode::sortSizeDown()
{
    sort (_files.rbegin(), _files.rend(), DirNode::compareFileSizes);
    for (index_t i = 0; i < _files.size(); i++)
    {
        FileNode temp = _files[i];
        temp.SetHeight(i);
        _files[i] = temp;
    }
}

bool DirNode::compareFileNames(FileNode one, FileNode two)
{
    return (one.GetName() < two.GetName());
}

bool DirNode::compareFileSizes(FileNode one, FileNode two)
{
    return (one.GetSize() < two.GetSize());
}


bool DirNode::compareDirNames(DirNode* one, DirNode* two)
{
    return (one->GetName() < two->GetName());
}



//AddSortedChild
    //cout << "Added child " << in->GetName() << " to: " << GetName() << " brings total to: " << _subDirectories.size()  <<" previous was: " << previous << endl; //    cout << "child id:  " << in->GetID() << " parent id: " << GetID() << endl;
    /* cout << "--Sanity check---"  << endl; cout << "Added child's height:" << in->GetHeight() << endl; cout << "Added child's depth:" << in->GetDepth() << endl; if (_subDirectories.size() > 0) { cout << "Siblling height:" << _subDirectories[0]->GetHeight() << endl; cout << "Siblling depth:" << _subDirectories[0]->GetDepth() << endl; }*/

