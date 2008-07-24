#include "widgets/QMtpDevice.h"
//TODO retrieving files with NULL filenames
//TODO memory leaks
//TODO update model correctly
//TODO move model information out of this thread and back into the main thread
//TODO should the model just use queuedConnections for removing and 
//     adding tracks/files/objects

QMtpDevice::QMtpDevice(MtpDevice* in_device, MtpWatchDog* in_watchDog, 
                       QObject* parent):
                       QThread(parent),
                       _device(in_device),
                       _watchDog(in_watchDog),
                       _icon(QPixmap(":/pixmaps/miscDev.png"))
{ 
  _device->SetProgressFunction(progressWrapper, this);
}

QString QMtpDevice::Name() { return  _name; }
QString QMtpDevice::Serial() { return  _serial; }
QIcon QMtpDevice::Icon() { return _icon; }

void QMtpDevice::IssueCommand(GenericCommand* in_cmd)
{
  QMutexLocker locker(&_jobLock);
  _jobs.push_back(in_cmd);
  _noJobsCondition.wakeOne();
  qDebug() << "Issued command";
}

void QMtpDevice::run()
{
  lockusb();
  initializeDeviceStructures();
  findAndRetrieveDeviceIcon();
  unlockusb();
  emit Initialized(this);

  while (true)
  {
    _jobLock.lock();
    while (_jobs.empty() )
      _noJobsCondition.wait(&_jobLock); 

    GenericCommand* currentJob = _jobs.dequeue();
    _jobLock.unlock();
    proccessJob(currentJob);
  }
}
void QMtpDevice::proccessJob(GenericCommand* currentJob)
{
  CommandCode type = currentJob->GetCommand();
  switch (type)
  {
    case Initialize:
    {
      delete currentJob;  
      break;
    }
    case SendFile:
    {
      SendFileCmd* sendCmd = (SendFileCmd*)currentJob;
      QString fullpath = sendCmd->Path;

      TagLib::FileRef tagFile(fullpath.toUtf8().data(), true,
                        TagLib::AudioProperties::Accurate);
      if (tagFile.isNull()) 
      {
        emit NotATrack(sendCmd);
        break;
      }
      qDebug() << "Syncing track with path: " << fullpath;
      syncTrack(tagFile, sendCmd->ParentID);
      delete sendCmd;
      break;
    }
    case GetObj:
    {
      GetObjCmd* getObj = (GetObjCmd*) currentJob;
      bool ret = _device->Fetch(getObj->ID,  getObj->Path.toUtf8().data());
      if (!ret)
        qDebug() << "Fetch failure!" << endl;
      delete getObj;
      //TODO update the model and delete memory
      break;
    }
    case CreateFSFolder:
    {
      CreateFSFolderCmd* createFolder = (CreateFSFolderCmd*) currentJob;
      qDebug() << "Got a create fs folder command path:" << createFolder->Path
      << " name: " << createFolder->Name;
      QDir temp(createFolder->Path);
      temp.mkdir(createFolder->Name);
      QFileInfo check (temp, createFolder->Name);
      bool ret = (check.exists() && check.isDir());
      if (ret)
        qDebug() << "Success creating folder";
      else
        qDebug() << "Failed creating folder";
      delete createFolder;
      break;
      //TODO update the model 
    }
    case Delete:
    {
      DeleteObjCmd* deleteThis = (DeleteObjCmd*)currentJob;
      qDebug() << "Whether the filter supports changes: " <<_sortedAlbums->dynamicSortFilter();
      deleteObject(deleteThis->Object);
      delete deleteThis;
      //_albumModel->Delete(deleteThis->Object);
//      _sortedAlbums->invalidate();
      break;
    }
  }
}

void QMtpDevice::Progress(uint64_t const sent, uint64_t total)
{
  float percent = (float)sent/(float)total;
  count_t per = (count_t) (percent*100);
  QString blank = "";
  emit UpdateProgress(blank, per);
}

void QMtpDevice::lockusb()
{
  assert(_watchDog);
  _watchDog->Lock();
}

void QMtpDevice::unlockusb()
{
  assert(_watchDog);
  _watchDog->Unlock();
}

/*
 * Initializes the base device which retrieves the device friendly name etc
 */
