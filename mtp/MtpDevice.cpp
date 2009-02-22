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

TODO Improve error handling / Reporting (once theres an error console)
TODO Deleting tracks does not update the free space reported.
TODO Remove ifdefs for SIMULATE_TRANSFERS and make it an option we can pass
     qlix

TODO Add InsanePlaylist, InsaneAlbums flags that allows us to avoid the
     assertions made WRT necessary MTP file associations with all other types
TODO Add autocleanup of broken playlists and albums
TODO Autocleanup of broken playlists and albums should attempt to find the raw
     file structure and attempt to create track based information from it
     this might be tedious since you have to read tag information and this can
     only be done if you transfer the file over to the computer

TODO What should we do if playlist items are located on another storage?
     Does the find function behave correctly in this case?

TODO Q. Should raw object references returns be of const types?
     A. No this is a bad idea as sending files updates the file id we discover

TODO Storage IDs are not correctly handled- recheck this- it should be fixed

TODO Remove RootFolder(count_t idx) function
TODO ReWrite dbgPrintFolders to account for the fake root folder we create

TODO Implement NewFolder and RemoveFolder
TODO When a file transfer is complete the libmtp struct may have new information
     this information needs to get propagated up the c++ structs
     This is especially the case when some devices do not support utf8
     characters- libmtp modifies these names before sending it over the wire

TODO !BLOCKING!: Find a way to retrieve default parent handles for albums and playlists
*/

#include "MtpDevice.h"
//#define SIMULATE_TRANSFERS
/**
 * Creates a MtpDevice
 * @param in_device The raw LIBMtp device
 * @return Returns the requested device
 */
MtpDevice::MtpDevice(LIBMTP_mtpdevice_t* in_device, CommandLineOptions in_opts)  :
                     _initialized(false),
                     _commandLineOpts(in_opts),
                     _rootFolder(NULL)
{
  _device = in_device;
  _serialNumber = LIBMTP_Get_Serialnumber(_device);
  UpdateSpaceInformation();
}

/**
 * Deletes allocated objects
 */
MtpDevice::~MtpDevice()
{
  if (_name)
  {
    delete _name;
    _name = NULL;
  }

  if (_serialNumber)
  {
    delete _serialNumber;
    _serialNumber = NULL;
  }

  if (_version)
  {
    delete _version;
    _version = NULL;
  }

  if (_syncPartner)
  {
    delete _syncPartner;
    _syncPartner = NULL;
  }

  if (_modelName)
  {
    delete _modelName;
    _modelName = NULL;
  }
}

/**
 * Initializes the MtpDevice and creates an internal tree structure for all
 * MTP objects
 */
void MtpDevice::Initialize()
{
  cout << "Initializing device.. "  << endl;
  if (!_device)
    return;
  assert(_initialized== false);

//  _progressFunc= NULL;
   _name = LIBMTP_Get_Friendlyname(_device);
   if (!_name)
     _name=strdup("MTP Device");

  _version = LIBMTP_Get_Deviceversion(_device);
  _syncPartner= LIBMTP_Get_Syncpartner(_device);
  _modelName = LIBMTP_Get_Modelname(_device);

  uint8_t max;
  uint8_t cur;
  int ret = LIBMTP_Get_Batterylevel(_device, &max, &cur);

  if (ret != 0)
    _batteryLevelSupport = false;

  _maxBatteryLevel     = max;
  _currentBatteryLevel = cur;
  ret = 0;


  uint16_t count = 0;
  uint16_t* types;
  ret = LIBMTP_Get_Supported_Filetypes(_device, &types, &count);
  if (ret == 0)
  {
    for (count_t i =0; i < count; i++)
    {
      char const* str;
      str = LIBMTP_Get_Filetype_Description( (LIBMTP_filetype_t)types[i] );
      string s(str);
      _supportedFileTypes.push_back(s);
    }
  }
  if (types)
    free(types);
  createObjectStructure();
  UpdateSpaceInformation();
  _initialized = true;
}

/**
 * Returns the total and free space on the requested storage device
 * @param in_ID the storage ID of the storage device
 * @param total the total amount of space on the device
 * @param free the amount of free space on the device
 */
void MtpDevice::FreeSpace(unsigned int in_ID, uint64_t* out_total, uint64_t* out_free)
{
  for (unsigned int i =0; i < _storageDeviceList.size(); i++)
  {
    if (_storageDeviceList[i]->ID() == in_ID)
    {
      MtpStorage* storage_dev = _storageDeviceList[i];

      if (_commandLineOpts.DebugOutput)
      {
        cout << "Storage reporting: " << storage_dev->TotalSpace() << " and " <<
                 storage_dev->FreeSpace() << endl;;
      }
      *out_total = storage_dev->TotalSpace();
      *out_free = storage_dev->FreeSpace();
      return;
    }
  }
  *out_total = 0;
  *out_free = 0;
}

