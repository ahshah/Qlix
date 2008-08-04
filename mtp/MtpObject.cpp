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
 
TODO error checkking when returning char* ?
TODO use multimap for track/file distinction
*/
#include "mtp/MtpObject.h"
using namespace MTP;
namespace MTP
{

/** Creates a new GenericObject, a generic base class for MTP objects
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
 * Simple function to get the type of the current MTP object
 * @return returns the type of the GenericObject
 */
MtpObjectType GenericObject::Type() const { return _type; }


/** 
 * @return the name of this object
 */
const char* const  GenericObject::Name() const { return ""; }



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
 * file
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


/** Creates a new Track object
 * @param in_track A pointer to the LIBMTP_track_t to wrap over
 * @return a new Track object
 */

Track::Track(LIBMTP_track_t* in_track) :
            GenericFileObject(MtpTrack, in_track->item_id),
            _parentAlbum (NULL),
            _parentPlaylist(NULL)
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
 * */
void Track::SetRowIndex(count_t in_row) { _rowIndex = in_row; }


/** Retreives the name of the wrapped Track
 * @return the tracks's UTF8 name 
 */
char const * const Track::Name() const
{
  return _rawTrack->title;
}

/** Retreives the name of the wrapped Album
 * @return the album's UTF8 name 
 */
char const * const Track::AlbumName() const
{
  return _rawTrack->album;
}

/** Returns the raw track that this object wraps around
 * @return the raw track;
 */
LIBMTP_track_t* const Track::RawTrack() const
{
  return _rawTrack;
}

/** Retreives the file name of the wrapped Track
 * @return the tracks's UTF8 name 
 */
char const * const Track::FileName() const
{
  return _rawTrack->filename;
}

/** Retreives the Artist name of the wrapped Track
 * @return the tracks's UTF8 name 
 */
char const * const Track::ArtistName() const
{
  return _rawTrack->artist;
}

/** Retreives the genre of the wrapped Track
 * @return the tracks's UTF8 name 
 */
char const * const Track::Genre() const
{
  return _rawTrack->genre;
}

/** Returns the parent id of this track
 * @return the parent id of this track
 */
count_t Track::ParentFolderID() const { return _rawTrack->parent_id; }


/** Sets the parent album of this track
 * @param in_album the parent album of this track
 */
void Track::SetParentAlbum(Album* in_album) {_parentAlbum = in_album; }


/** Sets the parent playlist of this track
 * @param in_pl the parent playlist of this track
 */
void Track::SetParentPlaylist(Playlist* in_pl) {_parentPlaylist = in_pl; }

/** Returns the parent Album of this track
 * @return the parent Album of this track
 */
Album* Track::ParentAlbum() const { return _parentAlbum; }

/** Returns the parent Playlist of this track
 * @return the parent Playlist of this track
 */
Playlist* Track::ParentPlaylist() const { return _parentPlaylist; }


/** Creates a new File object and and stores its representative data
 * @param in_file A pointer to the LIBMTP_file_to wrap over
 * @param in_sample A pointer to the LIBMTP_filesampledata_t
 * @return a new File object
 */
File::File(LIBMTP_file_t* in_file) : 
           GenericFileObject(MtpFile, in_file->item_id)
{
  _rawFile = in_file;
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
void File::SetParentFolder(Folder* in_parent) 
{ 
  _parent = in_parent;
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
char const * const File::Name() const
{
  //TODO error checking here?
  //     no, this should be the caller's duty
  return _rawFile->filename;
}

/** Returns the raw file that this object wraps around
 * @return the raw file;
 */
LIBMTP_file_t* const File::RawFile() const
{
  return _rawFile;
}

/** Creates a new Folder object
 * @param in_folder A pointer to the LIBMTP_folder_t wrap over
 * @param in_parent A pointer to this folder's parent
 * @return a new Folder object
 */
Folder::Folder(LIBMTP_folder_t* in_folder, Folder* in_parent) :
               GenericObject (MtpFolder, in_folder->folder_id)
{
  assert(in_folder);
//  cout << "Creating new folder " << in_folder->name << " with id:" << ID() << endl;
  _rawFolder = in_folder;
  _parent = in_parent;
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
char const* const Folder::Name() const
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
 *  The caller must ensure that the album is then updated on the device
 * @param in_index the track to remove
 * @param updateInternalStruct condition whether or not to update the
 *        structure on the device
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
    int prevIdx =  (*backup_iter)->GetRowIndex( );
    assert(prevIdx != 0);
    (*backup_iter)->SetRowIndex( (*backup_iter)->GetRowIndex() -1);
    backup_iter++;
  }
  assert(*iter == deletedTrack); 
  _childTracks.erase(iter);
  delete deletedTrack;
//  cout << "after removal album size: " << TrackCount() << endl;
}

void Album::RemoveFromRawAlbum(count_t index)
{
    count_t trackCount = _rawAlbum->no_tracks;
    count_t* tracks = NULL;
    if (trackCount-1 > 0)
      tracks = new count_t[trackCount-1];
    for (count_t i =0; i < index; i++)
    {
      tracks[i] = _rawAlbum->tracks[i];
    }
    for (count_t i = index+1; i < trackCount; i++)
    {
      tracks[i-1] = _rawAlbum->tracks[i];
    }

    _rawAlbum->no_tracks = trackCount -1;
//    delete [] _rawAlbum->tracks;
    _rawAlbum->tracks = tracks;
}

/** Retreives the name of the wrapped Album
 * @return the album's UTF8 name 
 */
char const * const Album::Name() const
{
  return _rawAlbum->name;
}

/**
 * Retreives the artist name of the wrapped Album
 * @return the albums's artist name in UTF8
 */
char const * const Album::ArtistName() const
{
  cout << "Artist: " << _rawAlbum->artist << endl;
  return _rawAlbum->artist;
}




/**
 * Albums are container objects that hold a list of tracks that 
 * reside underneath them
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




/** Creates a new Playlist object
 * @param in_pl t pointer to the LIBMTP_playlist_t wrap over
 * @return a new Playlist object
 */
Playlist::Playlist(LIBMTP_playlist_t* in_pl) : 
                  GenericFileObject(MtpPlaylist, in_pl->playlist_id)
{
  _initialized = false;
  _rawPlaylist =  in_pl;
}

/** Returns the name of this Playlist
 * @return the name of this playlist;
 */
char const * const Playlist::Name() const
{
  return _rawPlaylist->name;
}


/** Adds a track to the list of child tracks
 * @param in_track the pointer to the child track to add
 */
void Playlist::AddTrack(Track* in_track) 
{
  _childTracks.push_back(in_track);
  in_track->SetParentPlaylist(this);
}

/** Returns the child track at the given index
 * @param idx the index of the child track in the Playlists vector
 * @return the child tradck at the given index or null if it doesn't exist
 */
Track* Playlist::ChildTrack(count_t idx) const
{
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
    return _childTracks.size();
}

/**
 * Playlists are container objects that hold a list of track IDs 
 * @param idx the index of the requested track id
 * @return the uint32_t track ID specified at the given index
 */ 
//there is a serious bug here if the trackcount is off from the underlying obj
uint32_t Playlist::ChildTrackID(count_t idx) const
{
  assert(idx < TrackCount());
  return _rawPlaylist->tracks[idx];
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

}
