//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License
#ifndef __DIRNODE__
#define __DIRNODE__
#include <iostream>
#include <libmtp.h>
#include <algorithm>
#include <QString>
#include <QtDebug>
#include "types.h"
#include "MtpDir.h"
#include "FileNode.h"
#include "MtpFS.h"
using namespace std;
class MtpFS;
class DirNode
{
public:
    DirNode(LIBMTP_folder_t* begin, MtpFS* in_fs);
    DirNode (LIBMTP_folder_t* in_folder, MtpFS* in_fs, count_t in_height);
    ~DirNode ( void );

    count_t SubDirectoryCount();
    count_t GetFileCount();

    count_t GetSortedOrder(); 
    QString GetName() const;

    count_t GetID () const; 
    DirNode* GetParentDir(); 

    void SetName(const QString& in_str); 
    void SetSortedOrder(count_t in); 

    void AddSubDirectory (DirNode* in);
    void AddSortedSubDirectory (DirNode* in);

    FileNode GetFile(count_t idx);
    DirNode* GetSubDirectory(count_t in_index);

    void CreateMapping (map <uint32_t, DirNode*>& in_map );

    void SetParent (DirNode* in_parent);
    void AddFile(const FileNode& temp);

    void DeleteSubDirectory(index_t idx);
    void DeleteFile(index_t id);

    void SortDirectories();

    bool FindFile(const QString& in_name, uint32_t* id);
    void SortFiles(FileSortType );

    void GetSubTree (DirectoryIDList*);
    bool DirectoryExists(const QString& in_name);
    bool FileExists(const QString& in_name);

private:

    MtpFS* _fileSystem;
    MtpDir _currentDir;
    DirNode* _parent;
    QString _deviceID;

    vector <DirNode*> _subDirectories;
    vector <FileNode> _files; 
    void discoverChildren(LIBMTP_folder_t* in_folder);

    FileSortType _lastDirSort;
    FileSortType _lastFileSort;

    void deleteAllSubDirectories();
    void shallowDirectorySort();

    void sortFileNamesUp();
    void sortFileNamesDown();
    void sortSizeUp();
    void sortSizeDown();

    static bool compareFileNames(FileNode one, FileNode two);
    static bool compareFileSizes(FileNode one, FileNode two);
    static bool compareDirNames(DirNode* one, DirNode* two);

};
#endif