void QMtpDevice::initializeDeviceStructures()
{
  if (!_device)
    return;
  _device->Initialize();
  _name = QString::fromUtf8(_device->Name()); 
  _serial = QString::fromUtf8(_device->SerialNumber());
#ifdef QLIX_DEBUG
//  qDebug() << "Discovered name to be: " << _name;
#endif
  _albumModel = new AlbumModel(_device);
  //new ModelTest(_albumModel);
  _dirModel = new DirModel(_device);
  new ModelTest(_dirModel);
  _plModel = new PlaylistModel(_device);

  _sortedAlbums = new QSortFilterProxyModel();
  connect(_albumModel, SIGNAL(layoutChanged()),
          _sortedAlbums, SLOT(invalidate()));
  _sortedPlaylists = new QSortFilterProxyModel();
  _sortedFiles = new MtpDirSorter();
  _sortedAlbums->setDynamicSortFilter(true);
  _sortedFiles->setDynamicSortFilter(true);
  _sortedPlaylists->setDynamicSortFilter(true);

  _sortedAlbums->setSourceModel(_albumModel);
  _sortedPlaylists->setSourceModel(_plModel);
  _sortedFiles->setSourceModel(_dirModel);

  this->moveToThread(_sortedAlbums->thread());
  _sortedAlbums->moveToThread(QApplication::instance()->thread());
  _albumModel->moveToThread(QApplication::instance()->thread());
  _sortedFiles->moveToThread(QApplication::instance()->thread());
  _dirModel->moveToThread(QApplication::instance()->thread());
} 

/*
 * Iterates over all the devices files and tries to find devIcon.fil
 */
void QMtpDevice::findAndRetrieveDeviceIcon()
{
  qDebug() << "Searching for Device Icon";
  count_t fileCount = _device->RootFileCount();
  count_t thread_id = (int)this;
  QString iconPath = QString("/tmp/%1Icon").arg(thread_id); 
  MTP::File* iconFile= NULL;
  for (count_t i = 0; i < fileCount; i++)
  {
    curFile = _device->RootFile(i);
    QString name = QString::fromUtf8(curFile->Name());
    name = name.toLower();
    if (name == "devicon.fil")
    {
      break;
      iconFile = curFile;
    }
  }
  if (iconFile)
  {
    QPixmap image;
    _device->Fetch(iconFile->ID(), iconPath.toLatin1());
    QFile img_file(iconPath);
    if (img_file.exists())
    {
      img_file.open(QFile::ReadOnly);
      QByteArray buffer = img_file.readAll();
      DeviceIcon devIcon(buffer);
      if (devIcon.IsValid())
      {
          size_t temp = devIcon.GetBestImageSize();
          char buf[temp];
          devIcon.Extract(buf);
          Dimensions dim = devIcon.GetDimensions();
          QImage tempImage( (uchar*)buf, dim.Width, dim.Height, QImage::Format_ARGB32);
          image = (QPixmap::fromImage(tempImage));
      }
      _icon = QIcon(QPixmap(image));
    }
    else
      _icon = QIcon(QPixmap(":/pixmaps/miscDev.png"));
  }
}

/*
 * Returns the sorted AlbumModel
 */
QSortFilterProxyModel* QMtpDevice::GetAlbumModel() const
{
  return _sortedAlbums;
}
/*
 * Returns the sorted DirModel 
 */
QSortFilterProxyModel* QMtpDevice::GetDirModel() const
{
  return _sortedFiles;
}

/*
 * Returns the sorted PlaylistModel 
 */
QSortFilterProxyModel* QMtpDevice::GetPlaylistModel() const
{
  return _sortedPlaylists;
}
/**
 * Issues a command to initiate the transfer of a track
 * @param in_path the path to the track on the filesystem
 */
void QMtpDevice::TransferTrack(QString inpath) 
{
  QFileInfo file(inpath);
  if (file.isDir())
    return;

  SendFileCmd* cmd= new SendFileCmd(inpath, 0, true);
  IssueCommand(cmd);
  qDebug() << "Attempting to transfer file: " << inpath;
}

