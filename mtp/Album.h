#ifndef MTP_ALBUM
#define MTP_ALBUM
#include "GenericObjects.h"
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
 *k   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
namespace MTP
{

class Track;
class File;
/**
 * @class Album is a class that wraps around LIBMTP_album_t
*/
class Album : public GenericFileObject
{

public:
  Album(LIBMTP_album_t*, const LIBMTP_filesampledata_t&);
  const LIBMTP_filesampledata_t& SampleData() const;
  count_t TrackCount() const;
  void SetCover(LIBMTP_filesampledata_t const * in_sample);

  const LIBMTP_album_t* RawAlbum();
  uint32_t ChildTrackID(count_t ) const;
  void SetInitialized();
  bool Initialized();
  Track* ChildTrack(count_t ) const;
  uint32_t ParentFolderID() const;

  void AddTrack(Track*);
  void AddTrackToRawAlbum(Track* in_track);

  void RemoveFromRawAlbum(count_t index);
  void RemoveTrack(count_t in_index);

  virtual const char* Name() const;
  const char* ArtistName() const;

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  void SetAssociation(File*);

  virtual uint32_t StorageID() const;
private:
  bool _initialized;
  LIBMTP_album_t* _rawAlbum;
  LIBMTP_filesampledata_t _sample;
  std::vector <Track*> _childTracks;

  File* _associatedFile;
  count_t _rowIndex;
};
}

#endif
