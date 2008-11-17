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

#ifndef __MTPOBJECT__
#define __MTPOBJECT__

#include <libmtp.h>
#include <vector>
#include <assert.h>
#include "types.h"
#include <iostream>
#include <string>
namespace MTP
{

//forward declaration
  class GenericObject;
  class GenericFileObject;
  class File;
  class Folder;
  class Album;
  class Track;
  class Playlist;
  class ShadowTrack;

/** 
 * @class Generic base class for other MTP object types
*/
class GenericObject
{
public:
  GenericObject(MtpObjectType, uint32_t);
  virtual ~GenericObject();
  count_t ID() const;
  void SetID(count_t);
  MtpObjectType Type() const;
  virtual const char* Name() const;

private:
  MtpObjectType _type;
  count_t _id;
};

class GenericFileObject : public GenericObject
{
  public:
  GenericFileObject(MtpObjectType, uint32_t);
  void Associate(GenericFileObject* );
  GenericFileObject* Association() const;

  private:
  GenericFileObject* _association;
};

/** 
 * @class File is a class that wraps around LIBMTP_file_t
*/
class File : public GenericFileObject 
{
public:
  File(LIBMTP_file_t*);
  count_t ParentID() const;
  virtual const char * Name() const;

  void SetParentFolder(Folder*);
  Folder* ParentFolder() const;

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  LIBMTP_file_t* RawFile() const;
private:
  LIBMTP_file_t* _rawFile;
  LIBMTP_filesampledata_t _sampleData;
  Folder* _parent;
  count_t _rowIndex;
};

/** 
 * @class Folder is a class that wraps around LIBMTP_folder_t
*/
class Folder : public GenericObject
{
public:
  Folder(LIBMTP_folder_t*, Folder*);
  count_t FileCount() const;
  count_t FolderCount() const;
  Folder* ParentFolder() const;

  virtual char const * Name() const;
  Folder* ChildFolder(count_t ) const;
  File* ChildFile(count_t ) const;

  LIBMTP_folder_t* RawFolder() const;
  void AddChildFolder(Folder*);
  void AddChildFile(File*);
  void RemoveChildFolder(Folder*);
  void RemoveChildFile(File*);

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

private:
  LIBMTP_folder_t* _rawFolder;
  Folder* _parent;
  std::vector<Folder*> _childFolders;
  std::vector<File*> _childFiles;
  count_t _rowIndex;
};

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
  //Not such a hot idea..
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

  void AddTrack(Track*);
  void AddTrackToRawAlbum(Track* in_track);

  void RemoveFromRawAlbum(count_t index);
  void RemoveTrack(count_t in_index);

  virtual const char* Name() const;
  const char* ArtistName() const;

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  void SetAssociation(File*);

private:
  bool _initialized;
  LIBMTP_album_t* _rawAlbum;
  LIBMTP_filesampledata_t _sample;
  std::vector <Track*> _childTracks;

  File* _associatedFile;
  count_t _rowIndex;
};

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
