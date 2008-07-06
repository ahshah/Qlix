//TODO Improve error handling / Reporting (once theres an error console)
//TODO Add InsanePlaylist, InsaneAlbums flags that allows us to avoid the 
//     assertions made WRT necessary MTP file associations with all other types
//TODO Should raw object references returns be of const types? 
//     No this is a bad idea as sending files updates the file id we discover
//TODO Storage IDs are not correctly handled- recheck this- it should be fixed
//TODO Add autocleanup of broken playlists and albums

#include "MtpDevice.h"
#undef QLIX_DEBUG
//#define SIMULATE_TRANSFERS

/**
 * Creates a MtpDevice
 * @param in_device The raw LIBMtp device
 * @return Returns the requested device
 */
MtpDevice::MtpDevice(LIBMTP_mtpdevice_t* in_device)  :
                                                _initialized(false)
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

  ClearObjectMappings();
}

/**
 * Initializes the MtpDevice and creates an internal tree structure for all
 * MTP objects
 */
void MtpDevice::Initialize()
{
  if (!_device)
    return;

  assert(_initialized== false);
  _initialized = true;
//  _progressFunc= NULL;
   _name = LIBMTP_Get_Friendlyname(_device);

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
      cout << "Storage reporting: " << storage_dev->TotalSpace() << " and " <<
               storage_dev->FreeSpace() << endl;;
      *out_total = storage_dev->TotalSpace();
      *out_free = storage_dev->FreeSpace();
      return;
    }
  }
  *out_total = 0;
  *out_free = 0;
}

/** 
 * @param in_id the file id associated with the MTP::File
 * @return the MTP::File associated with the particular ID or NULL if not found
 */
MTP::File* const MtpDevice::FindFile(count_t in_id) const
{
  return (MTP::File*) find(in_id, MtpFile);
}

/** 
 * @param in_id the folder ID associated with the MTP::Folder
 * @return the MTP::Folder associated with the particular ID or NULL if not found
 */
MTP::Folder* const MtpDevice::FindFolder(count_t in_id) const
{
  return (MTP::Folder*) find(in_id, MtpFolder);
}

/** 
 * @param in_id the album ID associated with the MTP::Album
 * @return the MTP::Album associated with the particular ID or NULL if not found
 */
MTP::Album* const MtpDevice::FindAlbum(count_t in_id) const
{
  return (MTP::Album*) find(in_id, MtpAlbum);
}


/** 
 * @param in_id the playlist ID associated with the MTP::Playlist
 * @return the MTP::Playlist associated with the particular ID or NULL if not found
 */
MTP::Playlist* const MtpDevice::FindPlaylist(count_t in_id) const
{
  return (MTP::Playlist*) find(in_id, MtpPlaylist);
}

/** 
 * @param in_id the track ID associated with the MTP::Track
 * @return the MTP::Track associated with the particular ID or NULL if not found
 */
MTP::Track* const MtpDevice::FindTrack(count_t in_id) const
{
  return (MTP::Track*) find(in_id, MtpTrack);
}

/**
 * @return the number of folders found at the root level of the device
 */
count_t MtpDevice::RootFolderCount() const
{ return _rootFolders.size(); }

/**
 * @return the number of files found at the root level of the device
 */
count_t MtpDevice::RootFileCount() const
{ return _rootFiles.size(); }

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
 * @return the root File at the given index or NULL if it is out of bounds
 */
