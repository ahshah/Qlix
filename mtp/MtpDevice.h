/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This file is part of the Qlix project on http://berlios.de
 *
 *   This file may be used under the terms of the GNU General Public
 *   License version 2.0 as published by the Free Software Foundation
 *   and appearing in the file COPYING included in the packaging of
 *   this file.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License version 2.0 for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
typedef map<uint32_t, MTP::Folder* > FolderMap;
typedef map<uint32_t, MTP::File* > FileMap;
typedef map<uint32_t, MTP::Track* > TrackMap;
typedef map<uint32_t, MTP::Album* > AlbumMap;
typedef map<uint32_t, MTP::Playlist* > PlaylistMap;

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
  MTP::Folder* RootFolder() const;

//basic actions
  bool Fetch(uint32_t, char const * const );
  bool UpdateSpaceInformation();
  void FreeSpace(unsigned int, uint64_t*, uint64_t*);

  PlaylistMap GetPlaylistMap () const;
  FolderMap GetFolderMap() const;
  TrackMap  GetTrackMap()  const;
  FileMap   GetFileMap()   const;
  AlbumMap  GetAlbumMap()   const;

  vector<MTP::Album*> Albums() const;
  vector<MTP::Playlist*> Playlists() const;
  /*
  MTP::File* const FindFile(count_t in_id) const;
  MTP::Folder* const FindFolder(count_t in_id) const;
  MTP::Album* const FindAlbum(count_t in_id) const;
  MTP::Playlist* const FindPlaylist(count_t in_id) const;
  MTP::Track* const FindTrack(count_t in_id) const;
  */

//Device structures information
  count_t AlbumCount() const;
  count_t PlaylistCount() const;

  MTP::Album* Album(count_t idx) const;
  MTP::Playlist* Playlist(count_t idx) const;
  MTP::Folder* RootFolder(count_t idx) const;

//Album functions
  bool NewAlbum(MTP::Track*, int, MTP::Album** );
  bool RemoveAlbum(MTP::Album*);

//Extended Album functions
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
private:
  LIBMTP_mtpdevice_t* RawDevice() const;
  MTP::GenericObject* const find(count_t in_id, MtpObjectType type) const;
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

  bool removeObject(count_t);

  MTP::Folder* _rootFolder;

  LIBMTP_progressfunc_t _progressFunc;
  const void* _progressData;

  GenericMap  _objectMap;
  TrackMap    _trackMap;
  AlbumMap    _albumMap;
  PlaylistMap _playlistMap;
  FileMap     _fileMap;
  FolderMap   _folderMap;

  vector <MtpStorage*> _storageDeviceList;
  vector <string> _errorStack;
  vector <string> _supportedFileTypes;
  vector <MTP::Folder*> _rootFolders;
  vector <MTP::File*> _rootFiles;

  //These are used to display specific views of the device
  vector <MTP::Track*> _tracks;
  vector <MTP::Album*> _albums;
  vector <MTP::Playlist*> _playlists;

  //Container structure functions
  void createObjectStructure();
  void createFolderStructure(MTP::Folder*, bool);
  void createFileStructure();
  void createTrackBasedStructures();

  //Error Functions 
  void processErrorStack();
  //Debug functions
  void dbgPrintSupportedFileTypes();
  void dbgPrintFolders(MTP::Folder*, count_t);
};
#endif 