PlaylistMap MtpDevice::GetPlaylistMap () const { return _playlistMap; }
FolderMap MtpDevice::GetFolderMap() const { return _folderMap; }
FileMap   MtpDevice::GetFileMap () const { return _fileMap; }
TrackMap  MtpDevice::GetTrackMap () const { return _trackMap; }
AlbumMap  MtpDevice::GetAlbumMap () const { return _albumMap; }

vector<MTP::Album*> MtpDevice::Albums() const { return _albums; }
vector<MTP::Playlist*> MtpDevice::Playlists() const { return _playlists; }

/**
 * @return the Folder at the given index or NULL if it is out of bounds
 */
MTP::Folder* MtpDevice::RootFolder(count_t idx) const
{
  if (idx > _rootFolders.size() )
    return NULL;
  return _rootFolders[idx];
}

/**
 * @return the number of albums on the device
 */
count_t MtpDevice::AlbumCount() const
{ return _albums.size(); }

/**
 * @return the number of playlists on the device
 */
count_t MtpDevice::PlaylistCount() const
{ return _playlists.size(); }

/**
 * @return the Album at the given index or NULL if it is out of bounds
 */
MTP::Album* MtpDevice::Album(count_t idx) const
{
  if (idx > _albums.size() )
    return NULL;
  return _albums[idx];
}

/**
 * @return the Playlist at the given index or NULL if it is out of bounds
 */
MTP::Playlist* MtpDevice::Playlist(count_t idx) const
{
  if (idx > _playlists.size() )
    return NULL;
  return _playlists[idx];
}

/**
 * Retrieves the object to the specificed path
 * @param in_id the item id of the requested Mtp object
 * @param path the path to retreive to
 * @return true if the fetch was successfull or false otherwise
 */
bool MtpDevice::Fetch(uint32_t in_id, char const * const path)
{
  if (!_device)
    return false;
//TODO get error log
//What does get error log mean?
  int ret= LIBMTP_Get_File_To_File(_device, in_id, path,
                                   _progressFunc, _progressData);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  return true;
}

/**
 * @return the name of the device as a UTF8 string
 */
const char* MtpDevice::Name() const
{
  return _name;
}

/**
 * @return Returns the device serial number as a UTF8 string
 * This is mostly used for quickly connecting to a default device
 */
const char* MtpDevice::SerialNumber() const
{
  return _serialNumber;
}

/**
 * @return the version of the device as a UTF8 string
 */
const char* MtpDevice::Version() const
{
  return _version;
}

/**
 * @return the sync partner of the device as a UTF8 string
 */
const char* MtpDevice::SyncPartner() const
{
  return _syncPartner;
}

/**
 * @return the model name of the device as a UTF8 string
 */
const char* MtpDevice::ModelName() const
{
  return _modelName;
}

/**
 * @return whether retreiveing the battery level failed
 */
bool MtpDevice::BatteryLevelSupport() const
{
  return _batteryLevelSupport;
}

/**
 * @return the percentage of charge the battery holds
 */
float MtpDevice::BatteryLevel() const
{
  float ret = (float) ((float)_maxBatteryLevel / (float)_currentBatteryLevel);
  return ret;
}

/**
 * @return the number of storage devices associated with this device
 */
unsigned int MtpDevice::StorageDeviceCount() const
{
  return _storageDeviceList.size();
}
/**
 * @return the faked root folder for this device
 */
MTP::Folder* MtpDevice::RootFolder() const
{
  return _rootFolder;
}

/**
 * @param in_idx the index of storage device
 * @return the requested storage device
 */
MtpStorage* MtpDevice::StorageDevice(unsigned int in_idx) const
{
  if (in_idx > _storageDeviceList.size())
    return NULL;
  else
    return _storageDeviceList[in_idx];
}

/**
 * Gets the device's error stack and runs over each error to store its text
 * internally and then clears the device's error stack
 */
void MtpDevice::processErrorStack()
{
  if (!_device)
    return;
  LIBMTP_error_t* es = LIBMTP_Get_Errorstack(_device);
  while (es)
  {
    string s(es->error_text);
    _errorStack.push_back(s);
    es = es->next;
    cout << "Qlix MTP ERROR: " << s << endl;
  }

  LIBMTP_Clear_Errorstack(_device);
}

/**
 * Sets the callback progress function for operations with the MTP device
 * @param in_func the callback function to invoke during MTP operations
 * @param in_data a pointer to an object that will be used to update the
 *        of the GUI
 */
void MtpDevice::SetProgressFunction(LIBMTP_progressfunc_t in_func,
                                    const void* const in_data)
{
  _progressData = in_data;
  _progressFunc = in_func;
}

/**
 * Retrieves Tracks, Files and Albums and Folders form the devices and creates
 * a tree, storing each object's ID in its respective map
 */
void MtpDevice::createObjectStructure()
{
  if (!_device)
    return;
  //create container structures first..
  createFolderStructure(NULL, 0);
  createFileStructure();
  createTrackBasedStructures();

  if (_commandLineOpts.DebugOutput)
  {
    dbgPrintSupportedFileTypes();
    dbgPrintFolders(NULL, 0);
  }
}

/*
 * Returns the raw device that this object abstracts over
 * @return the raw device
 */
