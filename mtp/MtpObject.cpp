/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

TODO error checking when returning char* ?
TODO use multimap for track/file distinction
  RESOLVED by Association() parameter (I think)
TODO Figure out if we need to actually implement removeFromRawPlaylist/album/folder etc
  RESOLVED: Yes we do- we removeFromRawPlaylist() then update the device by reuploading
  the raw container
*/
#include "mtp/MtpObject.h"
using namespace MTP;
namespace MTP
{

/**
 * Creates a new GenericObject, a generic base class for MTP objects
 * @param in_type the MTP type of the new GenericObject to be created
 * @return a new GenericObject
 */
GenericObject::GenericObject(MtpObjectType in_type, uint32_t id) :
                             _type(in_type),
                             _id(id)
{ }

GenericObject::~GenericObject() {}

/**
 * @return the ID of this object
 */
count_t GenericObject::ID() const { return _id; }

/**
 * Sets the ID of this object
 */
void GenericObject::SetID( count_t in_id) { _id = in_id; }

/**
 * Returns the depth of the current object. Must be overriden by child classes
 * to provide meaningful behaviour
 */
//TODO why doesn't every object just have a depth field
//Potential answer: because we don't really need to know every object's depth.. yet
count_t GenericObject::Depth() const { return 0; }

/**
 * Simple function to get the type of the current MTP object
 * @return returns the type of the GenericObject
 */
MtpObjectType GenericObject::Type() const { return _type; }


/**
 * @return the name of this object
 */
const char* GenericObject::Name() const { return ""; }

/**
 * Creates a new GenericFileObject, a generic base class for File MTP objects
 * That are crosslinked with each other
 * @param in_type the MTP type of the new GenericObject to be created
 * @return a new GenericObject
 */
GenericFileObject::GenericFileObject(MtpObjectType in_type, uint32_t in_id) :
                                     GenericObject(in_type, in_id),
                                     _association(NULL)
{}

/**
 * Sets the objects file association, a file cannot be associated with another
 * file.
 * This is used to associate container structures to their underlying file
 * counter parts since LIBMTP makes a distinction between a container's
 * specific structure (eg. folder, playlist, album) and its underlying
 * file/object representation.
 * Thus for each playlist there is a file associated with it as well.
 * The same goes for albums, but not for folders (they are a special type of
 * container)
 * Also- when deleting an album, one should take care to delete its associated
 * file/object reference since it is not guaranteed that it will be deleted.
 * @param in_obj the object to associate this object with
 */
void GenericFileObject::Associate(GenericFileObject* in_obj)
{
  if (Type() == MtpFile)
   assert(in_obj->Type() != MtpFile);

  //assert that we don't call this function twice on the same object
  assert(!_association);
   _association = in_obj;
}

/**
 * Retreives the file associated with this album
 */
GenericFileObject* GenericFileObject::Association() const
{
  return _association;
}


/**
 * Creates a new Track object
 * @param in_track A pointer to the LIBMTP_track_t to wrap over
 * @return a new Track object
 */

Track::Track(LIBMTP_track_t* in_track) :
            GenericFileObject(MtpTrack, in_track->item_id),
            _parentAlbum (NULL)
{
  assert(in_track);
  _rawTrack = in_track;
}

/**
 * @return the visual row index for this track
 * */
count_t Track::GetRowIndex() const { return _rowIndex; }

/**
 * Sets the visual row index for this track
 **/
void Track::SetRowIndex(count_t in_row) { _rowIndex = in_row; }

/**
 * Retreives the name of the wrapped Track
 * @return the tracks's UTF8 name
 */
char const * Track::Name() const
{
  return _rawTrack->title;
}

/**
 * Retreives the name of the wrapped Album
 * @return the album's UTF8 name
 */
char const * Track::AlbumName() const
{
  return _rawTrack->album;
}

/**
 * Returns the raw track that this object wraps around
 * @return the raw track;
 */
LIBMTP_track_t* Track::RawTrack() const
{
  return _rawTrack;
}

/**
 * Retreives the file name of the wrapped Track
 * @return the tracks's UTF8 name
 */
char const * Track::FileName() const
{
  return _rawTrack->filename;
}

/**
 * Retreives the Artist name of the wrapped Track
 * @return the tracks's UTF8 name
 */
char const * Track::ArtistName() const
{
  return _rawTrack->artist;
}

/**
 * Retreives the genre of the wrapped Track
 * @return the tracks's UTF8 name
 */
char const * Track::Genre() const
{
  return _rawTrack->genre;
}

/**
 * Associates a shadow track to this track- this helps us keep track of
 * playlist membership when a track is deleted
 * @returns the association index of the track
 */
count_t Track::AssociateShadowTrack(ShadowTrack* in_track)
{
  count_t idx = _shadowTracks.size();
  _shadowTracks.push_back(in_track);
  return idx;
}

/**
 * Disassociates a shadow track to this track- we do this when a track is
 * removed from a playlist.
 **/
void Track::DisassociateShadowTrack(count_t in_idx)
{
  assert(in_idx < _shadowTracks.size());
  assert(_shadowTracks[in_idx] != NULL);
  _shadowTracks[in_idx] = NULL;
}

/**
 * @return the number of shadow track associations. Useful to iterate over all
 * assoicated tracks.
 **/
count_t Track::ShadowAssociationCount()
{
  return _shadowTracks.size();
}

/**
 * Returns the ShadowTrack by index of association.
 * @param in_index the index of the associated track.
 * @returns the track at the given index or NULL if the track was removed
 **/
ShadowTrack* Track::ShadowAssociation(count_t in_index)
{
  return _shadowTracks[in_index];
}

/**
 * Returns the parent id of this track
 * @return the parent id of this track
 */
count_t Track::ParentFolderID() const { return _rawTrack->parent_id; }

/**
 * Sets the parent album of this track
 * @param in_album the parent album of this track
 */
void Track::SetParentAlbum(Album* in_album) {_parentAlbum = in_album; }

/**
 * Returns the parent Album of this track
 * @return the parent Album of this track
 */
Album* Track::ParentAlbum() const { return _parentAlbum; }

/**
 * @return the size in bytes this track takes up on the device
 */
count_t Track::FileSize() const { return _rawTrack->filesize; }

/**
 * @return the LIBMTP filetype of this track
 */
LIBMTP_filetype_t Track::FileType() const { return _rawTrack->filetype; }


ShadowTrack::ShadowTrack(Track* in_track, Playlist* in_pl, count_t in_idx) :
                         GenericObject(MtpShadowTrack, 0),
                         _track(in_track),
                         _parentPlaylist(in_pl),
                         _rowIndex(in_idx)
{
  _trackAssociationIndex = _track->AssociateShadowTrack(this);
}

ShadowTrack::~ShadowTrack()
{
  _track->DisassociateShadowTrack(_trackAssociationIndex);
}

count_t ShadowTrack::RowIndex() const
{
  return _rowIndex;
}

void ShadowTrack::SetRowIndex(count_t in_idx)
{
  _rowIndex = in_idx;
}

Playlist* ShadowTrack::ParentPlaylist() const
{
  return _parentPlaylist;
}

const Track* ShadowTrack::GetTrack() const
{
  return _track;
}


/**
 * Creates a new File object and and stores its representative data
 * @param in_file A pointer to the LIBMTP_file_to wrap over
 * @param in_depth The depth of this file in the file system
 * @param in_sample A pointer to the LIBMTP_filesampledata_t
 * @return a new File object
 */
File::File(LIBMTP_file_t* in_file, count_t in_depth) :
           GenericFileObject(MtpFile, in_file->item_id),
           _depth(in_depth)
{
  _rawFile = in_file;
}

/**
 * Creates a new File object from a track and sets up the association
 * @param in_track A pointer to the associated track
 * @param in_parent A pointer to the parent folder for this file
 * @param in_storage_id The storage ID that this file resides on
 * @return a new File object
 */
File::File(Track* in_track, Folder* in_parent, uint32_t in_storage_id) :
          GenericFileObject(MtpFile, in_track->ID()),
          _depth(in_parent->Depth())
{
  assert(in_parent->ID() == in_track->ParentFolderID());
  _rawFile = new LIBMTP_file_t();
  _rawFile->parent_id = in_parent->ID();
  _rawFile->storage_id = in_storage_id;
  _rawFile->filename = strdup(in_track->FileName());
  _rawFile->filesize = in_track->FileSize();
  _rawFile->filetype = in_track->FileType();
}

/**
 * Retreives the file's parent ID
 * @return the file's parent ID it is zero if its in the root folder
 */
count_t File::ParentID() const { return _rawFile->parent_id; }

/**
 * Retreives the file's parent Folder
 * @return the file's parent Folder or NULL if it exists in the root dir
 */
Folder* File::ParentFolder() const { return _parent; }

/**
 * Sets the file's parent Folder
 */
void File::SetParentFolder(Folder const * in_parent)
{
  _parent = const_cast <Folder*>  (in_parent);
}

/*
 * Returns the row index of this file
 */
count_t File::GetRowIndex() const
{
  return _rowIndex;
}

/*
 * Returns the row index of this file
 */
void File::SetRowIndex(count_t in_idx)
{
  _rowIndex = in_idx;
}

/**
 * Returns the name of the file
 * @return The name of the file as a Utf8 string
 */
char const * File::Name() const
{
  //TODO error checking here?
  //     no, this should be the caller's duty
  return _rawFile->filename;
}

/** Returns the raw file that this object wraps around
 * @return the raw file;
 */
LIBMTP_file_t* File::RawFile() const
{
  return _rawFile;
}

/** Creates a new Folder object
 * @param in_folder A pointer to the LIBMTP_folder_t wrap over
 * @param in_depth The depth of this folder in the filesystem
 * @param in_parent A pointer to this folder's parent
 * @return a new Folder object
 */
Folder::Folder(LIBMTP_folder_t* in_folder, Folder* in_parent,
               count_t in_depth) :
               GenericObject (MtpFolder, in_folder->folder_id),
               _depth(in_depth),
               _parent(in_parent)
{
  assert(in_folder);
//  cout << "Creating new folder " << in_folder->name << " with id:" << ID() << endl;
  _rawFolder = in_folder;
}
/**
 * Return's this folder's parent
 * @return the folder's parent
 */
Folder* Folder::ParentFolder() const
{
  return _parent;
}

/**
 * Get the raw LIBMTP folder
 * @return the raw LIBMTP folder for this Folder object
 */
LIBMTP_folder_t* Folder::RawFolder() const
{
  return _rawFolder;
}

/**
 * Returns the subfolder at the given index if it exists.
 * @return the subfolder at the given index or NULL if it doesn't exist
 */
Folder* Folder::ChildFolder(count_t idx) const
{
  if (idx >= _childFolders.size())
  {
    cerr << "Requesting invalid child index for folder" << endl;
    return NULL;
  }
  else
    return _childFolders[idx];
}

/**
 * Returns the subfolder at the given index if it exists.
 * @return the subfolder at the given index or NULL if it doesn't exist
 */
File* Folder::ChildFile(count_t idx) const
{
  if (idx >= _childFiles.size())
    return NULL;
  else
    return _childFiles[idx];
}


/**
 * Returns the number of files under this folder
 * @return the number of files under this folder
 */
count_t Folder::FileCount() const
{
  return _childFiles.size();
}

/**
 * @return the number of folders under this folder
 */
count_t Folder::FolderCount() const
{
  return _childFolders.size();
}

/** Retreives the name of the wrapped folder or "" if it doesn't exist
 * @return the folder's UTF8 name or a blank string if it doesn't exist
 */
const char* Folder::Name() const
{
  if (!_rawFolder)
    return "";
  else
    return _rawFolder->name;
}

/** Adds the passed folder as a subdirectory to this folder
 * @param in_folder the folder to add as a subfolder of this folder
 */
void Folder::AddChildFolder(Folder* in_folder)
{
  _childFolders.push_back(in_folder);
}

/** Adds the passed file as a leaf file of this
 * @param in_file the file to add as a leaf of this folder
 */
void Folder::AddChildFile(File* in_file)
{
  _childFiles.push_back(in_file);
}
void Folder::RemoveChildFolder(Folder* in_folder)
{
  vector<MTP::Folder*>::iterator iter = _childFolders.begin();
  count_t i = 0;
  for (; iter != _childFolders.end() && i < in_folder->GetRowIndex(); i++)
  {
    iter++;
  }
  if (i != in_folder->GetRowIndex())
  {
    cerr << "Deletion error: in_folder row index:" << in_folder->GetRowIndex()
         << " child folder count: " << _childFolders.size() << endl;
    assert(false);
  }

  _childFolders.erase(iter);
  return;
}
/**
 * @return The depth of this folder in the folder tree
 * */
count_t Folder::Depth() const { return _depth; }
/**
 * Remove a direct child file from the folder's list of child files.
 * @param in_file The file to remove.
 * */
void Folder::RemoveChildFile(File* in_file)
{
  vector<MTP::File*>::iterator iter = _childFiles.begin();
  count_t i = 0;
  for (; iter != _childFiles.end() && i < in_file->GetRowIndex(); i++)
  {
    iter++;
  }
  if (i != in_file->GetRowIndex())
  {
    cerr << "Deletion error: in_file row index:" << in_file->GetRowIndex()
         << " child file count: " << _childFiles.size() << endl;
    assert(false);
  }

  _childFiles.erase(iter);
  return;
}

/**
 * @return the visual row index for this folder
 * */
count_t Folder::GetRowIndex() const { return _rowIndex; }

/**
 * Sets the visual row index for this folder
 * @param in_row the new row of this folder
 * */
void Folder::SetRowIndex(count_t in_row) { _rowIndex = in_row; }

/** Creates a new Album object
 * @param in_album A pointer to the LIBMTP_album_t wrap over
 * @return a new Album object
 */
Album::Album(LIBMTP_album_t* in_album,
             const LIBMTP_filesampledata_t & in_sample) :
             GenericFileObject(MtpAlbum, in_album->album_id),
             _sample(in_sample)
{
  assert(in_album);
  _rawAlbum = in_album;
  _initialized = false;
}
/**
 * This function sets the representative sample of the album to the passed
 * param
 * @param in_sample the sample that will be set for this album
 */
void Album::SetCover(LIBMTP_filesampledata_t const * in_sample)
{
  _sample = *in_sample;
}

/** Returns the sample data for the album
 * @return a reference to the LIBMTP_sampledata_t that was pulled from
 * the device
 */
const LIBMTP_filesampledata_t& Album::SampleData() const
{
  return _sample;
}


/** Adds the passed track as a subtrack to this album
 *  The caller must ensure that the album is then updated on the device
 *  If the gui is adding a track, you must first add the track to the raw
 *  folder before adding it to the wrapper
 * @param in_track the track to add as a subtrack to this folder
 */
void Album::AddTrack(Track* in_track)
{
  in_track->SetParentAlbum(this);
  //row index is not _childTracks.size() +1 as it is zero based..
  in_track->SetRowIndex( _childTracks.size());
  _childTracks.push_back(in_track);
}

/** Adds the passed track as a subtrack to the raw album of this object only
 *  The caller must ensure that the album is then updated on the device
 *  The reason this is split phase is so that we can update the device before
 *  we update the view
 * @param in_track the track to add as a subtrack to this folder
 */
void Album::AddTrackToRawAlbum(Track* in_track)
{
  count_t trackCount = _rawAlbum->no_tracks;
  count_t* tracks = new count_t[trackCount+1];
  for (count_t i =0; i < trackCount; i++)
    tracks[i] = _rawAlbum->tracks[i];

  tracks[trackCount] = in_track->ID();
  _rawAlbum->no_tracks = trackCount +1;
  //LIBMTP does this automatically for us..
  //delete [] _rawAlbum->tracks;
  _rawAlbum->tracks = tracks;
}

/* @return the RawAlbum that this object wraps over*/
LIBMTP_album_t const* Album::RawAlbum()
{
  return _rawAlbum;
}


/** Removes the track at the given index of the album.
 *  The caller must ensure that the album container is then updated on the
 *  device
 * @param in_index the track to remove
 */

void Album::RemoveTrack(count_t in_index)
{
  if (in_index > _childTracks.size())
    return;
//  cout << "before removal album size: " << _childTracks.size() << endl;
  Track* deletedTrack = _childTracks[in_index];
  vector<Track*>::iterator iter = _childTracks.begin();
  vector<Track*>::iterator backup_iter;
  int i =0;
  while (*iter !=  deletedTrack)
  {
    i++;
    iter++;
    assert(iter != _childTracks.end());
  }
//  cout << "Iterator found index at: " << i << " vs " << in_index << endl;

  backup_iter = iter +1;
  //Ensure that objects below this object have the correct index
  while (backup_iter != _childTracks.end())
  {
    Track* currentTrack = (*backup_iter);
    int prevIdx =  currentTrack->GetRowIndex( );
    assert(prevIdx != 0);
    currentTrack->SetRowIndex( currentTrack->GetRowIndex() -1);
    backup_iter++;
  }
  assert(*iter == deletedTrack);
  _childTracks.erase(iter);
  delete deletedTrack;
//  cout << "after removal album size: " << TrackCount() << endl;
}

/**
 * Removes the track at the given index of the album's internal structure.
 * @param in_index the track index to remove
 */
void Album::RemoveFromRawAlbum(count_t in_index)
{
    count_t trackCount = _rawAlbum->no_tracks;
    assert(trackCount != 0);
    assert(in_index < trackCount);

    count_t* tracks = NULL;
    if (trackCount > 1)
    {
      tracks = new count_t[trackCount-1];
      for (count_t i =0; i < in_index; i++)
      {
        tracks[i] = _rawAlbum->tracks[i];
      }
      for (count_t i = in_index+1; i < trackCount; i++)
      {
        tracks[i-1] = _rawAlbum->tracks[i];
      }

      //LIBMTP does not know that its the tracks association has been removed
      _rawAlbum->no_tracks = trackCount -1;
      //TODO will this cause problems in LIBMTP's caching system?
      delete [] _rawAlbum->tracks;
      _rawAlbum->tracks = tracks;
    }
    else if (trackCount == 1)
    {
      _rawAlbum->no_tracks = 0;
      //see TODO note above regarding LIBMTP's caching system
      delete [] _rawAlbum->tracks;
      _rawAlbum->tracks = NULL;
    }
    else if (trackCount == 0)
    {
      _rawAlbum->tracks = NULL;
    }
}

/**
 * Retreives the name of the wrapped Album
 * @return the album's UTF8 name
 */
char const * Album::Name() const
{
  return _rawAlbum->name;
}

/**
 * Retreives the artist name of the wrapped Album
 * @return the albums's artist name in UTF8
 */
char const * Album::ArtistName() const
{
  cout << "Artist: " << _rawAlbum->artist << endl;
  return _rawAlbum->artist;
}

/**
 * Albums are container objects that hold a list of tracks that
 * reside underneath them
 * If the object is not initialized then we return the track count from the raw
 * container representation
 * Otherwise return the vector size
 * @return the track count under this album
 */
count_t Album::TrackCount() const
{
  if (!_initialized)
   return _rawAlbum->no_tracks;
  else
    return _childTracks.size();
}

/**
 * Albums are container objects that hold a list of track IDs
 * @param idx the index of the requested track
 * @return the uint32_t track ID specified at the given index
 */
uint32_t Album::ChildTrackID(count_t idx) const
{
  assert(idx < TrackCount());
  return _rawAlbum->tracks[idx];
}


/**
 * Albums are also container objects that hold a list of tracks that
 * reside underneath them
 * @param idx the index of the requested track
 * @return the Track* specified at the given index
 */
Track* Album::ChildTrack(count_t idx) const
{
  assert(idx < _childTracks.size());
  return _childTracks[idx];
}

/**
 * The Initialized state tells us when to stop using the underlying
 * LIBMTP data structure as it might become stale.
 */
void Album::SetInitialized() { _initialized = true; }
bool Album::Initialized() { return _initialized; }

/**
 * @return the visual row index for this album
 * */
count_t Album::GetRowIndex() const { return _rowIndex; }

/**
 * Sets the visual row index for this album
 * @param in_row the new row of this album
 * */
void Album::SetRowIndex(count_t in_row) { _rowIndex = in_row; }

/**
 * Creates a new Playlist object
 * @param in_pl t pointer to the LIBMTP_playlist_t wrap over
 * @return a new Playlist object
 */
Playlist::Playlist(LIBMTP_playlist_t* in_pl) :
                  GenericFileObject(MtpPlaylist, in_pl->playlist_id)
{
  _initialized = false;
  _rawPlaylist =  in_pl;
}

/**
 * Returns the name of this Playlist
 * @return the name of this playlist;
 */
char const * Playlist::Name() const
{
  return _rawPlaylist->name;
}

/**
 * Adds a track to the list of child tracks. This function also sets the child
 * tracks rowIndex
 * @param in_track the pointer to the child track to add
 */
void Playlist::AddTrack(Track* in_track)
{
  ShadowTrack* strack = new ShadowTrack(in_track, this, _childTracks.size());
  _childTracks.push_back(strack);
  cout << "track count:" << _childTracks.size();
  cout << " given rowid:" << strack->RowIndex() << endl;
  /*
  in_track->SetPlaylistRowIndex(_childTracks.size());
  _childTracks.push_back(in_track);
  in_track->SetParentPlaylist(this);
  */
}

/**
 * Returns the child track at the given index
 * @param idx the index of the child track in the Playlists vector
 * @return the child tradck at the given index or null if it doesn't exist
 */
ShadowTrack* Playlist::ChildTrack(count_t idx) const
{
  assert(idx < _childTracks.size());
  if (idx >= _childTracks.size())
    return NULL;
  return _childTracks[idx];
}

/**
 * Playlists are also container objects that hold a list of tracks that
 * reside underneath them
 * @return the number of tracks underneath this playlist
 */
count_t Playlist::TrackCount() const
{
  if (!_initialized)
   return _rawPlaylist->no_tracks;
  else
  {
    return _childTracks.size();
  }
}

/**
 * Playlists are container objects that hold a list of track IDs
 * @param idx the index of the requested track id
 * @return the uint32_t track ID specified at the given index
 */
//FIXME there is a serious bug here if the trackcount is off from the underlying obj
uint32_t Playlist::ChildTrackID(count_t idx) const
{
  assert(idx < TrackCount());
  if (!_initialized)
    return _rawPlaylist->tracks[idx];
  else
    return _childTracks[idx]->GetTrack()->ID();
}

/**
 * The Initialized state tells us when to stop using the underlying
 * LIBMTP data structure as it might become stale.
 */
void Playlist::SetInitialized() { _initialized = true; }

/**
 * @return the visual row index for this playlist
 * */
count_t Playlist::GetRowIndex() const { return _rowIndex; }

/**
 * Sets the visual row index for this playlist
 * @param in_row the new row of this playlist
 * */
void Playlist::SetRowIndex(count_t in_row) { _rowIndex = in_row; }

/** Removes the track at the given index of the playlist.
 *  The caller must ensure that the playlist container is then updated on the
 *  device
 * @param in_index the track to remove
 */
void Playlist::RemoveTrack(count_t in_index)
{
  if (in_index > _childTracks.size())
    return;
  //cout << "before removal album size: " << _childTracks.size() << endl;
  ShadowTrack* deletedTrack = _childTracks[in_index];
  vector<ShadowTrack*>::iterator iter = _childTracks.begin();
  vector<ShadowTrack*>::iterator backup_iter;
  int i =0;
  while (*iter !=  deletedTrack)
  {
    i++;
    iter++;
    assert(iter != _childTracks.end());
  }
  //cout << "Iterator found index at: " << i << " vs " << in_index << endl;

  backup_iter = iter +1;
  //Ensure that objects below this object have the correct index
  while (backup_iter != _childTracks.end())
  {
    ShadowTrack* currentTrack = (*backup_iter);
    int prevIdx =  (*backup_iter)->RowIndex();
    assert(prevIdx != 0);
    (*backup_iter)->SetRowIndex( (*backup_iter)->RowIndex() -1);
    backup_iter++;
  }
  assert(*iter == deletedTrack);
  _childTracks.erase(iter);
  delete deletedTrack;
  //cout << "after removal album size: " << TrackCount() << endl;
}

/**
 * Removes the track at the given index of the playlist's internal structure.
 * @param in_index the track index to remove.
 */
const LIBMTP_playlist_t* Playlist::RawPlaylist()
{
  return _rawPlaylist;
}

/**
 * Removes the track at the given index of the playlist's internal structure.
 * @param in_index the track index to remove
 */
void Playlist::RemoveFromRawPlaylist(count_t in_index)
{
    cout << "Remove pl index: " << in_index<< endl;
    count_t trackCount = _rawPlaylist->no_tracks;
    assert(trackCount != 0);
    assert(in_index < trackCount);

    count_t* tracks = NULL;
    if (trackCount > 1)
    {
      tracks = new count_t[trackCount-1];
      for (count_t i =0; i < in_index; i++)
      {
        tracks[i] = _rawPlaylist->tracks[i];
      }
      for (count_t i = in_index+1; i < trackCount; i++)
      {
        tracks[i-1] = _rawPlaylist->tracks[i];
      }

      _rawPlaylist->no_tracks = trackCount -1;
      delete [] _rawPlaylist->tracks;
      _rawPlaylist->tracks = tracks;
    }
    else if (trackCount == 1)
    {
      //LIBMTP does not know that its the tracks association has been removed
      //TODO will this cause problems in LIBMTP's caching system?
      _rawPlaylist->no_tracks = 0;
      delete [] _rawPlaylist->tracks;
      _rawPlaylist->tracks = NULL;
    }
    else if (trackCount == 0)
    {
      _rawPlaylist->tracks = NULL;
    }

    cout << "RemoveFromRawPlaylists end: " << _rawPlaylist->no_tracks<< endl;
    cout << "RemoveFromRawPlaylists end2: " << tracks << endl;

}

}