void QMtpDevice::DeleteObject(MTP::GenericObject* obj)
{
  DeleteObjCmd* cmd; // = new DeleteObjCmd(obj->ID());
  switch(obj->Type())
  {
    case MtpTrack:
    case MtpFile:
    {
      cmd = new DeleteObjCmd(obj);
      IssueCommand(cmd);
      break;
    }
    case MtpAlbum:
    { 
      MTP::Album* album = (MTP::Album*) obj;
      MTP::Track* currentTrack;
      for (count_t i = 0; i < album->TrackCount(); i++)
      {
        currentTrack = album->ChildTrack(i);
        cmd = new DeleteObjCmd(currentTrack);
        IssueCommand(cmd);
      }
      cmd = new DeleteObjCmd(album);
      IssueCommand(cmd);
      break;
    }
    case MtpPlaylist:
    {
      MTP::Playlist* pl = (MTP::Playlist*) obj;
      MTP::Track* currentTrack;
      for (count_t i = 0; i < pl->TrackCount(); i++)
      {
        currentTrack = pl->ChildTrack(i);
        cmd = new DeleteObjCmd(currentTrack);
      }
      cmd = new DeleteObjCmd(pl);
      IssueCommand(cmd);
      break;
    }

    case MtpFolder:
    {
      MTP::Folder* rootFolder = (MTP::Folder*)obj;
      MTP::File* subFile;
      MTP::Folder* subFolder;

      for (count_t i =0; i < rootFolder->FileCount(); i++)
      {
        subFile = rootFolder->ChildFile(i);
        cmd = new DeleteObjCmd(subFile);
        IssueCommand(cmd);
      }
      //recurse on all subfolders
      for (count_t i =0; i < rootFolder->FolderCount(); i++)
      {
        subFolder = rootFolder->ChildFolder(i);
        DeleteObject(subFolder);
      }
      //delete the rootfolder
      cmd = new DeleteObjCmd(rootFolder);
      IssueCommand(cmd);
      break;
    }
  }
}

/**
 * Transfers an object from device the passed location
 * @param obj the object to transfer
 * @param filePath 
*/
void QMtpDevice::TransferFrom(MTP::GenericObject* obj, QString filePath)
{
  GetObjCmd* cmd;
  switch (obj->Type())
  {
    case MtpTrack:
    case MtpFile:
    {
      QString filename;
      if (obj->Type() == MtpTrack)
      {
        MTP::Track* currentTrack= (MTP::Track*) obj;
        filename= QString::fromUtf8(currentTrack->FileName());
      }
      else
      {
        MTP::File* currentFile= (MTP::File*) obj;
        filename= QString::fromUtf8(currentFile->Name());
      }
      qDebug() << "check here: " << filePath;
      qDebug() << "and here: " << filename;
      filePath += QDir::separator() + filename;
      qDebug() << "Transfer here: " << filePath;
      qDebug() << "Or here: " << QDir::toNativeSeparators(filePath);
      cmd = new GetObjCmd(obj->ID(), filePath);
      IssueCommand(cmd);
      break;
    }
    case MtpAlbum:
    { 
      MTP::Album* album = (MTP::Album*) obj;
      MTP::Track* currentTrack;

      QDir albumDir(filePath);
      QString albumName = QString::fromUtf8(album->Name());
      bool albumDirCreated = albumDir.mkdir(albumName);
      if (albumDirCreated)
        filePath = filePath + QDir::separator() + albumName;
      for (count_t i = 0; i < album->TrackCount(); i++)
      {
        currentTrack = album->ChildTrack(i);
        QString trackName = QString::fromUtf8(currentTrack->FileName());
        QString actualPath = filePath + QDir::separator() + trackName;
        cmd = new GetObjCmd (currentTrack->ID(), actualPath);
        IssueCommand(cmd);
      }
      LIBMTP_filesampledata_t cover = album->SampleData();
      if (cover.size == 0)
        break;

      QImage img;
      img.loadFromData( (const uchar*) cover.data, cover.size);
      QString coverName = albumName + ".jpg";
      QString coverPath;
      //no need to check albumDirCreated again, the path already holds the
      //right directory
        coverPath = filePath + QDir::separator() + coverName;

      img.save(coverPath);
      break;
    }
    case MtpPlaylist:
    {
      MTP::Playlist* pl = (MTP::Playlist*) obj;
      MTP::Track* currentTrack;
      for (count_t i = 0; i < pl->TrackCount(); i++)
      {
        currentTrack = pl->ChildTrack(i);
        QString trackName = QString::fromUtf8(currentTrack->FileName());
        QString actualPath = filePath + QDir::separator() + trackName;
        cmd = new GetObjCmd (currentTrack->ID(), actualPath);
        IssueCommand(cmd);
      }
      break;
    }
    case MtpFolder:
    {
      MTP::Folder* rootFolder = (MTP::Folder*)obj;
      MTP::File* currentFile;
      MTP::Folder* currentFolder;

      QString folderName = QString::fromUtf8(rootFolder->Name());
      CreateFSFolderCmd* newFolderCmd = new CreateFSFolderCmd
                                            (filePath, folderName); 
      IssueCommand(newFolderCmd);

      QString subFolderPath = filePath + QDir::separator() + folderName;
      for (count_t i =0; i < rootFolder->FileCount(); i++)
      {
        currentFile = rootFolder->ChildFile(i);
        QString subFilePath = subFolderPath + QDir::separator() + 
                              QString::fromUtf8(currentFile->Name());
        cmd = new GetObjCmd (currentFile->ID(), subFilePath);
        IssueCommand(cmd);
      }
      //recurse on all subfolders
      for (count_t i =0; i < rootFolder->FolderCount(); i++)
      {
        currentFolder = rootFolder->ChildFolder(i);
        TransferFrom(currentFolder, subFolderPath);
      }
      break;
    }

    default:
    {
      assert(false);
      break;
    }
  }
}

