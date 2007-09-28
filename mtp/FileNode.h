//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __FILENODE__
#define __FILENODE__
#include <libmtp.h>
#include <vector>
#include <map>
#include <libmtp.h>
#include <QFileInfo>
#include <QString>
#include <iostream>
#include <QtDebug>
#include "types.h"
using namespace std;

class FileNode
{
public:
    FileNode (const QFileInfo& in_file, uint32_t in_parentID);
    FileNode(LIBMTP_file_t* in_head, count_t in_height);
    FileNode(LIBMTP_track_t* in_head, count_t in_height);

    QString GetFileName() const;
    QString GetPath() const;
    QString GetType() const;

    LIBMTP_filetype_t GetMtpType();
    static LIBMTP_filetype_t GetMtpType(const QString& in_suffix);
    static QString GetMtpType(LIBMTP_filetype_t in);

    //should i rename this? 
    uint32_t GetParentID() const;
    uint64_t GetSize() const;
    uint32_t GetID() const;
    //debug
    void Sanity (LIBMTP_filetype_t in_type) const;
    count_t GetHeight() const;
    void SetHeight(count_t in_height);

private:
    count_t _height;
    uint32_t _itemId;
    uint32_t _parentId;
    uint64_t _fileSize;
    
    QString _deviceID;
    QString _name;
    QString _type;
    //only used to transfer..
    QString _path;

    //extended Metadata
    bool _extendedAvailable;
    QString _artist;
    QString _album;
    QString _track;
    QString _genre;
    QString _date;
    count_t _trackNum;
    count_t _duration;
    count_t _bitrate;
    count_t _bitrateType;
    count_t _rating;

    void setupDefaultMetaData();
    QString GetFileType (const QString& in_str); 
    QString standardizeType (const QString in);
    LIBMTP_filetype_t stringToType();;

    static QString typeToString (LIBMTP_filetype_t in_type);;
    static LIBMTP_filetype_t stringToType(const QString& in_type);;
};

#endif
