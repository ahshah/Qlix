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
  MtpObjectType Type();
  virtual const char * Name() const;

private:
  MtpObjectType _type;
  count_t _id;
};

class GenericFileObject : public GenericObject
{
  public:
  GenericFileObject(MtpObjectType, uint32_t);
  void Associate(GenericFileObject*);
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
  virtual const char * const Name() const;

  void SetParentFolder(Folder*);
  Folder* ParentFolder() const;

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  LIBMTP_file_t* const RawFile() const;
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

  virtual char const * const Name() const;
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
  void SetParentPlaylist(Playlist*);
  virtual const char* const Name() const;
  const char* const FileName() const;
  const char* const Genre() const;
  const char* const AlbumName() const;
  const char* const ArtistName() const;
  Album* ParentAlbum() const;
  Playlist* ParentPlaylist() const;
  //Not such a hot idea..
  LIBMTP_track_t* const RawTrack() const;

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);


  //to be deprecated
private:
  LIBMTP_track_t* _rawTrack;
  LIBMTP_filesampledata_t _sampleData;
  Album* _parentAlbum;
  Playlist* _parentPlaylist;

  File* _associatedFile;
  count_t _rowIndex;
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

  LIBMTP_album_t const* RawAlbum();
  uint32_t ChildTrackID(count_t ) const;
  void SetInitialized();
  bool Initialized();
  Track* ChildTrack(count_t ) const;

  void AddTrack(Track*);
  void AddTrackToRawAlbum(Track* in_track);

  void RemoveFromRawAlbum(count_t index);
  void RemoveTrack(count_t in_index);

  virtual const char* const Name() const;
  const char* const ArtistName() const;

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  void SetAssociation(File*);
  File* Association();

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
  virtual const char* const Name() const;

  void AddTrack(Track* );
  Track* ChildTrack(count_t idx) const; 
  uint32_t ChildTrackID(count_t idx) const;
  void SetInitialized();

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);


  void SetAssociation(File*);
  File* Association();
private:
  count_t _trackCount;
  count_t _rowIndex;
  bool _initialized;
  LIBMTP_playlist_t* _rawPlaylist;
  std::vector <Track*> _childTracks;

  File* _associatedFile;
};
//TODO this isn't very usefull if you have foreign characters
static LIBMTP_filetype_t StringToType(const std::string& in_type)
{
  if (in_type == "UNKNOWN")
      return  LIBMTP_FILETYPE_UNKNOWN;

  if (in_type == "WAV") return LIBMTP_FILETYPE_WAV; 
  if (in_type == "MP3")
      return LIBMTP_FILETYPE_MP3;

  if (in_type == "WMA")
      return LIBMTP_FILETYPE_WMA;

  if (in_type == "OGG")
      return LIBMTP_FILETYPE_OGG;

  if (in_type == "AUD")
      return LIBMTP_FILETYPE_AUDIBLE;

  if (in_type == "MP4")
      return LIBMTP_FILETYPE_MP4;

  if (in_type == "WMV")
      return LIBMTP_FILETYPE_WMV;

  if (in_type == "AVI")
      return LIBMTP_FILETYPE_AVI;

  if (in_type == "MPEG")
      return LIBMTP_FILETYPE_MPEG;

  if (in_type == "MPG")
      return LIBMTP_FILETYPE_MPEG;

  if (in_type == "ASF")
      return LIBMTP_FILETYPE_ASF;

  if (in_type == "QT")
      return LIBMTP_FILETYPE_QT;

  if (in_type == "JPEG" || in_type == "JPG")
      return LIBMTP_FILETYPE_JPEG;

  if (in_type == "JFIF")
      return LIBMTP_FILETYPE_JFIF;

  if (in_type == "BMP")
      return LIBMTP_FILETYPE_BMP;

  if (in_type == "GIF")
      return LIBMTP_FILETYPE_GIF;

  if (in_type == "PICT")
      return LIBMTP_FILETYPE_PICT;

  if (in_type == "PNG")
      return LIBMTP_FILETYPE_PNG;

  if (in_type == "EXE")
      return LIBMTP_FILETYPE_WINEXEC;

  if (in_type == "TXT")
      return LIBMTP_FILETYPE_TEXT;

  if (in_type == "HTML" || in_type == "HTM")
      return LIBMTP_FILETYPE_HTML;

  if (in_type == "AAC")
      return LIBMTP_FILETYPE_AAC;

  if (in_type == "FLAC")
      return LIBMTP_FILETYPE_FLAC;

  if (in_type == "MP2")
      return LIBMTP_FILETYPE_MP2;

  if (in_type == "M4A")
      return LIBMTP_FILETYPE_M4A;

  if (in_type == "XML")
      return LIBMTP_FILETYPE_XML;

  if (in_type == "XLS")
      return LIBMTP_FILETYPE_XLS;

  if (in_type == "PPT")
      return LIBMTP_FILETYPE_PPT;

  if (in_type == "MHT")
      return LIBMTP_FILETYPE_MHT;

  if (in_type == "JP2")
      return LIBMTP_FILETYPE_JP2;

  if (in_type == "JPX")
      return LIBMTP_FILETYPE_JPX;

  return LIBMTP_FILETYPE_UNKNOWN; //default
}