int QMtpDevice::progressWrapper(uint64_t const sent, uint64_t const total, const void* const data)
{
//  cout << "Progress: " << (float)sent/(float)total << endl;
  if (!data)
    return 1;
  QMtpDevice const * const tempDevice = static_cast<const QMtpDevice* const> (data);
  QMtpDevice* thisDevice = const_cast<QMtpDevice* const> (tempDevice);
  thisDevice->Progress(sent, total);
  return 0;
}

/**
 * Probes the device for free space
 * @param out_total the total amount of space on the device
 * @param out_free the total amount of free space on the device
 */
void QMtpDevice::FreeSpace(uint64_t* out_total , uint64_t* out_free) 
{
  _device->FreeSpace(SelectedStorage(), out_total, out_free);
}

/**
 * Sets the default storage ID for all transfer and create requests that are
 * made to the device
 * @param in_storageID the storage ID that will be the default
 */
void QMtpDevice::SetSelectedStorage(count_t in_storageID)
{
  _storageID = in_storageID;
  qDebug() << "Selected storage: " << _storageID;
}

/**
 * @return the storage id that was selected for this view
 */
unsigned int QMtpDevice::SelectedStorage()
{
  return _storageID;
}

/** 
 * @return the number of storage devices that exist on this MTP device
 */
unsigned int QMtpDevice::StorageDeviceCount()
{
  return _device->StorageDeviceCount();
}

/**
 * @param in_idx the index of requested storage device if the index is out of 
 * bounds, this function returns NULL
 * @return the requested storage device by index
 */
MtpStorage* QMtpDevice::StorageDevice(unsigned int in_idx)
{
  return _device->StorageDevice(in_idx);
}

/**
 * This function will sync a track to the device by looking up the track's
 * metadata and calling the TransferTrack function on the device.
 * @param in_path the path to the track on the host computer's filesystem
 * @param parent the parent id of this track, this should be the id of a
 *               folder where this track will reside in
 */
