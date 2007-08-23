#include "MtpDevice.h"
MtpDevice::MtpDevice( LIBMTP_mtpdevice_t* in_device, count_t in_id)
{
    _deviceID = in_id;
    if (!in_device)
        return;
    _device = in_device;
    getModelName();
    getSerialNum();
    getDeviceVersion();
    getSyncPartner();
    getFriendlyName();
    getBatteryLevel();
    GetFolders();
    int ret = LIBMTP_Get_Storage(in_device, 0);
    qDebug() << "Getting storage properties returned: " << ret ;
    GetErrors(_device);
}

MtpDevice::~MtpDevice()
{
    //remember to clear folders
    if (_device)
    {
        qDebug() << "Releasing device" ;
        LIBMTP_Release_Device(_device);
        _device = NULL;
    }
}

count_t MtpDevice::GetID()
{
    return _deviceID;
}

void MtpDevice::DumpInformation ( void )
{ //i don't know why i formated this junk ;P
   qDebug() << "Device serial:" << _serial;
   qDebug() << "Model name:" << _modelName; 
   qDebug() << "Device version:" << _deviceVersion; 
   qDebug() << "Device friendly name:"  << _friendlyName; 
   qDebug() << "Sync partner:" << _syncPartner; 
   qDebug() << "Current battery level:" << (count_t) _curBatteryLevel;
   qDebug() << "Max battery level:"     << (count_t) _maxBatteryLevel;
}

void MtpDevice::GetFolders ( void )
{
    LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
    GetErrors(_device);
    LIBMTP_file_t* files = LIBMTP_Get_Filelisting_With_Callback(_device, NULL, NULL);
    GetErrors(_device);
    _mtpFS = new MtpFS(folders, _device);
    _mtpFS->SetRootName(_friendlyName);
    _mtpFS->InsertFileList(files);
//        _mtpFS->Print();
}

MtpFS* MtpDevice::GetFileSystem ( void )
{
    return _mtpFS;
}

LIBMTP_mtpdevice_t* MtpDevice::rawDevice()
{
    return _device;
}

uint32_t MtpDevice::CreateFolder (const QString& in_FolderName, uint32_t in_parentID, int* newModelIndex)
{
    if (!_device)
    {
        qDebug() << "no device" ;
        return 0;
    }
    DirNode* parent = _mtpFS->GetDirectory(in_parentID);
    qDebug() << "Creating folder- Parent is: " << parent->GetName() ;
    qDebug() << "Creating folder- Parent ID: " << parent->GetID() ;

    char* foldername = strdup(in_FolderName.toUtf8().data());
    LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
    int ret = LIBMTP_Create_Folder(_device, foldername, in_parentID);
    qDebug() << "Created folder: " << ret ;
    for (count_t i=  0; i < 5 && (_mtpFS->GetDirectory(ret) != NULL); i ++)
    {
        qDebug() << "Error device returned an ID thats already in use, retrying up to five times" ;
        LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
        ret = LIBMTP_Create_Folder(_device, foldername, in_parentID);
        qDebug() << "Created folder: " << ret ;
        usleep(400);
    }

    if (_mtpFS->GetDirectory(ret) != NULL)
    {
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        return 0;
    }

    if (ret == 0)
    {
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        return ret;
    }
    (*newModelIndex) = _mtpFS->AddFolder(ret, in_parentID);

    return ret;
}