LIBMTP_mtpdevice_t* MtpDevice::RawDevice() const
{
  return _device;
}
/**
 * This function adds an album to the virtual album list
 * It is mostly supposed to be used by async calls from the GUI when it is
 * ready for such a fundamental change to occur in the structure.
 */
void MtpDevice::AddAlbum(MTP::Album* in)
{
  in->SetRowIndex(_albums.size());
  _albums.push_back(in);
}

/**
 * Recursively builds the folder structure
 * @param in_root the root folder on the device
 * @param in_depth the current depth of folder creation. It is also the current
 *        depth of recursion. If the current level of recursion is 0 we will
 *        retrieve a fresh folder list
 */
void MtpDevice::createFolderStructure(MTP::Folder* in_root, count_t in_depth)
{
  if (!_device)
    return;
  vector<MTP::Folder*> curLevelFolders;
  LIBMTP_folder_t* rootFolder;
  if (in_depth == 0)
  {
     assert(!in_root);
     rootFolder= LIBMTP_Get_Folder_List(_device);
     LIBMTP_folder_t* fakeRoot = LIBMTP_new_folder_t();
     fakeRoot->folder_id = 0;
     fakeRoot->parent_id = 0;
     fakeRoot->name = strdup(_name);
     fakeRoot->sibling= NULL;
     fakeRoot->child = rootFolder;
     rootFolder = fakeRoot;
  }
  else
    rootFolder = in_root->RawFolder()->child;

  while (rootFolder)
  {
    //if there is a parent, set the new folder's parent. And add to the
    //parent's childlist
    MTP::Folder* currentFolder;
    if(in_root)
    {
      currentFolder =  new MTP::Folder(rootFolder, in_root, in_depth);
      currentFolder->SetRowIndex(in_root->FolderCount());
      in_root->AddChildFolder(currentFolder);
    }
    else //else set the child's parent to NULL indicating its at the root
    {
      assert(_rootFolder ==NULL && in_depth == 0);
      currentFolder =  new MTP::Folder(rootFolder, NULL, in_depth);
      //set the row index first as it is zero based
      currentFolder->SetRowIndex(0);
      //set the root folder
      _rootFolder = currentFolder;
    }

    //add this folder to the list of folders at this level
    curLevelFolders.push_back(currentFolder);

    //find
    MTP::Folder* const previous = (MTP::Folder*) (find(currentFolder->ID(),
                                                     MtpFolder));
    if(previous)
    {
      cerr << "Folder crosslinked with another folder please file a bug report at: "
           << "caffein@gmail.com" << endl;
      cerr << " Previous folder's name: " << previous->Name() << endl;
      cerr << " New file's name: " << currentFolder->Name() << endl;
      assert(false);

    }

    _folderMap[currentFolder->ID()] = currentFolder;

    rootFolder = rootFolder->sibling;
  }
  for (count_t i =0; i < curLevelFolders.size(); i++)
      createFolderStructure(curLevelFolders[i], in_depth+1);
}

/**
 * Prints the file types supported by this device
 */
void MtpDevice::dbgPrintSupportedFileTypes()
{
  cout << "Supported file types: ";
  if (_supportedFileTypes.size() == 0)
    cout << "none!";
  cout << endl;

  for (count_t i =0; i < _supportedFileTypes.size(); i++)
    cout << _supportedFileTypes[i] << endl;
}

/**
 * Recursively prints the folders discovered
 * @param root the current level's root folder
 * @param level the current depth of the traversal used for ASCII alignment
 */
void MtpDevice::dbgPrintFolders(MTP::Folder* root, count_t level)
{
  if (root == NULL)
  {
    for (count_t i =0; i < _rootFolders.size(); i++)
    {
      cout << _rootFolders[i]->Name() << endl;
      dbgPrintFolders(_rootFolders[i], 1);
    }
    return;
  }

  for (count_t i = 0; i < root->FolderCount(); i++)
  {
    for (count_t j = 0; j < level; j++)
      cout << "  ";
    MTP::Folder* temp = root->ChildFolder(i);
    cout << temp->Name() << endl;
    dbgPrintFolders(temp, level+1);
  }
}

/**
 * Get the file list and iterate over and add them to their parent folder
 * This function must be called after createFolderStructure
 */
