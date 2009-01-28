#ifndef MTP_PLAYLIST
#define MTP_PLAYLIST

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
namespace MTP
{
  class ShadowTrack;
  class Track;
  class File;
/**
 * @class Playlist is a class that wraps around LIBMTP_playlist_t
*/
class Playlist: public GenericFileObject
{
public:
  Playlist(LIBMTP_playlist_t*);
  count_t TrackCount() const;
  virtual const char* Name() const;

  void AddTrack(Track* );
  ShadowTrack* ChildTrack(count_t idx) const;
  uint32_t ChildTrackID(count_t idx) const;
  void SetInitialized();

  virtual uint32_t StorageID() const;
  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  void RemoveFromRawPlaylist(count_t index);
  void RemoveTrack(count_t index);

  const LIBMTP_playlist_t* RawPlaylist();

  void SetAssociation(File*);
private:
  count_t _rowIndex;
  bool _initialized;
  LIBMTP_playlist_t* _rawPlaylist;
  std::vector <ShadowTrack*> _childTracks;

  File* _associatedFile;
};
}
#endif
