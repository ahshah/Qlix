#ifndef MTPDEVICE
#define MTPDEVICE
#include <libmtp.h>
#include <string>
#include <map>
#include <iostream>
//Taglib includes
#include <tag.h>
#include <fileref.h>
#include "types.h"
#include "mtp/MtpObject.h"
#include "mtp/MtpStorage.h"

using namespace std;
typedef map<uint32_t, MTP::GenericObject* > GenericMap;

/** 
 * @class MtpDevice is a wrapper class around libmtp's device module
*/
class MtpDevice
{
public:
//Internal functions
  MtpDevice(LIBMTP_mtpdevice_t* in_);
  ~MtpDevice();
  void Initialize();
  void SetProgressFunction(LIBMTP_progressfunc_t, const void* const );
  void ClearObjectMappings();

//basic device properties
  char const * const Name() const;
  char const * const SerialNumber() const;
  char const * const Version() const;
  char const * const SyncPartner() const;
  char const * const ModelName() const;
  bool BatteryLevelSupport() const;
  float BatteryLevel() const;
  unsigned int StorageDeviceCount() const;
  MtpStorage* StorageDevice(unsigned int) const;

//basic actions
  bool Fetch(uint32_t, char const * const );
  bool UpdateSpaceInformation();
  void FreeSpace(unsigned int, uint64_t*, uint64_t*);

//Device structures information
  count_t AlbumCount() const;
  count_t PlaylistCount() const;

  MTP::Album* Album(count_t idx) const;
  MTP::Playlist* Playlist(count_t idx) const;

  count_t RootFolderCount() const;
  count_t RootFileCount() const;
  MTP::Folder* RootFolder(count_t idx) const;
  MTP::File* RootFile(count_t idx) const;

//Extended Album functions
  bool NewAlbum(MTP::Track*, int, MTP::Album** );
  bool RemoveAlbum(MTP::Album*);
  bool UpdateAlbumArt(MTP::Album*, LIBMTP_filesampledata_t*);
  void AddAlbum(MTP::Album*);

// Track/Album
  bool AddTrackToAlbum(MTP::Track*, MTP::Album*);
  bool RemoveTrackFromAlbum(MTP::Track*, MTP::Album*);

//Track/Playlist functions
  bool AddTrackToPlaylist(MTP::Track*, MTP::Playlist*);
  bool RemoveTrackFromPlaylist(MTP::Track*, MTP::Playlist*);

//Extended Track functions
  bool RemoveTrack(MTP::Track*);
  bool TransferTrack(const char*, MTP::Track*);

//Extended File functions 
  bool RemoveFile(MTP::File*);
  bool TransferFile(const char*, MTP::File*);

//Extended Folder functions
  bool NewFolder(MTP::Folder*);
  bool RemoveFolder(MTP::Folder*);

  LIBMTP_filesampledata_t* DefaultJPEGSample();
  //should this be deprecated?
private:
  LIBMTP_mtpdevice_t* RawDevice() const;
  LIBMTP_mtpdevice_t* _device;
  bool _initialized;
  char* _name;
  char* _serialNumber;
  char* _version;
  char* _syncPartner;
  char* _modelName;
  count_t _maxBatteryLevel;
  count_t _currentBatteryLevel;
  bool _batteryLevelSupport;

  LIBMTP_progressfunc_t _progressFunc;
  const void* _progressData;

  GenericMap _objectMap;
  map<uint32_t, MTP::Track*> _trackMap;
  map<uint32_t, MTP::Album*> _albumMap;
  map<uint32_t, MTP::Playlist*> _playlistMap;
  void processErrorStack();

  vector <MtpStorage*> _storageDeviceList;
  vector <string> _errorStack;
  vector <string> _supportedFileTypes;
  vector <MTP::GenericObject*> _crossLinked;
  vector <MTP::Folder*> _rootFolders;
  vector <MTP::File*> _rootFiles;

  vector <MTP::Track*> _tracks;
  vector <MTP::Album*> _albums;
  vector <MTP::Playlist*> _playlists;

  //Container structure functions
  void createObjectStructure();
  void createFolderStructure(MTP::Folder*, bool);
  void createFileStructure();
  void createTrackBasedStructures();

  //Debug functions
  void dbgPrintSupportedFileTypes();
  void dbgPrintFolders(MTP::Folder*, count_t);
};
#endif 
