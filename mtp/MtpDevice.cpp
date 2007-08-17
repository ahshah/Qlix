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
    cout << "Getting storage properties returned: " << ret << endl;
    GetErrors(_device);
}

MtpDevice::~MtpDevice()
{
    //remember to clear folders
    if (_device)
    {
        cout << "Releasing device" << endl;
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
   cout << "Device serial:"         << _serial    << endl << "Model name:" << _modelName << endl << cout << "Device version:" << _deviceVersion << endl << "Device friendly name:"  << _friendlyName << endl << "Sync partner:" << _syncPartner << endl << "Current battery level:" << (count_t) _curBatteryLevel << endl << "Max battery level:"     << (count_t) _maxBatteryLevel << endl;
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

uint32_t MtpDevice::CreateFolder (const string& in_FolderName, uint32_t in_parentID, int* newModelIndex)
{
    if (!_device)
    {
        cout << "no device" << endl;
        return 0;
    }
    DirNode* parent = _mtpFS->GetDirectory(in_parentID);
    cout << "Creating folder- Parent is: " << parent->GetName() << endl;
    cout << "Creating folder- Parent ID: " << parent->GetID() << endl;

    char* foldername = const_cast<char*>(in_FolderName.c_str());
    LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
    int ret = LIBMTP_Create_Folder(_device, foldername, in_parentID);
    cout << "Created folder: " << ret << endl;
    for (count_t i=  0; i < 5 && (_mtpFS->GetDirectory(ret) != NULL); i ++)
    {
        cout << "Error device returned an ID thats already in use, retrying up to five times" << endl;
        LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
        ret = LIBMTP_Create_Folder(_device, foldername, in_parentID);
        cout << "Created folder: " << ret << endl;
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
        cout << "no device" << endl;
        return false;
    }

    DirNode* parentDir = _mtpFS->GetDirectory(in_parentID);
    if (in_FileID == -1)
    {
        cout << "About to delete folder: " << parentDir->GetName() <<" with ID: " << parentDir->GetID() << endl;
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
        cout << "Tried to delete child of a NULL folder! out of sync! " << endl;
        return false;
    }

    cout << "About to delete file with id: " << in_FileID<< endl;

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
        cout << "No device for file transfer" << endl;
        return false;
    }

    LIBMTP_track_t* newtrack = LIBMTP_new_track_t();
    if (!setupTrackForTransfer(fileinfo.canonicalFilePath(), in_parentID, newtrack))
    {
        cout << "Setup for track failed, attempting to send as a file" << endl;
        return SendFileToDevice(fileinfo, in_parentID);
    }
    LIBMTP_progressfunc_t staticProgressFunc = MtpFS::ProgressWrapper; 
    string filepath = fileinfo.canonicalFilePath().toStdString();
    int ret =LIBMTP_Send_Track_From_File( _device, filepath.c_str(),
                                         newtrack,
                                         staticProgressFunc, (void*)_mtpFS, newtrack->parent_id);
    if (ret != 0)
    {
        cout << "Failure while transfering:" << ret << endl;
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
            qDebug() << "Found a cover" << endl;
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
    QString artist(in_track->artist);
    QString album(in_track->album);
    QString genre(in_track->genre);
    const char* artistCString = artist.toUtf8().constData();
    const char* albumCString  = album.toUtf8().constData();
    const char* genreCString= genre.toUtf8().constData();

    count_t  albumNameLength  = album.toUtf8().size();
    count_t  artistNameLength = artist.toUtf8().size();
    count_t  genreNameLength  = genre.toUtf8().size();



    LIBMTP_album_t* newAlbum = LIBMTP_new_album_t();
    //setup albumname
    char* albumName = new char[albumNameLength+1];
    memcpy(albumName, albumCString, albumNameLength);
    albumName[albumNameLength] = '\0';
    newAlbum->name = albumName;

    //setup albumartist
    char* artistName = new char[artistNameLength+1];
    memcpy(artistName, artistCString, artistNameLength);
    artistName[artistNameLength] = '\0';
    newAlbum->artist = artistName;

    char* genreName= new char[genreNameLength+1];
    memcpy(genreName, genreCString, genreNameLength);
    genreName[genreNameLength] = '\0';
    newAlbum->genre = genreName;

    LIBMTP_album_t* albums;
    LIBMTP_album_t* albumSanity;
    LIBMTP_album_t* backups;
/*
    for (int i = 0; i < 10; i ++)
    {
        qDebug() << "Pass: " << i << "=============" << endl;
        albums = LIBMTP_Get_Album_List(_device);
        albumSanity = albums;
        while (albumSanity)
        {
            qDebug() << "Device album: " << albumSanity->name << endl;
            albumSanity = albumSanity->next;
        }
        qDebug() << "End of Pass: " << i << "=============" << endl;
        sleep(1);
    }
    */

    albums= LIBMTP_Get_Album_List(_device);
    albumSanity = albums;
    backups = albums;

    if (!albumSanity)
        qDebug() << "No albums on device";

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
                cout << "Album REPLACMENT FAIL: " << ret << endl;
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

    qDebug()<< "Album not on device, creating.."<< newAlbum->name << endl;
    //setup track count
    newAlbum->no_tracks = 1;
    uint32_t* track_array = new uint32_t(in_track->item_id);
    newAlbum->tracks = track_array;

    //update album on device
    int ret = LIBMTP_Create_New_Album(_device, newAlbum, 0);
    if (ret != 0)
    {
        qDebug()<< "Album creation FAILED" << ret << endl;
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
        cout << "No device for file transfer" << endl;
        return false;
    }

    //const char* filepath = (file.filePath().toStdString()).c_str();
    LIBMTP_file_t* mtpfile  = LIBMTP_new_file_t();

    string fn = fileinfo.fileName().toStdString();
    char* fnamebuf = new char[fn.length()+1];


    for (count_t i =0; i < fn.length(); i++)
    {
        fnamebuf[i] = fn[i];
    }
    fnamebuf[fn.length()] = '\0';

    cout << "filename is: " << fnamebuf << endl;

    mtpfile->filename = fnamebuf;
    mtpfile->parent_id = in_parentID;

    mtpfile->filesize = fileinfo.size();
    mtpfile->filetype = FileNode::GetMtpType((fileinfo.suffix()));
//    cout << "While sending file, type was set to: " << FileNode::GetMtpType(mtpfile->filetype) << endl;
    string wholefilepath = fileinfo.filePath().toStdString();
//    cout << "filepathis: " << wholefilepath<< endl;

    mtpfile->next = NULL;
    LIBMTP_progressfunc_t staticProgressFunc = MtpFS::ProgressWrapper; 

    int ret =LIBMTP_Send_File_From_File( _device, wholefilepath.c_str(), mtpfile,
                                         staticProgressFunc, (void*)_mtpFS, mtpfile->parent_id);
    cout <<"File transfer returned: " << ret << endl;
    if (ret != 0)
    {
        cout << "FAIL" << ret << endl;
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

bool MtpDevice::GetFileFromDevice (uint32_t in_ParentID, const string& target)
{
    if (!_device)
    {
        cout << "No device for file transfer" << endl;
        return -1;
    }

  //  funcPtr update = _mtpFS->getFunc();
    int ret = LIBMTP_Get_File_To_File( _device, in_ParentID,
                                       target.c_str(), NULL,NULL);

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
    string loc = in_location.toStdString();
    QFileInfo fileinfo(loc.c_str());
    TagLib::FileRef tagFile(loc.c_str());
    //cout <<"File path is: " + loc << endl; 
    if (tagFile.isNull())
    {
        cout << "Not a recognizable track format" << endl;
        return false;
    }
//    cout << "Sending track, filename is: " << tagFile.file()->name() << endl;
    
    string temp;
    int titleLength     = tagFile.tag()->title().size();
    temp = tagFile.tag()->title().to8Bit();
    
    TagLib::String::String actualTitleTemp(tagFile.tag()->title().data(TagLib::String::UTF8), TagLib::String::UTF8); 
    const char* actualTitle    = actualTitleTemp.toCString();
    
    char* title =    new char[titleLength+1];
    memcpy(title, actualTitle, titleLength+1);

 //   cout << "TagLib title: " << title << endl;;

    int artistLength         = tagFile.tag()->artist().size();

    const char* actualArtist = tagFile.tag()->artist().to8Bit().c_str();

    char* artist =   new char[artistLength+1];
    memcpy(artist, actualArtist, artistLength+1);
  //  cout << "TagLib Artist: " << artist << endl;;

    int genreLength          = tagFile.tag()->genre().size();
    char* genre =    new char[genreLength+1];

    const char* actualGenre = tagFile.tag()->genre().to8Bit().c_str();

    memcpy(genre, actualGenre, genreLength+1);
//    cout << "TagLib genre: " << genre << endl;;

    int albumLength          = tagFile.tag()->album().size();
    const char* actualAlbum= tagFile.tag()->album().to8Bit().c_str();
    char* album =    new char[albumLength+1];
    memcpy(album, actualAlbum, albumLength+1);
//    cout << "TagLib album: " << album<< endl;;

    int filenameLength         = fileinfo.fileName().toStdString().size();
    const char* actualFilename = fileinfo.fileName().toStdString().c_str();

    char* filename = new char[filenameLength+1];
    memcpy(filename, actualFilename, filenameLength+1);

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
        string errstring = errors->error_text;
        cout <<"Qlix bridge error: " << errstring << endl;
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

