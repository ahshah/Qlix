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
 *
 *   TODO retrieving files with NULL filenames
 *   TODO memory leaks
 *   TODO update model correctly
 *   TODO move model information out of this thread and back into the main thread
 *   TODO create signals to report errors
 *   Q. Should the model just use queuedConnections for removing and
 *   adding tracks/files/objects
 *   A. Yes this seems to be the right thing to do..
 *
 */
#include "widgets/QMtpDevice.h"

QMtpDevice::QMtpDevice(MtpDevice* in_device, MtpWatchDog* in_watchDog,
                       QObject* parent):
                       QThread(parent),
                       _device(in_device),
                       _watchDog(in_watchDog),
                       _icon(QImage(":/pixmaps/miscDev.png")),
                       _iconBuf(NULL)
{
  _device->SetProgressFunction(progressWrapper, this);
}

QMtpDevice::~QMtpDevice()
{
  if (_iconBuf)
  {
    delete _iconBuf;
    _iconBuf = NULL;
  }
}

QString QMtpDevice::Name() { return  _name; }
QString QMtpDevice::Serial() { return  _serial; }
QImage QMtpDevice::IconImage() { return _icon; }

void QMtpDevice::IssueCommand(GenericCommand* in_cmd)
{
  QMutexLocker locker(&_jobLock);
  _jobs.push_back(in_cmd);
  _noJobsCondition.wakeOne();
}

void QMtpDevice::run()
{
  lockusb();
  initializeDeviceStructures();
  findAndRetrieveDeviceIcon();
  unlockusb();
  for (count_t i =0; i < _device->StorageDeviceCount(); i++)
  {
    MtpStorage* storage = _device->StorageDevice(i);
    if (storage->ID() == SelectedStorage())
    {
      qDebug() << "Found default storage!" << endl;
      break;
    }

    //if we haven't found a storage device
    if (i+1 >= _device->StorageDeviceCount())
    {
      SetSelectedStorage(_device->StorageDevice(0)->ID());
      qDebug() << "Default storage not found, selecting first";
    }
  }
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
      qDebug() << "Syncing file.. ";
      SendFileCmd* sendCmd = (SendFileCmd*)currentJob;
      QString fullpath = sendCmd->Path;

      qDebug() << "Syncing file with path: " << fullpath;
      TagLib::FileRef tagFile(fullpath.toUtf8().data(), true,
                        TagLib::AudioProperties::Accurate);
      if (tagFile.isNull())
      {
        syncFile(fullpath, sendCmd->ParentFolder);
        delete sendCmd;
        break;
      }

      syncTrack(tagFile, sendCmd->ParentFolder);
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
  _albumModel = new AlbumModel(_device->Albums());
  new ModelTest(_albumModel);
  _dirModel = new DirModel(_device->RootFolder());
  new ModelTest(_dirModel);
  _plModel = new PlaylistModel(_device->Playlists());

  _sortedAlbums = new QSortFilterProxyModel();
  _sortedPlaylists = new QSortFilterProxyModel();
  _sortedFiles = new MtpDirSorter();
  connect(_albumModel, SIGNAL(layoutChanged()),
          _sortedAlbums, SLOT(invalidate()));
  connect(_dirModel, SIGNAL(layoutChanged()),
          _sortedFiles, SLOT(invalidate()));

  _sortedAlbums->setDynamicSortFilter(true);
  _sortedFiles->setDynamicSortFilter(true);
  _sortedPlaylists->setDynamicSortFilter(true);

  _sortedAlbums->setSourceModel(_albumModel);
  _sortedPlaylists->setSourceModel(_plModel);
  _sortedFiles->setSourceModel(_dirModel);

  //this makes no sense:
  //this->moveToThread(_sortedAlbums->thread());
  _sortedAlbums->moveToThread(QApplication::instance()->thread());
  _sortedPlaylists->moveToThread(QApplication::instance()->thread());
  _sortedFiles->moveToThread(QApplication::instance()->thread());

  _plModel->moveToThread(QApplication::instance()->thread());
  _albumModel->moveToThread(QApplication::instance()->thread());
  _dirModel->moveToThread(QApplication::instance()->thread());
}

/*
 * Iterates over all the devices files and tries to find devIcon.fil
 */
