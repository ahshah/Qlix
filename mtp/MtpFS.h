//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __MTPFS__
#define __MTPFS__
#include <libmtp.h>
#include <vector>
#include <map>
#include <cassert>
#include <QString>
#include <QDebug>
#include "types.h"
#include "DirNode.h"
#include "DeviceConsole.h"
class DirNode;
class MtpFS : QObject
{
    Q_OBJECT
public:
    MtpFS( LIBMTP_folder_t* in_root, LIBMTP_mtpdevice_t* in_dev);
    DirNode* GetRoot();
    void SetRootName (const QString& in_rootname);

    count_t GetSortedOrder(uint32_t in_id);
    DirNode* GetDirectory (uint32_t in_id);
    QString GetDirectoryByName(uint32_t in_id);
    DirNode* GetDirectory (DirNode* in_parent, uint32_t in_index);
    void InsertFileList (LIBMTP_file_t* in_rootFile);
    count_t GetRootFileCount();

    void AddFile (const FileNode& in_file);
    bool FindRootImage( uint32_t* idOut);
    void DeleteFolder(uint32_t folder_id);
    void DeleteDirectoryEntry(uint32_t folder_id);
    void DeleteFile (const FileNode& in_file);
    int AddFolder(uint32_t folderID, uint32_t parentID);
    bool DirectoryExists(uint32_t parent, const QString& name);

    static int ProgressWrapper(uint64_t const sent, uint64_t const total, void const * const data) ;
    void ProgressFunc(uint64_t const sent, uint64_t const total);
    void updateConsole();
    void setConsole(DeviceConsole* in_console);

signals:
    void updateProgress(unsigned int);
    void updateDeviceStats(quint64, quint64);
private:
    DirNode* _root;
    LIBMTP_folder_t* _mtpRoot;
    LIBMTP_mtpdevice_t* _device;
    LIBMTP_devicestorage_t* _storage;
    map <uint32_t, DirNode*> _dirMap;
    DeviceConsole* _console;
};
#endif

