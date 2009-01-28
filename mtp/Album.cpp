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

#include "GenericObjects.h"
#include "Album.h"
#include "Track.h"

using namespace MTP;

/**
 * Creates a new Album object
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


/**
 * Adds the passed track as a subtrack to this album
 * The caller must ensure that the album is then updated on the device
 * If the GUI is adding a track, you must first add the track to the raw
 * folder before adding it to the wrapper
 * @param in_track the track to add as a subtrack to this folder
 */
void Album::AddTrack(Track* in_track)
{
  in_track->SetParentAlbum(this);
  //row index is not _childTracks.size() +1 as it is zero based..
  in_track->SetRowIndex( _childTracks.size());
  _childTracks.push_back(in_track);
}

/**
 * Adds the passed track as a subtrack to the raw album of this object only
 * The caller must ensure that the album is then updated on the device
 * The reason this is split phase is so that we can update the device before
 * we update the view
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


/**
 * Removes the track at the given index of the album.
 * The caller must ensure that the album container is then updated on the
 * device
 * @param in_index the track to remove
 */

void Album::RemoveTrack(count_t in_index)
{
  if (in_index > _childTracks.size())
    return;
  // cout << "before removal album size: " << _childTracks.size() << endl;
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
  // cout << "Iterator found index at: " << i << " vs " << in_index << endl;

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
 * @return the storage ID that this album resides on
 */
uint32_t Album::StorageID() const { return _rawAlbum->storage_id; }

/**
 * @return the parent folder's ID that this album resides in
 */
uint32_t Album::ParentFolderID() const { return _rawAlbum->parent_id; }

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