void QMtpDevice::findAndRetrieveDeviceIcon()
{
  MTP::Folder* rootFolder = _device->RootFolder();
  count_t fileCount = rootFolder->FileCount();

  QString iconPath = QString("/tmp/QlixDeviceIcon-%1").arg( (int) this);
  MTP::File* iconFile= NULL;
  for (count_t i = 0; i < fileCount; i++)
  {
    MTP::File* curFile = rootFolder->ChildFile(i);
    QString name = QString::fromUtf8(curFile->Name());
    name = name.toLower();
    if (name == "devicon.fil")
    {
      qDebug() << "Found icon file with id: " << curFile->ID();
      iconFile = curFile;
      break;
    }
  }
  if (iconFile)
  {
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
          _iconBuf= new char[temp];
          //unecessary
          memset(_iconBuf, 0, temp);
          devIcon.Extract(_iconBuf);
          Dimensions dim = devIcon.GetDimensions();
          _icon = QImage( (uchar*)_iconBuf, dim.Width, dim.Height, QImage::Format_ARGB32);
//          _icon.save("/tmp/qlix-test.png", "PNG");
      }
    }
    else
      _icon = QImage(":/pixmaps/miscDev.png");
  }
  else
    qDebug() << "No device icon found";
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
 * @param in_parent the parent folder of the transfered file/track
 */
void QMtpDevice::TransferTrack(const QString& inpath, MTP::Folder* parentFolder)
{
  QFileInfo file(inpath);
  if (file.isDir())
    return;

  SendFileCmd* cmd= new SendFileCmd(inpath, parentFolder, true);
  IssueCommand(cmd);
  qDebug() << "Attempting to transfer file: " << inpath;
}

/**
 * This function issues the delete command to the device thread after the user
 * has confirmed the deletion.
 * @param in_obj the object to be deleted
 */