void MtpDevice::createFileStructure()
{
  LIBMTP_file_t* fileRoot = LIBMTP_Get_Filelisting_With_Callback(_device, NULL, NULL);
  while (fileRoot)
  {
    MTP::Folder* const parentFolder =
      (MTP::Folder*) find(fileRoot->parent_id, MtpFolder);

    //Get the parent folder's depth
    count_t fileDepth = (parentFolder ? parentFolder->Depth() + 1 : 0);
    MTP::File* currentFile = new MTP::File(fileRoot, fileDepth);

    if (_commandLineOpts.DebugOutput)
      cout << "Created file: " << currentFile << " with id: "<< currentFile->ID() << endl;
    //Sanity check: find previous instances for crosslinks
    MTP::File* prev_file = (MTP::File*) find(currentFile->ID(), MtpFile);
    //crosslink check
    if(prev_file)
    {
      cerr << "File crosslinked with another file please file a bug report at: "
           << "caffein@gmail.com" << endl;
      cerr << " Previous file's name: " << prev_file->Name() << endl;
      cerr << " New file's name: " << currentFile->Name() << endl;
      assert(false);
    }
    _fileMap[currentFile->ID()] = currentFile;

    if(! parentFolder)
    {
      cerr << "Database corruption. Parent folder for file:" << currentFile->ID()
           << " which has ID: " << currentFile->ParentID()
           << " was not discovered" << endl;
      assert(false);
    }
    currentFile->SetRowIndex(parentFolder->FileCount());
    parentFolder->AddChildFile(currentFile);
    if (_commandLineOpts.DebugOutput)
    {
      if (parentFolder->ID() == 0)
       cout << "Added new child folder to root, now file count: " << parentFolder->FileCount() << endl;
    }
    currentFile->SetParentFolder(parentFolder);

    //move on to the next file
    fileRoot = fileRoot->next;
  }
}

/**
 * Retreives the album and track list then Iterates over all the Albums and
 * their tracks. Each track is looked up in the object map and added to the
 * Album's list of child tracks
 */
