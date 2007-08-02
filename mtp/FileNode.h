//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __FILENODE__
#define __FILENODE__
#include <libmtp.h>
#include <vector>
#include <map>
#include <string>
#include <libmtp.h>
#include <QFileInfo>
#include <QString>
#include <iostream>
#include "types.h"
using namespace std;

class FileNode
{
public:
    FileNode (const QFileInfo& in_file, uint32_t in_parentID)
    {
        _parentId = in_parentID;
        _name = in_file.fileName().toStdString();
        _fileSize = in_file.size();
        _path = in_file.path().toStdString();
        _type = standardizeType(in_file.suffix());
    }

    FileNode(LIBMTP_file_t* in_head, count_t in_height)
    {
            _itemId = in_head->item_id;
            _parentId = in_head->parent_id;
            _name = in_head->filename;
            _fileSize = in_head->filesize;
            _type = typeToString(in_head->filetype).c_str();
            _height = in_height;
    }

    string GetName() const
    {
        return _name;
    }

    string GetPath() const
    {
        return _path;
    }

    string GetType() const
    {
        return _type.toStdString();
    }

    LIBMTP_filetype_t GetMtpType()
    {
        return stringToType();
    }

    static LIBMTP_filetype_t GetMtpType(const QString& in_suffix)
    {
        QString temp = in_suffix.toUpper();
        return stringToType(temp.toStdString());
    }

    static string GetMtpType(LIBMTP_filetype_t in)
    {
        return typeToString(in);
    }


    uint32_t GetParentID() const //need to rename this
    {
        return _parentId;
    }

    const uint64_t GetSize()
    {
        return _fileSize;
    }

    uint32_t GetID() const
    {
        return _itemId;
    }

    //debug
    void Sanity (LIBMTP_filetype_t in_type)
    {
        cout << "received this type: " << typeToString(in_type) << endl;
    }
    
    count_t GetHeight()
    {
        return _height;
    }

    void SetHeight(count_t in_height)
    {
        _height = in_height;
        return;
    }

private:
    count_t _height;
    uint32_t _itemId;
    uint32_t _parentId;
    string   _name;
    QString _type;
    string _path; //only used to transfer..
    uint64_t _fileSize;

    QString GetFileType (const QString& in_str) // standardize the string
    {
        int loc = in_str.lastIndexOf(".");
        if (loc == -1)
            return "Unknown";
        QString s = in_str.right(in_str.size()-(loc+1));
        return s.toUpper();
    }

    QString standardizeType (const QString in)
    {
        LIBMTP_filetype_t thistype = stringToType(((in.toUpper()).toStdString()));
        QString standardType = typeToString(thistype).c_str();
        return standardType;
    }
    
    static string typeToString (LIBMTP_filetype_t in_type)
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


    LIBMTP_filetype_t stringToType()
    {
        if (_type.toUpper() == "UNKNOWN")
            return  LIBMTP_FILETYPE_UNKNOWN;

        if (_type.toUpper() == "WAV")
            return LIBMTP_FILETYPE_WAV;

        if (_type.toUpper() == "MP3")
            return LIBMTP_FILETYPE_MP3;

        if (_type.toUpper() == "WMA")
            return LIBMTP_FILETYPE_WMA;

        if (_type.toUpper() == "OGG")
            return LIBMTP_FILETYPE_OGG;

        if (_type.toUpper() == "AUD")
            return LIBMTP_FILETYPE_AUDIBLE;

        if (_type.toUpper() == "MP4")
            return LIBMTP_FILETYPE_MP4;

        if (_type.toUpper() == "WMV")
            return LIBMTP_FILETYPE_WMV;

        if (_type.toUpper() == "AVI")
            return LIBMTP_FILETYPE_AVI;

        if (_type.toUpper() == "MPEG")
            return LIBMTP_FILETYPE_MPEG;

        if (_type.toUpper() == "MPG")
            return LIBMTP_FILETYPE_MPEG;

        if (_type.toUpper() == "ASF")
            return LIBMTP_FILETYPE_ASF;

        if (_type.toUpper() == "QT")
            return LIBMTP_FILETYPE_QT;

        if (_type.toUpper() == "JPEG" || _type == "JPG")
            return LIBMTP_FILETYPE_JPEG;

        if (_type.toUpper() == "JFIF")
            return LIBMTP_FILETYPE_JFIF;

        if (_type.toUpper() == "BMP")
            return LIBMTP_FILETYPE_BMP;

        if (_type.toUpper() == "GIF")
            return LIBMTP_FILETYPE_GIF;

        if (_type.toUpper() == "PICT")
            return LIBMTP_FILETYPE_PICT;

        if (_type.toUpper() == "PNG")
            return LIBMTP_FILETYPE_PNG;

        if (_type.toUpper() == "EXE")
            return LIBMTP_FILETYPE_WINEXEC;

        if (_type.toUpper() == "TXT")
            return LIBMTP_FILETYPE_TEXT;

        if (_type.toUpper() == "HTML" || _type == "HTM")
            return LIBMTP_FILETYPE_HTML;

        if (_type.toUpper() == "AAC")
            return LIBMTP_FILETYPE_AAC;

        if (_type.toUpper() == "FLAC")
            return LIBMTP_FILETYPE_FLAC;

        if (_type.toUpper() == "MP2")
            return LIBMTP_FILETYPE_MP2;

        if (_type.toUpper() == "M4A")
            return LIBMTP_FILETYPE_M4A;

        if (_type.toUpper() == "XML")
            return LIBMTP_FILETYPE_XML;

        if (_type.toUpper() == "XLS")
            return LIBMTP_FILETYPE_XLS;

        if (_type.toUpper() == "PPT")
            return LIBMTP_FILETYPE_PPT;

        if (_type.toUpper() == "MHT")
            return LIBMTP_FILETYPE_MHT;

        if (_type.toUpper() == "JP2")
            return LIBMTP_FILETYPE_JP2;

        if (_type.toUpper() == "JPX")
            return LIBMTP_FILETYPE_JPX;

        return LIBMTP_FILETYPE_UNKNOWN; //default
    }

    static LIBMTP_filetype_t stringToType(const string& in_type)
    {
        if (in_type == "UNKNOWN")
            return  LIBMTP_FILETYPE_UNKNOWN;

        if (in_type == "WAV")
            return LIBMTP_FILETYPE_WAV;

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

};

#endif
