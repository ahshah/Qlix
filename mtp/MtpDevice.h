//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License


#ifndef __MTPDEVICE__
#define __MTPDEVICE__
#include <iostream>
#include <libmtp.h>
#include "MtpParent.h"
#include "types.h"
#include <vector>
#include <string>
#include "DirNode.h"
#include "FileNode.h"
#include "MtpFS.h"
using namespace std;

class MtpDevice : protected MtpParent
{
public:
    MtpDevice( LIBMTP_mtpdevice_t* in_device );
    ~MtpDevice();
    void DumpInformation ( void );
    void GetFolders ( void );
    MtpFS* GetFileSystem ( void );
    LIBMTP_mtpdevice_t* rawDevice();
    uint32_t CreateFolder (string in_FolderName, uint32_t in_parentID, int* newmodelindex);
    bool SendFileToDevice(const QFileInfo& file,uint32_t in_parentID);
    bool DeleteObject(uint32_t in_parentID, int in_depth);
    bool GetFileFromDevice(uint32_t fileID, const string& target); 


private:
    LIBMTP_mtpdevice_t* _device;
    MtpFS* _mtpFS;

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
};

#endif