MTP::File* MtpDevice::RootFile(count_t idx) const
{ 
  if (idx > _rootFiles.size() )
    return NULL;
  return _rootFiles[idx];
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
char const * const MtpDevice::Name() const
{
  return _name;
}

/**
 * @return Returns the device serial number as a UTF8 string
 * This is mostly used for quickly connecting to a default device
 */
char const * const MtpDevice::SerialNumber() const
{
  return _serialNumber;
}

/**
 * @return the version of the device as a UTF8 string
 */
char const * const MtpDevice::Version() const
{
  return _version;
}

/**
 * @return the sync partner of the device as a UTF8 string
 */
char const * const MtpDevice::SyncPartner() const
{
  return _syncPartner;
}

/**
 * @return the model name of the device as a UTF8 string
 */
char const * const MtpDevice::ModelName() const
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
  createFolderStructure(NULL, true);
  for (int i =0; i < _rootFolders.size();i++ )
  {
    assert(_rootFolders[i]->GetRowIndex() == i);
  }

  createFileStructure();
  createTrackBasedStructures();

#ifdef QLIX_DEBUG
  //dbgPrintSupportedFileTypes();
  //dbgPrintFolders(NULL, 0);
#endif
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
 * @param firstRun whether to retreive a fresh list of folders from the device
 */
void MtpDevice::createFolderStructure(MTP::Folder* in_root, bool firstRun)
{
  if (!_device)
    return;
  vector<MTP::Folder*> curLevelFolders;
  LIBMTP_folder_t* folderRoot;
  if (!in_root && firstRun)
  {
     folderRoot= LIBMTP_Get_Folder_List(_device);
     LIBMTP_folder_t* fakeRoot = LIBMTP_new_folder_t();
     fakeRoot->folder_id = 0;
     fakeRoot->parent_id = 0;
     fakeRoot->name = new char[strlen(_name)];
     fakeRoot->name = strdup(_name);
     fakeRoot->sibling= NULL;
     fakeRoot->child = folderRoot;
     folderRoot = fakeRoot;
  }
  else
    folderRoot = in_root->RawFolder()->child;

  while (folderRoot)
  {
    //if there is a parent, set the new folder's parent. And add to the 
    //parent's childlist
    MTP::Folder* currentFolder;
    if(in_root)
    {
      currentFolder =  new MTP::Folder(folderRoot, in_root);
      currentFolder->SetRowIndex(in_root->FolderCount());
      in_root->AddChildFolder(currentFolder);
    }
    else //else set the child's parent to NULL indicating its at the root
    {
      currentFolder =  new MTP::Folder(folderRoot, NULL);
      //set the row index first as it is zero based
      currentFolder->SetRowIndex(_rootFolders.size());
      //add to the root level folder
      _rootFolders.push_back(currentFolder);
    }

    //add this folder to the list of folders at this level
    curLevelFolders.push_back(currentFolder);
    
    //find
    MTP::Folder* const previous = FindFolder(currentFolder->ID());
    if(previous)
    {
      cerr << "Folder crosslinked with another folder please file a bug report at: "
           << "caffein@gmail.com" << endl;
      cerr << " Previous folder's name: " << previous->Name() << endl;
      cerr << " New file's name: " << currentFolder->Name() << endl;
      assert(false);

    }

    _folderMap[currentFolder->ID()] = currentFolder; 

    folderRoot = folderRoot->sibling;
  }
  for (count_t i =0; i < curLevelFolders.size(); i++)
      createFolderStructure(curLevelFolders[i], false);
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
 * Frees all memory used by instantiated objects
 */
void MtpDevice::ClearObjectMappings()
{
  quickSanityCheck();
  for (count_t i = 0; i < _tracks.size(); i++)    { delete _tracks[i];  }
  for (count_t i = 0; i < _albums.size(); i++)    { delete _albums[i];  }
  for (count_t i = 0; i < _playlists.size(); i++) { delete _playlists[i];  }

  FileMap::iterator file_iter = _fileMap.begin();
  for (; file_iter != _fileMap.end(); file_iter++)   
  {
    if (file_iter->second) //deletions will set iter->second to NULL
     delete file_iter->second; 
  }

  FolderMap::iterator folder_iter = _folderMap.begin();
  for (; folder_iter != _folderMap.end(); folder_iter++)   
  {
    if (folder_iter->second) //deletions will set iter->second to NULL
     delete folder_iter->second; 
  }

  _rootFolders.clear();
  _rootFiles.clear();

  _tracks.clear();
  _trackMap.clear();

  _albums.clear();
  _albumMap.clear();

  _playlistMap.clear();
  _playlists.clear();
  

  _folderMap.clear();
  _fileMap.clear();
}

/**
 * Get the file list and iterate over and add them to their parent folder
 * This function must be called after createFolderStructure
 */
void MtpDevice::createFileStructure()
{
  LIBMTP_file_t* fileRoot = LIBMTP_Get_Filelisting(_device);
  while (fileRoot)
  {
    MTP::File* currentFile = new MTP::File(fileRoot);
    //Sanity check: find previous instances for crosslinks
    MTP::File* prev_file = FindFile(currentFile->ID());

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

    if (currentFile->ParentID() == 0)
    {
      currentFile->SetRowIndex(_rootFiles.size());
      _rootFiles.push_back(currentFile);
    }

    MTP::Folder* const parentFolder = FindFolder(currentFile->ParentID());
    if(! parentFolder)
    {
      cerr << "Database corruption. Parent folder for file:" << currentFile->ID()
           << " which has ID: " << currentFile->ParentID() 
           << " was not discovered" << endl;
      assert(false);
    }
    currentFile->SetRowIndex(parentFolder->FileCount());
    parentFolder->AddChildFile(currentFile);
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
  LIBMTP_track_t* trackRoot = LIBMTP_Get_Tracklisting(_device);
  LIBMTP_playlist_t* playlistRoot = LIBMTP_Get_Playlist_List(_device);

  //create the wrapper tracks
  while (trackRoot)
  {
    MTP::Track* currentTrack = new MTP::Track(trackRoot);
    _tracks.push_back(currentTrack);
//    cout << "Inserting track with ID: " << currentTrack->ID() << endl;
    MTP::GenericObject* previousFile = FindFile(currentTrack->ID());
    if(!previousFile || previousFile->ID() != currentTrack->ID())
    {
      cerr << "Qlix internal database corruption! please report this to: " <<
              "caffein@gmail.com as this is a fairly serious error!" << endl;
      assert(false);
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
#ifdef QLIX_DEBUG
    cout << "Discovered a sample of type: " << temp.filetype 
         << " with height: " << temp.height << " and width: "
         << temp.width << " with size: " << temp.size << endl;
#endif

    MTP::Album* currentAlbum = new MTP::Album(albumRoot, temp);
    currentAlbum->SetRowIndex( _albums.size());
    _albums.push_back(currentAlbum);

    MTP::Album* const prev_album = FindAlbum(currentAlbum->ID());
    MTP::File* file_association = FindFile(currentAlbum->ID());

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
    _albumMap[currentAlbum->ID()] = currentAlbum; 


    //now iterate over the albums's children..
    for (count_t j = 0; j < currentAlbum->TrackCount(); j++)
    {
      uint32_t track_id = currentAlbum->ChildTrackID(j);
      //sanity check
      MTP::Track* const track = FindTrack(track_id);
      if(!track)
      {
        cerr << "Current track: " << track_id << " does not exist.. skipping"
        << " this usually happens when MTP programs do not remove files " 
        << " correctly" << endl;
        //TODO add an autocorrect option here
        //report this to caffein@gmail.com" << endl;
        //assert(false);
        continue;
      }
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

    MTP::Playlist* prev_playlist= FindPlaylist(currentPlaylist->ID());
    MTP::File* file_association= FindFile(currentPlaylist->ID());
    
    //crosslink check with file database
    if (file_association)
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
      assert(false);
    }

    _playlistMap[currentPlaylist->ID()] = currentPlaylist; 

    //now iterate over the playlist's children..
    for (count_t j = 0; j < currentPlaylist->TrackCount(); j++)
    {
      uint32_t track_id = currentPlaylist->ChildTrackID(j);
      //sanity check
      MTP::Track* const track = FindTrack(track_id);

      if (!track)
      {
        cerr << "Current track: " << track_id << "belongs to a playlist but"
             << " does not exist on the device. This usually happens when"
             << " faulty MTP programs delete items from the device."
             << " Please report this to caffein@gmail.com" << endl;
        assert(false);
        //TODO add an autocorrect option
      }
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
  cout << "Track's storage id: " << in_track->RawTrack()->storage_id << endl;

#ifndef SIMULATE_TRANSFERS
  int ret = LIBMTP_Send_Track_From_File(_device, in_path, in_track->RawTrack(),
                                        _progressFunc, _progressData);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
#endif

  //necessary due to stupid inheritence
  //TODO fix this?
  in_track->SetID(in_track->RawTrack()->item_id);
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
 * @return true if successfull false otherwise
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

  cerr << "Created new album with name: " << newAlbum->name << endl;
  cerr << "Created new album with artist: " << newAlbum->artist << endl;
  cerr << "Created new album with genre: " << newAlbum->genre << endl;
//  *(newAlbum->tracks) = in_track->ID();
//  cout << "Set the album's first track to: " << *(newAlbum->tracks) << endl;
  newAlbum->no_tracks = 0;
  newAlbum->next = NULL;
#ifndef SIMULATE_TRANSFERS
  int ret =  LIBMTP_Create_New_Album(_device, newAlbum);
  if (ret != 0)
  {
    (*out_album) = NULL;
    processErrorStack();
    return false;
  }
#endif
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
#ifndef SIMULATE_TRANSFERS
  int ret = LIBMTP_Update_Album(_device, in_album->RawAlbum());
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
#endif
  return true;
}
/**
 * Removes a track from the device
 * @param in_track the track to remove
 */
bool MtpDevice::RemoveTrack(MTP::Track* in_track)
{
  assert(in_track);
  MTP::Album* parentAlbum = in_track->ParentAlbum();
  parentAlbum->SetInitialized();

  parentAlbum->RemoveFromRawAlbum(in_track->GetRowIndex());

#ifndef SIMULATE_TRANSFERS
  int ret = LIBMTP_Update_Album(_device, parentAlbum->RawAlbum());
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }

  ret = LIBMTP_Delete_Object(_device, in_track->ID());
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
#endif
  return true;
}

/** 
 * Removes an album from the device
 * @param in_album the album to remove
 */
bool MtpDevice::RemoveAlbum(MTP::Album* in_album)
{
  assert(in_album);

#ifndef SIMULATE_TRANSFERS
  bool ret = LIBMTP_Delete_Object(_device, in_album->ID());
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
#endif
  return true;
}

/**
 * Removes a folder from the device
 * @param in_folder the folder to remove
 */
bool MtpDevice::RemoveFolder(MTP::Folder* in_folder)
{
  assert(in_folder);

#ifndef SIMULATE_TRANSFERS
  bool ret = LIBMTP_Delete_Object(_device, in_folder->ID());
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
#endif
  return true;
}



/**
 * Private function that performs a search on a specific object mapping
 * Since MTP defines a protocol that crosslinks files with tracks, playlists, albums.
 * @param in_id the id of the requested crosslinked object
 * @param in_type the type of the requested crosslinked object
 * @return the requested associated object, or NULL for invalid types or 
 * inconclusive searches
 */
MTP::GenericObject * const MtpDevice::find(count_t in_id, MtpObjectType in_type) const
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
void MtpDevice::quickSanityCheck() const
{
  count_t mappedObjCount= 0;
  assert(_tracks.size() == _trackMap.size());
  assert(_albums.size() == _albumMap.size());
  assert(_playlists.size() == _playlistMap.size());
}

void MtpDevice::AddToRootFolder(MTP::Folder* in_folder)
{
  _rootFolders.push_back(in_folder);
}

void MtpDevice::RemoveFolderFromRoot(MTP::Folder* in_folder)
{
  vector<MTP::Folder*>::iterator iter = _rootFolders.begin();
  count_t i = 0;
  for (; iter != _rootFolders.end() && i < in_folder->GetRowIndex(); i++)
  {
    iter++;
  }
  if (i != in_folder->GetRowIndex())
  {
    cerr << "Deletion error: in_folder row index:" << in_folder->GetRowIndex() 
         << " Root file count: " << _rootFolders.size() << endl;
    assert(false);
  }

  _rootFolders.erase(iter);
  for (i =0; i < _rootFolders.size();i++ )
  {
    assert(_rootFolders[i]->GetRowIndex() == i);
  }
  return;
}

void MtpDevice::RemoveFileFromRoot(MTP::File* in_file)
{
  vector<MTP::File*>::iterator iter = _rootFiles.begin();
  count_t i = 0;
  for (; iter != _rootFiles.end() && i < in_file->GetRowIndex(); i++)
  {
    iter++;
  }
  if (i != in_file->GetRowIndex())
  {
    cerr << "Deletion error: in_file row index:" << in_file->GetRowIndex() 
         << " Root file count: " << _rootFiles.size() << endl;
    assert(false);
  }

  _rootFiles.erase(iter);
  return;
}