void QMtpDevice::syncTrack(TagLib::FileRef tagFile, uint32_t parent)
{
  QString filePath = tagFile.file()->name();
  QFileInfo file(filePath);
  QString suffixStr = file.suffix().toUpper();
  char* suffix = strdup(suffixStr.toUtf8().data());
  QString filename = file.fileName();
  uint64_t size = (uint64_t) file.size();
  LIBMTP_filetype_t type = MTP::StringToType(suffix);
  delete suffix;

  MTP::Track* newTrack;
  newTrack = SetupTrackTransfer(tagFile, filename, size,
                                         parent, type);
  assert(newTrack);

  if (! _device->TransferTrack(filePath.toUtf8().data(), newTrack) )
  {
    qDebug() << "Transfer track failed.. ";
    return;
  }

  MTP::Album* trackAlbum = NULL;
  QString findThisAlbum = QString::fromUtf8(newTrack->AlbumName());
  for (int i = 0; i < _albumModel->rowCount(); i++)
  {
    QModelIndex idx = _albumModel->index(i, 0, QModelIndex());
    MTP::Album* album = (MTP::Album*) idx.internalPointer();
    if (QString::fromUtf8(album->Name()) == findThisAlbum)
    {
      trackAlbum = album;
      break;
    }
  }
  // if there is no trackAlbum then we have to create one
  // We start by creating a new album, finding appropriete cover art  and 
  // finally we apply the cover art to the newly created album
  if (!trackAlbum)
  {
    //first try adding a new album to the device because one does not exist..
    if(!_device->NewAlbum(newTrack, SelectedStorage(), &trackAlbum))
    {
      qDebug() << "Failed to create new album";
      return;
    }
    //Try and find some cover art
    QFileInfo cover;
    bool ret = discoverCoverArt(filePath, 
                                QString::fromUtf8(trackAlbum->Name()),
                                &cover);
    qDebug() << "Found cover art!";
    LIBMTP_filesampledata_t* sample = _device->DefaultJPEGSample();
    if (ret && sample != NULL)
    {
      qDebug() << "LIBMTP reported valid sample data";
      count_t width = sample->width;
      count_t height = sample->height;
      if (height > width)
        height = width;
      else
        width = height;
      QImage img(cover.canonicalFilePath());
      img = img.scaled( QSize(width, height), Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
      QByteArray barray;
      QBuffer buffer(&barray);
      buffer.open(QIODevice::WriteOnly);
      img.save(&buffer, "JPEG");
      sample->filetype = LIBMTP_FILETYPE_JPEG;
      sample->size = barray.size();
      sample->width = width;
      sample->height = height;
      char* newBuffer = new char[barray.size()];
      memcpy(newBuffer, barray.data(), barray.size());
      sample->data = newBuffer;
      _device->UpdateAlbumArt(trackAlbum, sample);
    }
    else if (sample == NULL)
    {
      qDebug() << "LIBMTP reported invalid sample data for this device please report"
               << "this";
    }

    trackAlbum->SetInitialized();

    //if thats successful we can update the view with the new album
    emit CreatedAlbum(trackAlbum);
  }
    
  //now add the track to the found album and update it on the device
  if (!_device->AddTrackToAlbum(newTrack, trackAlbum))
  {
    qDebug() << "Failed to add track to album";
    return;
  }
  newTrack->SetParentAlbum(trackAlbum);
  qDebug() << "About to emit AddedTrackToAlbum thread id is: " << currentThread();
  emit AddedTrackToAlbum(newTrack);
  return ;
}

/**
 * This function will sync a file to the device
 * @param in_path the path to the file on the host computer's filesystem
 * @param parent the parent id of this file, this should be the id of a
 *               folder
 */
bool QMtpDevice::syncFile(const QString& in_path, uint32_t parent)
{
  QFileInfo file(in_path);
  QString suffixStr = file.suffix().toUpper();

  char* suffix = suffixStr.toUtf8().data();
  char* filename = file.completeBaseName().toLocal8Bit().data();
  uint64_t size = (uint64_t) file.size();
  LIBMTP_filetype_t type = MTP::StringToType(suffix);

  MTP::File* newFile = SetupFileTransfer(filename, size, parent,
                                                  type); 
  return _device->TransferFile((const char*) in_path.toUtf8().data(), 
                                newFile);
}

/**
 * This function creates a MTP::File object filled with sane values
 * @param in_filename the name of the file
 * @param in_si the size of the file
 * @param in_parentid the parent folder of this object, if its 0, its on the
 *        root level
 * @param in_type the LIBMTP_filetype_t of file
 */
MTP::File* QMtpDevice::SetupFileTransfer(const char* in_filename, 
                                         uint64_t in_sz, 
                                         count_t in_parentid, 
                                         LIBMTP_filetype_t in_type)
{
  LIBMTP_file_t* file = LIBMTP_new_file_t();
  file->filename = strdup(in_filename);
  file->filesize = in_sz;
  file->storage_id = SelectedStorage();
  file->parent_id = in_parentid;
  file->filetype = in_type;
  return new MTP::File(file);
}


MTP::Track* QMtpDevice::SetupTrackTransfer(TagLib::FileRef tagFile,
                                           const QString& in_filename,
                                           uint64_t in_size,
                                           uint32_t in_parentID, 
                                           LIBMTP_filetype_t in_type)
{
    TagLib::String unknownString = "Unknown";
    //Copy the album
    TagLib::String albumTag = tagFile.tag()->album();
    char* album;
    if (albumTag.isEmpty() || albumTag.upper() == TagLib::String("UNKNOWN"))
        album = strdup(unknownString.toCString(true));
    else
        album = strdup(albumTag.toCString(true));
    cout << "Album sanity check: " << album << endl;

    //Copy the title
    char* title;
    TagLib::String titleTag = tagFile.tag()->title();
    if (titleTag.isEmpty() || titleTag.upper() == TagLib::String("UNKNOWN"))
        title = strdup(unknownString.toCString(true));
    else
        title = strdup(titleTag.toCString(true));
    cout << "Title sanity check: " << title << endl;

    //Copy the artist
    char* artist;
    TagLib::String artistTag = tagFile.tag()->artist();
    if (artistTag.isEmpty() || artistTag.upper() == TagLib::String("UNKNOWN"))
        artist = strdup(unknownString.toCString(true));
    else
        artist = strdup(artistTag.toCString(true));
    cout << "Artist sanity check: " <<  artist << endl;

    //Copy the genre
    TagLib::String genreTag = tagFile.tag()->genre();
    char* genre;
    if (genreTag.isEmpty() || genreTag.upper() == TagLib::String("UNKNOWN"))
        genre =  strdup(unknownString.toCString(true));
    else
        genre = strdup(genreTag.toCString(true));
    cout<< "Genre sanity check: " << genre << endl;

    //Copy the filename
    //TODO why doesn't this work?
    char* filename = strdup(in_filename.toLocal8Bit().data());
    cout << "Filename sanity check: " << filename << endl;
    LIBMTP_track_t* newtrack = LIBMTP_new_track_t();
    cout << "File type sanity check: " << MTP::TypeToString(in_type) << endl;

    newtrack->parent_id = in_parentID;
    newtrack->title = title;
    newtrack->storage_id = SelectedStorage();
    newtrack->artist = artist;
    newtrack->genre = genre;
    newtrack->album = album;
    newtrack->filename= filename;
    newtrack->tracknumber = tagFile.tag()->track();
    newtrack->duration  = tagFile.audioProperties()->length()*1000;
    newtrack->bitrate   = tagFile.audioProperties()->bitrate();
    newtrack->filesize  = in_size;
    newtrack->filetype = in_type;
    newtrack->next = NULL;
    return new MTP::Track(newtrack);
}


bool QMtpDevice::discoverCoverArt(const QString& in_path,
                                  const QString& in_albumName,
                                  QFileInfo* outFile)
{
  QFileInfo finfo(in_path);
  QDir search_dir;
  if (finfo.isFile())
    search_dir = finfo.dir();
  else
    search_dir = QDir(in_path);

  QFileInfoList children = search_dir.entryInfoList(QDir::Files);
  QString albumName = in_albumName + ".jpg";
  QString albumNameAlt= in_albumName +".jpeg";

  while (!children.isEmpty())
  {
    QFileInfo temp = children.front();
    QString name= temp.fileName().toLower();
    if (name == "cover.jpg" || name == "cover.jpeg" ||
        name == albumName.toLower() ||
        name == albumNameAlt.toLower() ||  name == "folder.jpg" ||
        name == "folder.jpeg" || name == "album art.jpg" || 
        name == "album art.jpeg" || name == "albumart.jpg" ||
        name == "albumart.jpeg")
    {
      (*outFile) = temp;
        //find out the default image size
      return true;
    }
    children.pop_front();
  }
  return false;
}

void QMtpDevice::deleteObject(MTP::GenericObject* in_obj)
{
  switch (in_obj->Type())
  {
    case MtpFile:
    {
      break;
    }

    case MtpTrack:
    {
      _device->RemoveTrack( (MTP::Track*)in_obj);
      emit RemovedTrack((MTP::Track*)in_obj);
      //TODO emit removedFile here
      break;
    }

    case MtpFolder:
    {
      qDebug() << "About to emit folder removal: thread id is: " << currentThread();
      emit RemovedFolder((MTP::Folder*) in_obj);
      break;
    }
    case MtpAlbum:
    {
      _device->RemoveAlbum( (MTP::Album*)in_obj);
      emit RemovedAlbum( (MTP::Album*)in_obj);
      //TODO emit removedFile here
      break;
    }

    case MtpPlaylist:
    {
      break;
    }
    default:
      qDebug() << "Object of unknown type!" << endl;
      assert(false);
  }
}