void MtpDevice::createTrackBasedStructures()
{
  LIBMTP_album_t* albumRoot= LIBMTP_Get_Album_List(_device);
  LIBMTP_track_t* trackRoot = LIBMTP_Get_Tracklisting_With_Callback(_device, NULL, NULL);
  LIBMTP_playlist_t* playlistRoot = LIBMTP_Get_Playlist_List(_device);

  //create the wrapper tracks
  while (trackRoot)
  {
    MTP::Track* currentTrack = new MTP::Track(trackRoot);
    if (_commandLineOpts.DebugOutput)
    {
      cout <<"Creating track: " << currentTrack << endl;
      cout << "Inserting track with ID: " << currentTrack->ID() << endl;
    }

    MTP::Track * prev_track =
                             (MTP::Track*) find(currentTrack->ID(), MtpTrack);

    MTP::File * previousFile =
                                (MTP::File*) find(currentTrack->ID(), MtpFile);

    //ensure that there exists a file association
    if(!previousFile || previousFile->ID() != currentTrack->ID())
    {
      cerr << "Qlix internal database corruption! please report this to: " <<
              "caffein@gmail.com as this is a fairly serious error!" << endl;
      assert(false);
    }

    //crosslink check with previous tracks
    if (prev_track)
    {
      cerr << "Track crosslinked with another track! Please report this to"
           << "caffein@gmail.com" << endl;
      cerr << "Previous tracks's name: " << prev_track->Name() << endl;
      cerr << "New track's name: " << currentTrack->Name() << endl;
      assert(false);
    }

    //now crossreference the track with the file and the file with the track
    previousFile->Associate(currentTrack);
    currentTrack->Associate(previousFile);

    if (_commandLineOpts.DebugOutput)
    {
      cout <<"Track:" << currentTrack << " associated with file: " << previousFile<< endl;
      cout <<"File:" << previousFile << " associated with Track: " << currentTrack <<endl;
    }
    MTP::GenericObject* previousTrack = _trackMap[currentTrack->ID()];
    if (previousTrack)
    {
      cerr << "Track crosslinked with another track! please report this to "
           <<" caffein@gmail.com"<< endl;
      assert(false);
    }
    _trackMap[currentTrack->ID()] = currentTrack;
    trackRoot = trackRoot->next;
  }

  //create the wrapper albums and add its child tracks
  while (albumRoot)
  {
    LIBMTP_filesampledata_t temp;
    LIBMTP_Get_Representative_Sample(_device, albumRoot->album_id, &temp);
    if (_commandLineOpts.DebugOutput)
    {
      cout << "Discovered a sample of type: " << temp.filetype
           << " with height: " << temp.height << " and width: "
           << temp.width << " with size: " << temp.size << endl;
    }

    MTP::Album* currentAlbum = new MTP::Album(albumRoot, temp);
    currentAlbum->SetRowIndex( _albums.size());
    _albums.push_back(currentAlbum);

    MTP::Album * const prev_album =
                             (MTP::Album*) find(currentAlbum->ID(), MtpAlbum);
    MTP::File * const file_association =
                               (MTP::File*) find(currentAlbum->ID(), MtpFile);

    //crosslink check with file database
    if(!file_association)
    {
      cerr << "Album not crosslinked with file as expected!" <<
               " Please report this to caffein@gmail.com" << endl;
      cerr << "New album's name: " << currentAlbum->Name() << endl;
      assert(false);
    }

    //crosslink check with albums
    if (prev_album)
    {
      cerr << "Album crosslinked with another album! Please report this to"
           << "caffein@gmail.com" << endl;
      cerr << "Previous album's name: " << prev_album->Name() << endl;
      cerr << "New album's name: " << currentAlbum->Name() << endl;
      assert(false);
    }

    //now crossreference the album with the file and the file with the album
    file_association->Associate(currentAlbum);
    currentAlbum->Associate(file_association);

    _albumMap[currentAlbum->ID()] = currentAlbum;


    //now iterate over the albums's children..
    for (count_t j = 0; j < currentAlbum->TrackCount(); j++)
    {
      uint32_t track_id = currentAlbum->ChildTrackID(j);
      //sanity check
      //This is not a const * const as it will be modified once its added
      MTP::Track * const track = (MTP::Track*) find(track_id, MtpTrack);
      if(!track)
      {

        cerr << "Current track: " << track_id << " is associated with an album"
        << " but it does not exist on the device." << endl
        << " This usually happens when faulty MTP programs delete files" << endl
        << " from the device without deleting the associated track object"<< endl
        << " or a file was created and added to a playlist without" << endl
        << " creating a track association." << endl
        << "Please report this to caffein@gmail.com" << endl << endl
        << "Please backup your music and run Qlix with the"
        << " --FixBadAlbums option." << endl << endl;
        if (!_commandLineOpts.AutoFixAlbums)
          assert(false);
        else
        {
          cout << "Album: " << currentAlbum->Name() << " " << currentAlbum->ID()
               << " has a non-existant track association. Autofixing.." << endl;
          currentAlbum->RemoveFromRawAlbum(j);

          /**
           * If we simulate transfers then we do not make any calls that might
           * update the underlying device
           **/
          if (!_commandLineOpts.SimulateTransfers)
          {
            int ret = LIBMTP_Update_Album(_device, currentAlbum->RawAlbum());
            if (ret != 0)
            {
              processErrorStack();
              cout << "Failed to auto correct playlist." << endl;
            }
          }
          continue;
        }

      }

      //AddTrack will set the track's (album) row index as well
      currentAlbum->AddTrack(track);
    }

    currentAlbum->SetInitialized();
    albumRoot = albumRoot->next;
  }

  while(playlistRoot)
  {
    MTP::Playlist* currentPlaylist = new MTP::Playlist(playlistRoot);
    //set the row index first, as it is zero based
    currentPlaylist->SetRowIndex(_playlists.size());
    _playlists.push_back(currentPlaylist);

    MTP::Playlist* prev_playlist= (MTP::Playlist*) find(currentPlaylist->ID(), MtpPlaylist);
    MTP::File* file_association= (MTP::File*) find(currentPlaylist->ID(), MtpFile);

    //crosslink check with file database
    if (!file_association)
    {
       cerr << "Playlist not crosslinked with file as expected!" <<
               " Please report this to caffein@gmail.com" << endl;
       assert(false);
    }

    //crosslink check with albums
    if (prev_playlist)
    {
      cerr << "Playlist crosslinked with another playlist! Please report this "
           << "to caffein@gmail.com" << endl;
      cerr << "Previous playlist name: " << prev_playlist->Name() << endl;
      cerr << "Previous playlist ID: " << prev_playlist->ID() << endl;

      cerr << "Current playlist name: " << currentPlaylist->Name() << endl;
      cerr << "Current playlist ID: " << currentPlaylist->ID() << endl;
      assert(false);
    }

    //now crossreference the playlist with the file
    //and crossreference the file with the playlist
    currentPlaylist->Associate(file_association);
    file_association->Associate(currentPlaylist);

    _playlistMap[currentPlaylist->ID()] = currentPlaylist;

    //now iterate over the playlist's children..
    for (count_t j = 0; j < currentPlaylist->TrackCount(); j++)
    {
      uint32_t track_id = currentPlaylist->ChildTrackID(j);
      //sanity check
      MTP::Track* const track = (MTP::Track*) find(track_id, MtpTrack);

      if (!track)
      {
        cerr << "Current track: " << track_id << " belongs to a playlist but"
             << " does not exist on the device." << endl
             << " This usually happens when faulty MTP programs delete files" << endl
             << " from the device without deleting the associated track object"<< endl
             << " or a file was created and added to a playlist without" << endl
             << " creating a track association." << endl
             << "Please report this to caffein@gmail.com" << endl << endl
             << "Please backup your music and run Qlix with the"
             << " --FixBadPlaylists option." << endl << endl;
        if (!_commandLineOpts.AutoFixPlaylists)
          assert(false);
        else
        {
          cout << "Playlist: " << currentPlaylist->Name()  << " "
          << currentPlaylist->ID() << " has non-existant track. Autofixing.."
          << endl;

          currentPlaylist->RemoveFromRawPlaylist(j);
          if (_commandLineOpts.SimulateTransfers)
            continue;

          int ret = LIBMTP_Update_Playlist(_device,
        		  const_cast<LIBMTP_playlist_t*> (currentPlaylist->RawPlaylist()));
          if (ret != 0)
          {
            processErrorStack();
            cout << "Failed to auto correct playlist." << endl;
          }
          continue;
        }
      }

      //AddTrack will set the track's (playlist) row index as well
      currentPlaylist->AddTrack( (MTP::Track*) track );
    }
    currentPlaylist->SetInitialized();

    playlistRoot = playlistRoot->next;
  }
}