bool MtpDevice::DeleteObject (uint32_t in_parentID, int in_FileID)
{
    if (!_device)
    {
        qDebug() << "no device" ;
        return false;
    }

    DirNode* parentDir = _mtpFS->GetDirectory(in_parentID);
    if (in_FileID == -1)
    {
        qDebug() << "About to delete folder: " << parentDir->GetName() <<" with ID: " << parentDir->GetID() ;
        int ret = LIBMTP_Delete_Object(_device, in_parentID);
        if (ret != 0)
        {
            LIBMTP_Dump_Errorstack(_device);
            LIBMTP_Clear_Errorstack(_device);
            return false;
        }
        return true;
    }
    
    DirNode* current_dir =  _mtpFS->GetDirectory(in_parentID);
    if (current_dir == NULL)
    {
        qDebug() << "Tried to delete child of a NULL folder! out of sync! " ;
        return false;
    }

    qDebug() << "About to delete file with id: " << in_FileID;

    int ret = LIBMTP_Delete_Object(_device, in_FileID);
   // int ret = 0;
    if (ret != 0)
    {
       LIBMTP_Dump_Errorstack(_device);
       LIBMTP_Clear_Errorstack(_device);
       return false;
    }
    return true;
}


//Messy function
bool MtpDevice::SendTrackToDevice(const QFileInfo& fileinfo,
                                  uint32_t in_parentID)
{
    if (!_device)
    {
        qDebug() << "No device for file transfer" ;
        return false;
    }

    LIBMTP_track_t* newtrack = LIBMTP_new_track_t();
    if (!setupTrackForTransfer(fileinfo.canonicalFilePath(), in_parentID, newtrack))
    {
        qDebug() << "Setup for track failed, attempting to send as a file" ;
        return SendFileToDevice(fileinfo, in_parentID);
    }
    LIBMTP_progressfunc_t staticProgressFunc = MtpFS::ProgressWrapper; 
    QByteArray tempUtfFilepath = fileinfo.canonicalFilePath().toUtf8();
    const char* actualFilePath = tempUtfFilepath.data();
    int ret =LIBMTP_Send_Track_From_File( _device, actualFilePath,
                                         newtrack,
                                         staticProgressFunc, (void*)_mtpFS, newtrack->parent_id);
    if (ret != 0)
    {
        qDebug() << "Failure while transfering:" << ret ;
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        LIBMTP_destroy_track_t(newtrack);
        return false;
    }
    
    uint32_t albumID;
    if (!CreateAlbum(newtrack, &albumID))
        return false;

    UpdateAlbumArt(fileinfo.canonicalPath(), albumID);
    
    DirNode* parent = _mtpFS->GetDirectory(in_parentID);
    FileNode newFile(newtrack, parent->GetFileCount());
    _mtpFS->AddFile(newFile);
    LIBMTP_destroy_track_t(newtrack);
    return true;
}

