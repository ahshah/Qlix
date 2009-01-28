#ifndef MTP_TRACK
#define MTP_TRACK

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


namespace MTP
{
class Album;
class ShadowTrack;
class File;
/**
 * @class Track is a class that wraps around LIBMTP_track_t
*/
class Track : public GenericFileObject
{
public:
  Track(LIBMTP_track_t*);
  count_t ParentFolderID() const;
  void SetParentAlbum(Album*);

  virtual const char* Name() const;
  const char* FileName() const;
  const char* Genre() const;
  const char* AlbumName() const;
  const char* ArtistName() const;
  Album* ParentAlbum() const;
  size_t FileSize() const;
  LIBMTP_filetype_t FileType() const;
  //Not such a hot idea..
  virtual uint32_t StorageID() const;
  LIBMTP_track_t* RawTrack() const;

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  count_t AssociateShadowTrack(ShadowTrack*);
  void DisassociateShadowTrack(count_t);

  count_t ShadowAssociationCount();
  ShadowTrack* ShadowAssociation(count_t);

private:
  LIBMTP_track_t* _rawTrack;
  LIBMTP_filesampledata_t _sampleData;
  Album* _parentAlbum;
  vector<ShadowTrack*> _shadowTracks;

  File* _associatedFile;
  count_t _rowIndex;
};

}
#endif