/**
 *  Transfers a track to the device
 *  @param in_path the path of the file to transfer
 *  @param in_track the track metadata for this file
 *  @return true if succesfull, false otherwise
 */
bool MtpDevice::TransferTrack(const char* in_path, MTP::Track* in_track)
{
  if (_commandLineOpts.DebugOutput)
    cout << "Track's storage id: " << in_track->RawTrack()->storage_id << endl;
  if (! _commandLineOpts.SimulateTransfers)
  {
    int ret = LIBMTP_Send_Track_From_File(_device, in_path,
                                          in_track->RawTrack(),
                                          _progressFunc, _progressData);
    if (ret != 0)
    {
      processErrorStack();
      return false;
    }
  }

  //necessary due to composition
  //TODO brainstorm portential fixes
  // ^ fix what precisely?
  in_track->SetID(in_track->RawTrack()->item_id);
  if (_commandLineOpts.DebugOutput)
    cout << "Transfer succesfull, new id: " << in_track->ID() << endl;
  UpdateSpaceInformation();
  return true;
}

/**
 * Transfers a file to the device
 * @param in_path the path of the file to transfer
 * @param in_file the metadata for the file
 * @return true if succesfull, false otherwise
 */
bool MtpDevice::TransferFile(const char* in_path, MTP::File* in_file)
{
  int ret = LIBMTP_Send_File_From_File(_device, in_path, in_file->RawFile(),
                                        _progressFunc, _progressData);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  UpdateSpaceInformation();
  return true;
}

/**
 * This function updates the space usage information, it should be called at
 * every function call that could potentially change the space usage of the
 * device
 * @return true if successfully retreived, false otherwise
 */
bool MtpDevice::UpdateSpaceInformation()
{
  if (!_device)
    return false;
  if (!_initialized)
    assert(_storageDeviceList.size() == 0);

  int ret = LIBMTP_Get_Storage(_device, LIBMTP_STORAGE_SORTBY_NOTSORTED);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  for (count_t i =0; i < _storageDeviceList.size(); i++)
  {
    delete _storageDeviceList[i];
  }
  _storageDeviceList.clear();

  LIBMTP_devicestorage_t* storage_dev = _device->storage;
  while (storage_dev)
  {
    MtpStorage* new_storage = new MtpStorage(storage_dev);
    _storageDeviceList.push_back(new_storage);
    storage_dev = storage_dev->next;
  }
  return true;
}

/**
 * This function creates a new album on the device, using the information from
 * the track that is passed as a param. This function assumes that this
 * album's name is unique on the device.
 * The new album will be added to the album's list, be sure to notify any
 * models before hand
 *
 * @param in_track the track that is used as a template for the album's name,
 *        artist, and genre fields.
 * @param in_storageID the storage id to create the album on
 * @param out_album the newly allocated MTP::Album, if the operation fails
 *        this value is NULL
 * @return true if successful false otherwise
 */
bool MtpDevice::NewAlbum(MTP::Track* in_track, int in_storageID,
                         MTP::Album** out_album)
{
  LIBMTP_album_t* newAlbum = LIBMTP_new_album_t();
  newAlbum->name = strdup(in_track->AlbumName());
  newAlbum->artist = strdup(in_track->ArtistName());
  newAlbum->genre = strdup(in_track->Genre());
  newAlbum->tracks  = NULL;
  newAlbum->parent_id = 0;
  newAlbum->storage_id = in_storageID;

  if (_commandLineOpts.DebugOutput)
  {
    cerr << "Created new album with name: " << newAlbum->name << endl;
    cerr << "Created new album with artist: " << newAlbum->artist << endl;
    cerr << "Created new album with genre: " << newAlbum->genre << endl;
  }
//  *(newAlbum->tracks) = in_track->ID();
//  cout << "Set the album's first track to: " << *(newAlbum->tracks) << endl;
  newAlbum->no_tracks = 0;
  newAlbum->next = NULL;

  if (!_commandLineOpts.SimulateTransfers)
  {
    int ret =  LIBMTP_Create_New_Album(_device, newAlbum);
    if (ret != 0)
    {
      (*out_album) = NULL;
      processErrorStack();
      return false;
    }
  }

  UpdateSpaceInformation();
  LIBMTP_filesampledata_t sample;
  sample.size = 0;
  sample.data = NULL;
  (*out_album) = new MTP::Album(newAlbum, sample);
  return true;
}

/**
 * This function updates the representative sample of album on the device
 * @param in_album the album whose art should be updated
 * @param in_sample the album art encased in a LIBMTP_filesampledata_t* struct
 * @return true if the operation succeeded, false otherwise
 */
bool MtpDevice::UpdateAlbumArt(MTP::Album* in_album,
                               LIBMTP_filesampledata_t* in_sample)
{
  int ret = LIBMTP_Send_Representative_Sample(_device,
                                              in_album->ID(), in_sample);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  in_album->SetCover(in_sample);
  return true;
}

