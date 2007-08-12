//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __COMMANDCODES__
#define __COMMANDCODES__
#include "types.h"
#include "MtpFS.h"
#include "MtpPortal.h"
#include <QDir>
#include <QString>
#include <QList>
using namespace std;

enum MtpCommandCode
{
    Connect,
    Disconnect,
    GetDeviceInfo,
    GetFile,
    SendFile,
    Delete,
    GetDirMetaData,
    GetFileMetaData,
    GetSampleData,
    SendSampleData,
    CreateFolder,
    TransferDeviceFolder,
    TransferSystemFolder
}; 

struct MtpDeviceInfo
{
    string _serial;
    string _modelName;
    string _friendlyName;
    string _syncPartner;
    ubyte _curBatteryLevel;
    ubyte _maxBatteryLevel;
};

struct MtpCommand
{
    MtpCommandCode ComCode;
    MtpCommandCode GetCommand()
    {
        return ComCode;
    }
};

struct MtpUpdate
{
    MtpCommandCode ComCode;
    bool Success;
    MtpCommandCode GetCommand()
    {
        return ComCode;
    }

    bool isSuccess()
    {
        return Success;
    }
};

struct MtpCommandGetDeviceInfo : MtpCommand
{
    MtpCommandGetDeviceInfo ( void )
    {
        ComCode = GetDeviceInfo;
    }
};


struct MtpUpdateDeviceInfo
{
    //todo
    MtpDeviceInfo Info;
};

struct MtpCommandConnect : MtpCommand
{
    MtpCommandConnect (void )
    {
        ComCode = Connect;
    }
};

struct MtpUpdateConnect: MtpUpdate
{
    MtpFS* MtpFileSystem;
    MtpUpdateConnect(bool in_success, MtpFS* in_fs)
    {
        MtpFileSystem = in_fs;
        Success = in_success;
    }
};

struct MtpCommandDisconnect : MtpCommand
{
    MtpCommandDisconnect ( void )
    {
        ComCode= Disconnect;
    }
};


struct MtpCommandGetFile : MtpCommand
{
    string FileName;
    uint32_t ID;
    bool IsRootImage;
    MtpCommandGetFile (uint32_t file_id, const string& in_FileName)
    {
        ID = file_id;
        FileName = in_FileName;
        ComCode = GetFile;
        IsRootImage = false;
    }
};

struct MtpCommandSendFile : MtpCommand
{
    string Path;
    uint32_t ParentID;
    bool IsTrack;
    MtpCommandSendFile (string in_path, uint32_t in_ParentID, bool in_asTrack = false) 
    {
        ParentID = in_ParentID;
        Path = in_path;
        IsTrack = in_asTrack;
        ComCode = SendFile;
    }
};

struct MtpCommandDelete : MtpCommand
{
    uint32_t FolderID;
    int FileID;
    MtpCommandDelete (uint32_t in_Folderid, int in_FileID)
    {
        FolderID = in_Folderid;
        FileID = in_FileID;
        ComCode = Delete;
    }
};

struct MtpCommandTransferSystemFolder : MtpCommand
{
    QFileInfoList Files;
    DirNode* Parent;
    QString DirName;
    MtpCommandTransferSystemFolder(QDir folder, DirNode* parent)
    {
        ComCode = TransferSystemFolder;
        DirName = folder.dirName();
        Parent = parent;
        Files = folder.entryInfoList();
    }
};

struct MtpUpdateDelete : MtpUpdate
{
    uint32_t FolderID;
    int Depth;
    MtpUpdateDelete (bool in_success, uint32_t in_Folderid, int in_depth) 
    {
        Success = in_success;
        FolderID = in_Folderid;
        Depth = in_depth;
    }
};


struct MtpUpdateTransfer : MtpUpdate
{
    uint32_t FolderID;
    MtpUpdateTransfer(bool in_success, uint32_t in_Folderid)
    {
        FolderID = in_Folderid;
        Success = in_success;
    }
};

/*
struct MtpThreadData
{
    MtpCommand* Command;
};

*/
#endif