void QMtpDevice::DeleteObject(MTP::GenericObject* in_obj)
{
  if (in_obj->Type() != MtpFile   && in_obj->Type() != MtpTrack  &&
      in_obj->Type() != MtpFolder && in_obj->Type() != MtpAlbum  &&
      in_obj->Type() != MtpPlaylist && in_obj->Type() != MtpShadowTrack)
  {
    qDebug() << "Object of unknown type!" << endl;
    assert(false);
  }

  DeleteObjCmd* cmd; // = new DeleteObjCmd(obj->ID());

  switch(in_obj->Type())
  {
    case MtpTrack:
    case MtpShadowTrack:
    case MtpFile:
    {
      cmd = new DeleteObjCmd(in_obj);
      IssueCommand(cmd);
      break;
    }
    case MtpAlbum:
    {
      MTP::Album* album = (MTP::Album*) in_obj;
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
      MTP::Playlist* pl = (MTP::Playlist*) in_obj;
      MTP::ShadowTrack* currentTrack;
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
      MTP::Folder* rootFolder = (MTP::Folder*)in_obj;
      /*
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
       *
       */
      cmd = new DeleteObjCmd(rootFolder);
      IssueCommand(cmd);
      break;
    }
    case MtpInvalid:
    {
      assert(false);
      break;
    }
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
      MTP::ShadowTrack* currentTrack;
      for (count_t i = 0; i < pl->TrackCount(); i++)
      {
        currentTrack = pl->ChildTrack(i);
        QString trackName = QString::fromUtf8(currentTrack->GetTrack()->FileName());
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
void QMtpDevice::syncTrack(const TagLib::FileRef& tagFile, MTP::Folder const * parent)
{
  QString filePath = QFile::decodeName(QByteArray(tagFile.file()->name()));
  QFileInfo file(filePath);

  QString suffixStr = file.suffix().toUpper();
  char* suffix = strdup(suffixStr.toUtf8().data());
  QString filename = file.fileName();
  uint64_t size = (uint64_t) file.size();
  LIBMTP_filetype_t type = MTP::StringToType(suffix);
  delete suffix;

  MTP::Track* newTrack;
  newTrack = SetupTrackTransfer(tagFile, filename, size,
                                         parent->ID(), type);
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
  emit AddedTrack(newTrack);
  return ;
}

/**
 * This function will sync a file to the device
 * @param in_path the path to the file on the host computer's filesystem
 * @param parent the parent id of this file, this should be the id of a
 *               folder
 */
void QMtpDevice::syncFile(const QString& in_path, MTP::Folder const * in_parent)
{
  QFileInfo file(in_path);

  char* suffix= NULL;
  char* filename = NULL;

  QByteArray convertedFilename;
  QByteArray convertedSuffix;
  //if we have a file named ".filename"
  if (file.completeBaseName().length() == 0 && file.suffix().length() > 0)
  {
    QString blankSuffix;
	  convertedFilename = QFile::encodeName(file.fileName()).data();
	  convertedSuffix = QFile::encodeName(blankSuffix).data();
  }
  else
  {
	  convertedFilename = QFile::encodeName(file.completeBaseName()).data();
    QString suffixStr = file.suffix().toUpper();
	  convertedSuffix = QFile::encodeName(suffixStr).data();
  }
  filename = convertedFilename.data();
  suffix = convertedSuffix.data();

  std::cout << "COUT Local8 FileName: " << file.completeBaseName().toLocal8Bit().data() << std::endl;
  std::cout << "COUT FileName: " << filename << std::endl;
  std::cout << "COUT Suffix: " << suffix << std::endl;
  uint64_t size = (uint64_t) file.size();
  LIBMTP_filetype_t type = MTP::StringToType(suffix);

  qDebug() << "Syncing file of type: " << QString( MTP::TypeToString(type).c_str());

  MTP::File* newFile = SetupFileTransfer(filename, size, in_parent->ID(),
                                                  type);
  newFile->SetParentFolder(in_parent);
  if (! _device->TransferFile((const char*) in_path.toUtf8().data(),
                                newFile) )
  {
    qDebug() << "Failed to transfer file";
    return;
  }
  qDebug() << "About to emit AddedFile thread id is: " << currentThread();
  emit AddedFile(newFile);
  return;
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
  //For now we give it a fake parent ID
  return new MTP::File(file, 0);
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
  if (in_obj->Type() != MtpFile   && in_obj->Type() != MtpTrack  &&
      in_obj->Type() != MtpFolder && in_obj->Type() != MtpAlbum  &&
      in_obj->Type() != MtpPlaylist && in_obj->Type() != MtpShadowTrack)
  {
    qDebug() << "Object of unknown type!" << endl;
    assert(false);
  }

  switch (in_obj->Type())
  {
    case MtpInvalid:
    {
      assert(false);
      break;
    }
    case MtpFile:
    {
      MTP::File* deletedObj = (MTP::File*) in_obj;
      assert(!deletedObj->Association());
      bool ret = _device->RemoveFile( deletedObj);
      if (ret)
      {
        emit RemovedFile(deletedObj);
      }
      break;
    }

    case MtpTrack:
    {
      MTP::Track* deletedObj = (MTP::Track*) in_obj;
      assert(deletedObj->Association()->Type() == MtpFile);
      bool ret = _device->RemoveTrack( deletedObj);
      if (ret)
      {
        MTP::File* association= (MTP::File*) deletedObj->Association();
        MTP::Track* deletedTrack = ((MTP::Track*)deletedObj);
        for (count_t i = 0; i < deletedTrack->ShadowAssociationCount(); i++)
        {
          MTP::ShadowTrack* strack = deletedTrack->ShadowAssociation(i);
          if (strack)
            emit RemovedTrackFromPlaylist(strack);
        }
        emit RemovedTrack(deletedTrack);
        emit RemovedFile(association);
      }
      break;
    }

    case MtpShadowTrack:
    {
      MTP::ShadowTrack* deletedTrack = (MTP::ShadowTrack*) in_obj;
      MTP::Playlist* parent = deletedTrack->ParentPlaylist();
      bool ret;
      ret = _device->RemoveShadowTrack(deletedTrack);
      if (ret)
        emit RemovedTrackFromPlaylist(deletedTrack);
      break;
    }

    case MtpFolder:
    {
      emit RemovedFolder((MTP::Folder*) in_obj);
      break;
    }

    case MtpAlbum:
    {
      qDebug() << "Deleting album";
      assert(in_obj->Type() == MtpAlbum);
      MTP::Album* deletedAlb = (MTP::Album*) in_obj;
      assert(deletedAlb->Association()->Type() == MtpFile);

      bool ret = false;
      //TODO make this a configurable option-
      //Delete tracks associated with albums on album deletions?
      while(deletedAlb->TrackCount() > 0)
      {
        MTP::Track* deletedTrack = deletedAlb->ChildTrack(0);
        assert(deletedTrack->Association()->Type() == MtpFile);

        ret = _device->RemoveTrack(deletedTrack);
        //TODO report error here ?
        if (!ret)
          return;
        emit RemovedTrack( deletedTrack);
        emit RemovedFile( (MTP::File*)deletedTrack->Association());
      }

      ret = _device->RemoveAlbum( deletedAlb);
      MTP::File* association= (MTP::File*) deletedAlb->Association();
      if (ret)
      {
        emit RemovedAlbum( deletedAlb);
        emit RemovedFile(association);
      }
      break;
    }

    case MtpPlaylist:
    {
      assert(in_obj->Type() == MtpPlaylist);
      MTP::Playlist* deletedPl = (MTP::Playlist*) in_obj;
      assert(deletedPl->Association()->Type() == MtpFile);
      bool ret = false;
      for (count_t i = 0; i < deletedPl->TrackCount(); i++)
      while(deletedPl->TrackCount() > 0)
      {
        ret = _device->RemoveShadowTrack(deletedPl->ChildTrack(0));
        if (!ret)
          return;
        emit RemovedTrackFromPlaylist( deletedPl->ChildTrack(0) );
      }

      ret = _device->RemovePlaylist(deletedPl);
      MTP::File* association = (MTP::File*) deletedPl->Association();
      if (ret)
      {
        emit RemovedPlaylist(deletedPl);
        emit RemovedFile(association);
      }
    }
   }
}