/**
 * This function retreives the default JPEG sample parameters from the device
 * This function assumes that libmtp is initialized
 * @return a preallocated LIBMTP_filesampledata_t* with sane values it is up to
 *         the caller to call LIBMTP_delete_object to free up the memory
 */
//TODO is this safe when mtp is not initialized?
LIBMTP_filesampledata_t* MtpDevice::DefaultJPEGSample()
{
  if (!_device)
    return NULL;
  LIBMTP_filesampledata_t* sample;
  int ret = LIBMTP_Get_Representative_Sample_Format(_device,
                                                    LIBMTP_FILETYPE_JPEG,
                                                    &sample);
  if (ret != 0)
  {
    processErrorStack();
    return NULL;
  }
  /*
  //device does not support JPEG samples..
  if (ret == 0 && sample == NULL)
  {
    cerr << "Device does not support JPEG (proper) file type, checking for"
    <<" JPEG 2000 regular" << endl;
    ret = LIBMTP_Get_Representative_Sample_Format(_device,
                                                         LIBMTP_FILETYPE_JP2,
                                                         &sample);
    if (ret != 0)
    {
      processErrorStack();
      return NULL;
    }
  }

  //device does not support JPEG 2000 samples..
  if (ret == 0 && sample == NULL)
  {
    cerr << "Device does not support JPEG 2000 regular file type, checking"
    " for JPEG 2000 extended" << endl;
    ret = LIBMTP_Get_Representative_Sample_Format(_device,
                                                         LIBMTP_FILETYPE_JPX,
                                                         &sample);
    if (ret != 0)
    {
      processErrorStack();
      return NULL;
    }
  }
  */

  if(ret !=0)
    cerr << "Problem getting JPEG information from device" << endl;
  else if (ret == 0 && sample == NULL)
    cerr << "Device does not support JPEG filetype" << endl;
  else if (ret == 0 && sample != NULL)
    cerr << "Device supports JPEG filetype" << endl;

  return sample;
}

/**
 * This function adds the passed track to the album on the device by first
 * adding it to the raw track, syncing to the device and then adds the track
 * to the wrapper track.
 * @param in_track the track that is to be added to the parent album
 * @param in_album the parent album for this track
 * @return true if successfull false otherwise
 */
bool MtpDevice::AddTrackToAlbum(MTP::Track* in_track, MTP::Album* in_album)
{
  in_album->SetInitialized();
  in_album->AddTrackToRawAlbum(in_track);
  //if we are simulating then we return true
  if (_commandLineOpts.SimulateTransfers)
    return true;

  //otherwise we update the album on the device accordingly
  int ret = LIBMTP_Update_Album(_device, in_album->RawAlbum());
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  return true;
}

/**
 * Removes a track from an associated playlist
 * @param in_idx The track index to be removed
 * @param in_pl The container playlist that will reflect the removal
 */
bool MtpDevice::RemoveShadowTrack(MTP::ShadowTrack* in_track)
{
  MTP::Playlist* pl = in_track->ParentPlaylist();
  pl->RemoveFromRawPlaylist(in_track->RowIndex());

  if (_commandLineOpts.SimulateTransfers)
    return true;

  int ret = LIBMTP_Update_Playlist(_device,
						  const_cast<LIBMTP_playlist_t*> (pl->RawPlaylist()));
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  return true;
}


/**
 * Removes a file from the device by first removing the file from the raw
 * parent structure. This structure is *not* sent to the device as an update
 * as files are handled differently than playlists and albums.
 * The parent structure is assumed to exist (all files must be contained by a
 * folder)
 * @param in_file the file to remove
 */
bool MtpDevice::RemoveFile(MTP::File* in_file)
{
  assert(in_file);
  assert(in_file->ParentFolder());

  if (_commandLineOpts.SimulateTransfers)
    return true;
  /**
  * There should be no need to do this since there is no metadata to update on
  * the device.
  * MTP::Folder* parentFolder = in_file->ParentFolder();
  * parentFolder->RemoveFileFromRawFolder(in_file->GetRowIndex());
  */
  bool ret = removeObject(in_file->ID());
  return ret;
}

/**
 * Removes a folder from the device by first removing the folder from the
 * raw parent structure. This sturcture is *not* sent to the device as an update
 * as folders are handled in similar fashion to files and as such differ from
 * other containers such as albums and playlists.
 * If the parent structure doesn't exist then we ignore the request as we
 * cannot delete the root folder
 * @param in_track the track to remove
 */
bool MtpDevice::RemoveFolder(MTP::Folder* in_folder)
{
  assert(in_folder);
  //Lets make sure that this folder is empty
  assert(in_folder->FileCount() == 0);
  assert(in_folder->FolderCount() == 0);
  //for some reason I think that the root folder is its own parent..
  if(in_folder == in_folder->ParentFolder())
  {
    cout << "Debug this immediately! "<< endl;
    assert(false);
  }

  if (_commandLineOpts.SimulateTransfers)
    return true;
 /**
  *  There should be no need to do this since there is no metadata to update on
  *   the device.
  MTP::Folder* parentFolder = in_folder->ParentFolder();
  if (parentFolder != NULL)
    parentFolder->RemoveFolderFromRawFolder(in_folder->GetRowIndex());
  */

  bool ret = removeObject(in_folder->ID());
  return ret;
}
/**
 * Removes a track from the device and the raw album object that is its parent
 * @param in_track the track to remove
 */
