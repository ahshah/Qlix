#ifndef MTP_SHADOW_TRACK
#define MTP_SHADOW_TRACK
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
class Playlist;
class Track;
/**
 * @class ShadowTrack is a class that provides an abstraction of the
 * relationship between tracks and playlists.
 * Since this relationship can be one(track)-to-many(playlists) we must create
 * a relationship for each one the associations.
 * This object does not exist in LIBMTP
*/
class ShadowTrack : public GenericObject
{
public:
  ShadowTrack(Track*, Playlist*, count_t);
  ~ShadowTrack();
  void SetRowIndex(count_t);
  count_t RowIndex() const;
  Playlist* ParentPlaylist() const;
  const Track* GetTrack() const;
private:
  Track* _track;
  Playlist* _parentPlaylist;
  count_t _rowIndex;
  count_t _trackAssociationIndex;
};
}
#endif
