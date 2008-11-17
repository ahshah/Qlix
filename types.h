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
 *   TODO Move CommandLineOpts to singleton pattern
 */

#ifndef __TYPES__
#define __TYPES__
#include <vector>
#include <string>
#include <libmtp.h>
using namespace std;
typedef vector<LIBMTP_error_t> MTPErrorVector;
typedef unsigned int count_t;
typedef unsigned int index_t;
typedef unsigned char ubyte;


enum MtpObjectType
{
  MtpTrack,
  MtpShadowTrack,
  MtpFile,
  MtpFolder,
  MtpAlbum,
  MtpPlaylist
};

enum QlixProgressType
{
  FreeSpaceUsage,
  TransferAmount
};

struct CommandLineOptions
{
  const bool AutoFixPlaylists;
  const bool AutoFixAlbums;
  const bool SimulateTransfers;
  const bool DebugOutput;

  CommandLineOptions(bool in_AutoFixPlaylists = false, bool in_AutoFixAlbums = false, 
              bool in_SimulateTransfers = false, bool in_DebugOutput = false) :
                                        AutoFixPlaylists(in_AutoFixPlaylists),
                                        AutoFixAlbums(in_AutoFixAlbums),
                                        SimulateTransfers(in_SimulateTransfers),
                                        DebugOutput(in_DebugOutput)
  { }

  CommandLineOptions(const CommandLineOptions& in_opts) :
                                        AutoFixPlaylists(in_opts.AutoFixPlaylists),
                                        AutoFixAlbums(in_opts.AutoFixAlbums),
                                        SimulateTransfers(in_opts.SimulateTransfers),
                                        DebugOutput(in_opts.DebugOutput)
  { }

  CommandLineOptions& operator = (const CommandLineOptions& in_opts)
  {
    (const_cast<bool&> (this->AutoFixPlaylists)) = in_opts.AutoFixPlaylists;
    (const_cast<bool&> (this->AutoFixAlbums)) = in_opts.AutoFixAlbums;
    (const_cast<bool&> (this->SimulateTransfers)) = in_opts.SimulateTransfers;
    (const_cast<bool&> (this->DebugOutput)) = in_opts.DebugOutput;
    return *this;
  }

};

namespace MTP
{
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

  if (in_type == "AVI" || in_type == "SVI")
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