bool MtpDevice::RemoveTrack(MTP::Track* in_track)
{
  assert(in_track);
  MTP::Album* parentAlbum = in_track->ParentAlbum();

  //Q. Why do we do this?
  //A. Quick answer because if the album is not initialized we goto the
  //   raw MTP object for information.
  //Added assertion to see if the initialization check really is necessary
  //preliminary research shows that its not necessary as folders don't seem
  //to need it but this could be an oversight on my part
  //TODO resolve this
  if (parentAlbum)
  {
    if (!parentAlbum->Initialized())
      assert(false);
	  parentAlbum->SetInitialized();
	  //End of testing check the following is necessary
	  parentAlbum->RemoveFromRawAlbum(in_track->GetRowIndex());
  }

  if (_commandLineOpts.SimulateTransfers)
    return true;

  // we report overall failure for any MTP transaction
  bool emminentFailure = false;
  int ret = LIBMTP_Update_Album(_device, parentAlbum->RawAlbum());
  if (ret != 0)
  {
    processErrorStack();
    emminentFailure = true;
  }
  for (count_t i = 0; i < in_track->ShadowAssociationCount(); i++)
  {
    MTP::ShadowTrack* strack = in_track->ShadowAssociation(i);
    if (strack == NULL)
      continue;
    MTP::Playlist* parentPl = strack->ParentPlaylist();
    parentPl->RemoveFromRawPlaylist(strack->RowIndex());
    ret = LIBMTP_Update_Playlist(_device,
    		const_cast<LIBMTP_playlist_t*> (parentPl->RawPlaylist()));
    if (ret != 0 )
    {
      processErrorStack();
      emminentFailure = true;
    }
  }

  ret = removeObject(in_track->ID());
  if (ret != 0 )
  {
      processErrorStack();
      emminentFailure = true;
  }
  return emminentFailure;
}

/**
 * Removes an album from the device
 * @param in_album the album to remove
 */
bool MtpDevice::RemoveAlbum(MTP::Album* in_album)
{
  assert(in_album);
  bool ret = removeObject(in_album->ID());
  return ret;
}

/**
 * Removes a playlist from the device
 * @param in_pl the playlist to remove
 */
bool MtpDevice::RemovePlaylist(MTP::Playlist* in_pl)
{
  assert(in_pl);
  bool ret = removeObject(in_pl->ID());
  return ret;
}


/**
 * Private function that performs a search on a specific object mapping
 * Since MTP defines a protocol that crosslinks files with tracks, playlists, albums.
 * @param in_id the id of the requested crosslinked object
 * @param in_type the type of the requested crosslinked object
 * @return the requested associated object, or NULL for invalid types or
 * inconclusive searches
 */
MTP::GenericObject * MtpDevice::find(count_t in_id, MtpObjectType in_type) const
{
  switch(in_type)
  {
    case MtpTrack:
    {
      TrackMap::const_iterator iter = _trackMap.find(in_id);
      TrackMap::const_iterator iterEnd = _trackMap.end();
      if(iterEnd == iter)
        return NULL;
      else
        return iter->second;

    }
    case MtpFile:
    {
      FileMap::const_iterator iter = _fileMap.find(in_id);
      FileMap::const_iterator iterEnd = _fileMap.end();
      if(iterEnd == iter)
        return NULL;
      else
        return iter->second;
    }
    case MtpFolder:
    {
      FolderMap::const_iterator iter = _folderMap.find(in_id);
      FolderMap::const_iterator iterEnd = _folderMap.end();
      if(iterEnd == iter)
        return NULL;
      else
        return iter->second;
    }
    case MtpAlbum:
    {
      AlbumMap::const_iterator iter = _albumMap.find(in_id);
      AlbumMap::const_iterator iterEnd = _albumMap.end();
      if(iterEnd == iter)
        return NULL;
      else
        return iter->second;
    }
    case MtpPlaylist:
    {
      PlaylistMap::const_iterator iter = _playlistMap.find(in_id);
      PlaylistMap::const_iterator iterEnd = _playlistMap.end();
      if(iterEnd == iter)
        return NULL;
      else
        return iter->second;
    }
    default:
      return NULL;
  }
}

/**
 * Creates a new folder on the device, and should update its object id
 * @param in_folder the folder to be created on the device
 */
bool MtpDevice::NewFolder(MTP::Folder* in_folder)
{
  cout << "New Folder stub!" << endl;
  return true;
}

/**
 * Generic function to remove an object from the device
 * @param in_id the id of the object to be removed
 */
bool MtpDevice::removeObject(count_t in_id)
{
  if (_commandLineOpts.SimulateTransfers)
    return true;

  bool ret = LIBMTP_Delete_Object(_device, in_id);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  UpdateSpaceInformation();
  return true;
}
