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
 */

#include "GenericObjects.h"
#include "Playlist.h"
#include "Track.h"
#include "ShadowTrack.h"
using namespace MTP;

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
 * @return the storage ID that this playlist resides on.
 */
uint32_t Playlist::StorageID() const
{
  return _rawPlaylist->storage_id;
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

