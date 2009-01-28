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
#include "Track.h"
using namespace MTP;

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
 * Retrieves the name of the wrapped Album
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
 * @return the storage ID that this track resides on
 */
uint32_t Track::StorageID() const
{
  return _rawTrack->storage_id;
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


