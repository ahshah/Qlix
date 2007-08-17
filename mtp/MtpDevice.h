//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License


#ifndef __MTPDEVICE__
#define __MTPDEVICE__
#include <iostream>
#include <libmtp.h>
#include "types.h"
#include <vector>
#include <string>
#include "DirNode.h"
#include "FileNode.h"
#include "MtpFS.h"
#include <fileref.h>
#include <tfile.h>
#include <tag.h>
#include <QDebug>
#include <QBuffer>
#include <QByteArray>
using namespace std;

class MtpDevice 
{
public:
    MtpDevice( LIBMTP_mtpdevice_t* in_device, count_t id);
    ~MtpDevice();
    void DumpInformation ( void );
    void GetFolders ( void );
    MtpFS* GetFileSystem ( void );
    LIBMTP_mtpdevice_t* rawDevice();
    uint32_t CreateFolder (const string& in_FolderName, uint32_t in_parentID, int* newmodelindex);
    bool CreateAlbum(LIBMTP_track_t*, uint32_t*);
    
    bool UpdateAlbumArt (const QString& in_path, uint32_t in_album_id);

    bool SendFileToDevice(const QFileInfo& file,uint32_t in_parentID);
    bool SendTrackToDevice(const QFileInfo& file,uint32_t in_parentID);
    bool DeleteObject(uint32_t in_parentID, int in_depth);
    bool GetFileFromDevice(uint32_t fileID, const string& target); 
    DirNode* GetDirectory(uint32_t);
    count_t GetID();


private:
    LIBMTP_mtpdevice_t* _device;
    MtpFS* _mtpFS;
    count_t _deviceID;
    string _serial;
    string _modelName;
    string _deviceVersion;
    string _friendlyName;
    string _syncPartner;
    ubyte _curBatteryLevel;
    ubyte _maxBatteryLevel;

    void getModelName();
    void getSerialNum();
    void getDeviceVersion();
    void getFriendlyName();
    void getSyncPartner();
    void getBatteryLevel();
    bool setupTrackForTransfer(const QString& in_location, uint32_t in_parentID, LIBMTP_track_t* newtrack);
    void GetErrors (LIBMTP_mtpdevice_t* in_device);
    bool isTerminal (LIBMTP_error_number_t in_err);
};

#endif