static std::string TypeToString (LIBMTP_filetype_t in_type)
{
  switch (in_type)
  {
    case LIBMTP_FILETYPE_WAV:
      return "Wav";

    case LIBMTP_FILETYPE_MP3:
      return "Mp3";

    case LIBMTP_FILETYPE_WMA:
      return "Wma";

    case LIBMTP_FILETYPE_OGG:
      return "Ogg";

    case LIBMTP_FILETYPE_AUDIBLE:
      return "Aud";

    case LIBMTP_FILETYPE_MP4:
      return "Mp4";

    case LIBMTP_FILETYPE_UNDEF_AUDIO:
      return "Undef Audio";

    case LIBMTP_FILETYPE_WMV:
      return "Wmv";

    case LIBMTP_FILETYPE_AVI:
      return "Avi";

    case LIBMTP_FILETYPE_MPEG:
      return "Mpeg";

    case LIBMTP_FILETYPE_ASF:
      return "Asf";

    case LIBMTP_FILETYPE_QT:
      return "Qt";

    case LIBMTP_FILETYPE_UNDEF_VIDEO:
      return "Undef Video";

    case LIBMTP_FILETYPE_JPEG:
      return "Jpeg";

    case LIBMTP_FILETYPE_JFIF:
      return "Jfif";

    case LIBMTP_FILETYPE_TIFF:
      return "Tiff";

    case LIBMTP_FILETYPE_BMP:
      return "Bmp";

    case LIBMTP_FILETYPE_GIF:
      return "Gif";

    case LIBMTP_FILETYPE_PICT:
      return "Pict";

    case LIBMTP_FILETYPE_PNG:
      return "Png";

    case LIBMTP_FILETYPE_VCALENDAR1:
      return "VCalendar1";

    case LIBMTP_FILETYPE_VCALENDAR2:
      return "VCalendar2";

    case LIBMTP_FILETYPE_VCARD2:
      return "VCard2";

    case LIBMTP_FILETYPE_VCARD3:
      return "VCard3";

    case LIBMTP_FILETYPE_WINDOWSIMAGEFORMAT:
      return "Windows Image";

    case LIBMTP_FILETYPE_WINEXEC:
      return "Exe";

    case LIBMTP_FILETYPE_TEXT:
      return "Txt";

    case LIBMTP_FILETYPE_HTML:
      return "Html";

    case LIBMTP_FILETYPE_FIRMWARE:
      return "IMG- Firmware";

    case LIBMTP_FILETYPE_AAC:
      return "Aac";

    case LIBMTP_FILETYPE_MEDIACARD:
      return "MediaCard";

    case LIBMTP_FILETYPE_FLAC:
      return "Flac";

    case LIBMTP_FILETYPE_MP2:
      return "Mp2";

    case LIBMTP_FILETYPE_M4A:
      return "M4a";

    case LIBMTP_FILETYPE_DOC:
      return "Doc";

    case LIBMTP_FILETYPE_XML:
      return "Xml";

    case LIBMTP_FILETYPE_XLS:
      return "Xls";

    case LIBMTP_FILETYPE_PPT:
      return "Ppt";

    case LIBMTP_FILETYPE_MHT:
      return "Mht";

    case LIBMTP_FILETYPE_JP2:
      return "Jp2";

    case LIBMTP_FILETYPE_JPX:
      return "Jpx";

    case LIBMTP_FILETYPE_UNKNOWN:
          return "Unknown";
    default:
          return "Unknown";
  }
}

}
#endif