bool MtpDevice::UpdateAlbumArt (const QString& in_path, uint32_t in_album_id)
{
    QDir search_dir(in_path);
    QFileInfoList children = search_dir.entryInfoList(QDir::Files);

    for (int i =0; i < children.size(); i++)
    {
        QFileInfo temp = children[i];
        if (temp.fileName() == "cover.jpg")
        {
            qDebug() << "Found a cover" ;
            QImage img(temp.canonicalFilePath());
            QImage imgResized(img.scaled(QSize(64,64), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            QByteArray barray;
            QBuffer buffer(&barray);
            buffer.open(QIODevice::WriteOnly);
            imgResized.save(&buffer, "JPEG");

            LIBMTP_filesampledata_t* sample = LIBMTP_new_filesampledata_t();
            sample->width = 64;
            sample->height = 64;
            sample->filetype = LIBMTP_FILETYPE_JPEG;
            sample->size = barray.size();

            char* newbuffer = new char[barray.size()];
            memcpy (newbuffer, barray.data(), barray.size());

            sample->data = newbuffer;

            int ret = LIBMTP_Send_Representative_Sample(_device, in_album_id, sample);
            if (ret != 0)
            {
                LIBMTP_Dump_Errorstack(_device);
                LIBMTP_Clear_Errorstack(_device);
                return false;
            }
            qDebug() << "Sending representative data returned: " << ret;
            LIBMTP_destroy_filesampledata_t (sample);
            return true;
        }
    }
    return true;
}

bool MtpDevice::CreateAlbum(LIBMTP_track_t* in_track, uint32_t* albumID_out)
{
    LIBMTP_album_t* newAlbum = LIBMTP_new_album_t();

    newAlbum->name = strdup(in_track->album);
    newAlbum->artist = strdup(in_track->artist);
    newAlbum->genre = strdup(in_track->genre);

    qDebug()<< "Create album album:" << newAlbum->name;
    qDebug()<< "Create album artist:" << newAlbum->artist;
    qDebug() << "Create album genre:" << newAlbum->genre; 

    QString album = QString::fromUtf8(newAlbum->name);


    LIBMTP_album_t* albums;

    albums= LIBMTP_Get_Album_List(_device);
    while (albums)
    {
        QString currentName(albums->name);
        qDebug() << "Comparing device album["<< currentName <<"] with length:" << currentName.length();
        qDebug() << "with track's album:[" << album << "] with length:" << album.length() ;
        if (currentName == album)
        {
            qDebug() << "album already on player";
            //update the new structure
            count_t trackCount = albums->no_tracks;
            newAlbum->album_id = albums->album_id;
            newAlbum->no_tracks = trackCount+1;

            qDebug() << "updated the new structure";
            //update the trackList array
            uint32_t* trackList = new uint32_t[trackCount+1];
            for (count_t i = 0; i < trackCount; i++)
            {
                *(trackList+i) = *((albums->tracks)+i);
            }
            *(trackList+trackCount) = in_track->item_id;

            newAlbum->tracks = trackList;
            newAlbum->no_tracks= trackCount+1;
            qDebug() << "updated the new tracklist";
            qDebug() << "Album Update ID: " << newAlbum->album_id;
            qDebug() << "Album Update TrackCount: " << newAlbum->no_tracks;
            qDebug() << "Album Update Name: " << newAlbum->name;
            qDebug() << "Album Update Aritst: " << newAlbum->artist;
            qDebug() << "Album Update Genre: " << newAlbum->genre;
            //push update to the device
            int ret = LIBMTP_Update_Album(_device, newAlbum);
            if (ret != 0)
            {
                qDebug() << "Album REPLACMENT FAIL: " << ret ;
                LIBMTP_Dump_Errorstack(_device);
                LIBMTP_Clear_Errorstack(_device);
                LIBMTP_destroy_album_t(newAlbum);
                return false;
            }

            qDebug() << "Updating external id: " << newAlbum->genre;
            (*albumID_out) = newAlbum->album_id;
            LIBMTP_destroy_album_t(newAlbum);
            qDebug() << "about to return";
            return true;
        }
        albums = albums->next;
    }

    qDebug()<< "Album not on device, creating.."<< newAlbum->name ;
    //setup track count
    newAlbum->no_tracks = 1;
    uint32_t* track_array = new uint32_t(in_track->item_id);
    newAlbum->tracks = track_array;

    //update album on device
    int ret = LIBMTP_Create_New_Album(_device, newAlbum, 0);
    if (ret != 0)
    {
        qDebug()<< "Album creation FAILED" << ret ;
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        LIBMTP_destroy_album_t(newAlbum);
        return false;
    }
    qDebug() << "Reported id: " << newAlbum->album_id;

    (*albumID_out) = newAlbum->album_id;
    LIBMTP_destroy_album_t(newAlbum);
    return true; 
}

bool MtpDevice::SendFileToDevice(const QFileInfo& fileinfo,
                                 uint32_t in_parentID)
{
    if (!_device)
    {
        qDebug() << "No device for file transfer" ;
        return false;
    }

    LIBMTP_file_t* mtpfile  = LIBMTP_new_file_t();
    QString fn = fileinfo.fileName();
    char* fnamebuf = strdup(fn.toUtf8().data());

    qDebug() << "filename is: " << fnamebuf ;
    mtpfile->filename = fnamebuf;
    mtpfile->parent_id = in_parentID;

    mtpfile->filesize = fileinfo.size();
    mtpfile->filetype = FileNode::GetMtpType((fileinfo.suffix()));
//    qDebug() << "While sending file, type was set to: " << FileNode::GetMtpType(mtpfile->filetype) ;
    QString wholefilepath = fileinfo.filePath();
//    qDebug() << "filepathis: " << wholefilepath;

    mtpfile->next = NULL;
    LIBMTP_progressfunc_t staticProgressFunc = MtpFS::ProgressWrapper; 

    int ret =LIBMTP_Send_File_From_File( _device, wholefilepath.toUtf8().data(), mtpfile,
                                         staticProgressFunc, (void*)_mtpFS, mtpfile->parent_id);
    qDebug() <<"File transfer returned: " << ret ;
    if (ret != 0)
    {
        qDebug() << "FAIL" << ret ;
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        LIBMTP_destroy_file_t(mtpfile);
        return false;
    }
    DirNode* parent = _mtpFS->GetDirectory(in_parentID);
    FileNode newFile(mtpfile, parent->GetFileCount());
    _mtpFS->AddFile(newFile);
    LIBMTP_destroy_file_t(mtpfile);
    return true;
}

bool MtpDevice::GetFileFromDevice (uint32_t in_ParentID, const QString& target)
{
    if (!_device)
    {
        qDebug() << "No device for file transfer" ;
        return -1;
    }

  //  funcPtr update = _mtpFS->getFunc();
    int ret = LIBMTP_Get_File_To_File( _device, in_ParentID,
                                       target.toUtf8().data(), NULL,NULL);

    if (ret != 0)
    {
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        return false;
    }
    return true;
}


//private:

bool MtpDevice::setupTrackForTransfer(const QString& in_location, uint32_t in_parentID, LIBMTP_track_t* newtrack)
{
    QString unknownString("Unknown");
    QFileInfo fileinfo(in_location);

    QByteArray tempUtfLocation = fileinfo.canonicalFilePath().toUtf8();
    const char* UtfLocation = tempUtfLocation.data();
    TagLib::FileRef tagFile(UtfLocation, true, TagLib::AudioProperties::Accurate);
    //qDebug() <<"File path is: " + loc ; 
    if (tagFile.isNull())
    {
        qDebug() << "Not a recognizable track format" ;
        return false;
    }
    
   // TagLib::String::String actualTitleTemp(tagFile.tag()->title().data(TagLib::String::UTF8), TagLib::String::UTF8); 


 ////////////////////Copy the album
 //
    QString qAlbumTag = TStringToQString(tagFile.tag()->album());
    char* album;
    if (qAlbumTag.isEmpty() || qAlbumTag.toUpper() == "UNKNOWN")
        album = strdup(unknownString.toUtf8().data());
    else
        album = strdup(qAlbumTag.toUtf8().data());

    QString qalbum2 = QString::fromUtf8(album);
    qDebug() << "Album sanity check2: " << qalbum2;
    qDebug() << "Album sanity check3: " << qAlbumTag;

////////////////////Copy the title
    QString qTitleTag = TStringToQString(tagFile.tag()->title());
    char* title;
    if (qTitleTag.isEmpty() || qTitleTag.toUpper() == "UNKNOWN")
        title = strdup(unknownString.toUtf8().data());
    else
        title = strdup(qTitleTag.toUtf8().data());

    QString qtitle2 = QString::fromUtf8(title);
    qDebug() << "Title sanity check2: " << qtitle2;
    qDebug() << "Title sanity check3: " << qTitleTag;

////////////////////Copy the artist
    QString qArtistTag = TStringToQString(tagFile.tag()->artist());
    char* artist;
    if (qArtistTag.isEmpty() || qArtistTag.toUpper() == "UNKNOWN")
        artist = strdup(unknownString.toUtf8().data());
    else
        artist = strdup(qArtistTag.toUtf8().data());

    QString qartist2 = QString::fromUtf8(artist);
    qDebug() << "Artist sanity check2: " << qartist2;
    qDebug() << "Artist sanity check3: " << qArtistTag;

////////////////////Copy the genre
    QString qGenreTag = TStringToQString(tagFile.tag()->genre());
    char* genre;
    if (qGenreTag.isEmpty() || qGenreTag.toUpper() == "UNKNOWN")
        genre =  strdup(unknownString.toUtf8().data());
    else
        genre = strdup(qGenreTag.toUtf8().data());

    QString qgenre2 = QString::fromUtf8(genre);
    qDebug() << "Genre sanity check2: " << qgenre2;
    qDebug() << "Genre sanity check3: " << qGenreTag;

////////////////////Copy the filename
    const char* actualFilename = fileinfo.fileName().toUtf8().data();
    char* filename = strdup(actualFilename);

    newtrack->parent_id = in_parentID;
    newtrack->title = title;
    newtrack->artist = artist;
    newtrack->genre = genre;
    newtrack->album = album;
    newtrack->filename= filename;
    newtrack->tracknumber = tagFile.tag()->track();
    newtrack->duration  = tagFile.audioProperties()->length()*1000;
    newtrack->bitrate   = tagFile.audioProperties()->bitrate();
    newtrack->filesize  = QFile(fileinfo.canonicalFilePath()).size();
    newtrack->filetype = FileNode::GetMtpType((fileinfo.suffix()));
    newtrack->next = NULL;
    return true;
}

DirNode* MtpDevice::GetDirectory(uint32_t type)
{
    if (!_device)
        return NULL;
    return _mtpFS->GetDirectory(type);
}

void MtpDevice::getModelName()
{
   char* temp;
   temp = LIBMTP_Get_Modelname (_device);
   _modelName = temp;
   if (temp)
       delete temp;
    else
       GetErrors(_device); 
}

void MtpDevice::getSerialNum()
{
    char* temp;
    temp = LIBMTP_Get_Serialnumber (_device);
    _serial = temp;
   if (temp)
       delete temp;
    else
       GetErrors(_device); 
    
} 

void MtpDevice::getDeviceVersion() 
{
    char* temp;
    temp = LIBMTP_Get_Deviceversion (_device);
    _deviceVersion = temp;
    if (temp)
       delete temp;
    else
       GetErrors(_device); 

}
void MtpDevice::getFriendlyName()
{
    char* temp;
    temp = LIBMTP_Get_Friendlyname (_device);
    _friendlyName = temp;
    if (temp)
       delete temp;
    else
       GetErrors(_device); 
}

void MtpDevice::getSyncPartner()
{
    return;
}

void MtpDevice::getBatteryLevel()
{
    int ret;
    ret = LIBMTP_Get_Batterylevel (_device, &_curBatteryLevel,
                                       &_maxBatteryLevel );
    if (ret != 0)
    {
       GetErrors(_device); 
    }
}
void MtpDevice::GetErrors (LIBMTP_mtpdevice_t* in_device)
{
    if (!in_device)
        return;
    LIBMTP_error_t* errors = LIBMTP_Get_Errorstack(in_device);
    while (errors)
    {
        QString errQString = errors->error_text;
        qDebug() <<"Qlix bridge error: " << errQString ;
        if (isTerminal(errors->errornumber) )
            exit(1);
        errors = errors->next;
    }
    LIBMTP_Clear_Errorstack(in_device);
}

bool MtpDevice::isTerminal (LIBMTP_error_number_t in_err)
{
    switch (in_err)
    {
        case LIBMTP_ERROR_USB_LAYER:
            return true;

        case LIBMTP_ERROR_MEMORY_ALLOCATION:
            return true;

        case LIBMTP_ERROR_NO_DEVICE_ATTACHED:
            return true;

        case LIBMTP_ERROR_CONNECTING:
            return true;
        default:
            return false;
    }
}

