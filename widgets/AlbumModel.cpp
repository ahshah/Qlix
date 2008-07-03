//TODO  check duplicate enteries when adding files to an album
#include "widgets/AlbumModel.h"
/**
 * Construct a new AlbumModel
 */
AlbumModel::AlbumModel(MtpDevice* in_dev, QObject* parent) :
                       _device(in_dev)
{ 
  for (int i = 0; i < _device->AlbumCount(); i++)
  {
//    (_device->Album(i))->SetRowIndex(i);
    _albumList.push_back(_device->Album(i));
  }
}


/**
 * Return the Album or track at the given index and parent 
 * row and col are realtive to parent
 * The parent's internal pointer should only be of types MtpTrack and MtpAlbum
 * @param row the row coordinate of the item to display
 * @param col the column coordinate of the item to display
 * @param parent the parent of the object
 * @return an index constructed of the item to display or an invalid index if
 * the request coordinates are out of bounds
 */
QModelIndex AlbumModel::index(int row, int col, 
                        const QModelIndex& parent) const
{ 
  if(col < 0 || row < 0)
    return QModelIndex();

  if(!parent.isValid() && row >= (int)_albumList.size())
    return QModelIndex();

  if (!parent.isValid() && col == 0)
  {
    MTP::Album* album = _albumList[row];
    return createIndex(row, col, (void*) album);
  }
  if (!parent.isValid() && col > 0)
  {
    return QModelIndex();
  }

  MTP::GenericObject* obj= (MTP::GenericObject*)parent.internalPointer();
  assert(obj->Type() == MtpAlbum);

  MTP::Album* album = (MTP::Album*) obj;

  if (row >= (int)album->TrackCount())
    return QModelIndex();

  MTP::Track* track = album->ChildTrack(row);
  assert (track->Type() == MtpTrack);

  return createIndex(row, col, track);
}

/** 
 * Returns the parent of the given index
 * @param idx the index of whose parent we must create
 * @return an index constructured of parent the passed paramameter idx or an
 * invalid index if the parent is the root level
 */
QModelIndex AlbumModel::parent(const QModelIndex& idx) const
{
  if (!idx.isValid())
    return QModelIndex();

  MTP::GenericObject* obj=(MTP::GenericObject*) idx.internalPointer();

  if(obj->Type() == MtpTrack)
  {
    MTP::Album* parent = ((MTP::Track*)obj)->ParentAlbum();
    assert(parent != (MTP::Album*)obj);
//    QModelIndex ret = index((int)parent->GetRowIndex()-1, 0, QModelIndex()); 
    QModelIndex ret = createIndex(parent->GetRowIndex(),0, parent);

    return ret;
  }
  else if (obj->Type() == MtpAlbum)
    return QModelIndex();
  else
  {
    qDebug() << "object is of type: " << obj->Type();
    qDebug() << "Requesting row: "<< idx.row() << "column: " << idx.column() << "of object " << (void*)obj;
    assert(false);
  }
  return QModelIndex();
}

/**
 * Returns the row count of the parent this should be the number of tracks 
 * under an album or 0 if the parent happens to be a track
 * @param parent the parent item whos row counts we are trying to discover
 * @return the number of rows that occur under the given parameter: parent
 */ 
int AlbumModel::rowCount(const QModelIndex& parent) const 
{ 
  if (!parent.isValid() )
    return _albumList.size();
  MTP::GenericObject* obj= (MTP::Album*)parent.internalPointer();
  if(obj->Type() == MtpTrack)
    return 0;
  else if (obj->Type() == MtpAlbum)
    return ((MTP::Album*)obj)->TrackCount();
  else
  {
    qDebug() << "invalid reference of type: " << obj->Type();
    qDebug() << "Requesting row: "<< parent.row() << "column: " << parent.column() << "of object " << (void*)obj;
    assert(false);
  }
}
/** Return the column count at the given parent index, 2 seemed reasonable 
 * at the current time
 * @param parent the index whos column count we are trying to discover
 * @return the number of colums that occur beside the given parent
 */
int AlbumModel::columnCount(const QModelIndex& parent ) const 
{ 
//  MTP::GenericObject* obj = (MTP::GenericObject*)parent.internalPointer();
//  if (obj && obj->Type() == MtpAlbum)
//    return 2;
  return 1;
}
/**
 * Returns the data to display at the given index and the role
 * @param index the index of the item to display
 * @param role the role this data will be used for
 */
QVariant AlbumModel::data(const QModelIndex& index, int role ) const
{ 
  if (!index.isValid())
    return QVariant();
  if (role == Qt::DisplayRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpAlbum && index.column() == 0)
    {
        MTP::Album* tempAlbum = (MTP::Album*)temp;
        QString first = QString::fromUtf8(tempAlbum->Name());
//        QString second = QString('\n') + QString("    ") + QString::fromUtf8(tempAlbum->Artist());
        return QVariant(first);
    }
    else if (temp->Type() == MtpTrack && index.column() == 0)
    {
        MTP::Track* tempTrack = (MTP::Track*)temp;
        QString temp = QString::fromUtf8(tempTrack->Name());
        return QVariant(temp);
    }
    return QVariant();
  }
  if (role == Qt::DecorationRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpAlbum && index.column() == 0)
    {
      MTP::Album* tempAlbum = (MTP::Album*)temp;
      LIBMTP_filesampledata_t sample = tempAlbum->SampleData();
      if (sample.size > 0 && sample.data )
        /*&& 
          (sample.filetype == LIBMTP_FILETYPE_JPEG ||
          sample.filetype == LIBMTP_FILETYPE_JPX ||
          sample.filetype == LIBMTP_FILETYPE_JP2))
          */
      {
        QPixmap ret;
        ret.loadFromData( (const uchar*)sample.data, sample.size);
//        qDebug() << "album decoration found:" << sample.filetype  << " with size: " << sample.size;
#ifdef SIMULATE_TRANSFERS
        qDebug()  << "Actual sample found in simulate mode!";
#endif
        return ret.scaledToWidth(24, Qt::SmoothTransformation);
      }
      else 
      {
//        qDebug() << "album decoration is not a jpeg:" << sample.filetype  << " with size: " << sample.size;
        QPixmap ret("pixmaps/miscAlbumCover.png");
        return ret.scaledToWidth(24, Qt::SmoothTransformation);
      }
    }
    else if (temp->Type() == MtpTrack && index.column() == 0)
    {
        return QIcon(QPixmap (":/pixmaps/track.png"));
    }
    else
      return QVariant();
  }
  if (role == Qt::SizeHintRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    /*
    if (temp->Type() == MtpAlbum && index.column() == 0)
      return QSize(28, 28);
      */
  }
  if (role == Qt::FontRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    //Its an album
    if (temp->Type() == MtpAlbum && index.column() == 0)
    {
      QFont temp;
      temp.setBold(true);
      temp.setPointSize(8);
      return temp;
    }
  }
  return QVariant();
}
/*
bool AlbumModel::Delete(MTP::GenericObject* in_obj)
{
  if (in_obj->Type() != MtpTrack && in_obj->Type() != MtpAlbum)
    return false;
  if (in_obj->Type() == MtpAlbum)
  {
    MTP::Album* deletedAlbum = (MTP::Album*) in_obj;
    assert(deletedAlbum->TrackCount() == 0);
  }
  else if (in_obj->Type() == MtpTrack)
  {
    MTP::Track* deletedTrack = (MTP::Track*) in_obj;
    /*
    assert(deletedTrack->ParentAlbum());
    if (!_device->DeleteTrackFromAlbum(deletedTrack, 
                                       deletedTrack->ParentAlbum()) )
    {
      qDebug() << "Could not delete track!";
      return false;
    }

    if (!_device->DeleteTrack(deletedTrack))
    {
      qDebug() << "Could not delete track!"; 
      return false;
    }

    */

/*
    MTP::Album* parent = deletedTrack->ParentAlbum();
    assert(parent);

    QModelIndex temp;
    temp = createIndex(parent->GetRowIndex(), 0, parent);
    qDebug() <<"Under parent: " << QString::fromUtf8(parent->Name()) << " with row:"<< parent->GetRowIndex()
             << " Removing child: " << QString::fromUtf8(deletedTrack->Name()) << " with row: " << deletedTrack->GetRowIndex();
    int idx = 0;
    for (; idx < parent->TrackCount(); idx++)
    {
      MTP::Track* track = parent->ChildTrack(idx);
      if(track == in_obj)
        break;
    }
    qDebug() << "Album is initialized:" << parent->Initialized();
    assert(idx < parent->TrackCount());
    assert(idx == deletedTrack->GetRowIndex());
    emit layoutAboutToBeChanged();
    emit beginRemoveRows(temp, deletedTrack->GetRowIndex(),
                         deletedTrack->GetRowIndex());
    parent->RemoveTrack(deletedTrack->GetRowIndex());
    emit endRemoveRows();
    emit layoutChanged();
  }
}
*/

/**
 * Adds an album to this model
 */
void AlbumModel::AddAlbum(MTP::Album* in_album)
{
  //invalid parent for root items
  qDebug() << "Called Add Album" << endl;
  QModelIndex parentIdx = QModelIndex();
//  emit layoutAboutToBeChanged();
  emit beginInsertRows(parentIdx, _albumList.size(), _albumList.size());
  in_album->SetRowIndex(_albumList.size());
  _albumList.push_back(in_album);
  emit endInsertRows();
//  emit layoutChanged();
}

void AlbumModel::AddTrack(MTP::Track* in_track)
{
  qDebug() << "Called Add Track";
  MTP::Album* parentAlbum = in_track->ParentAlbum();
  assert(parentAlbum);
  in_track->SetRowIndex(parentAlbum->TrackCount());
  QModelIndex parentIdx = createIndex(parentAlbum->GetRowIndex(), 0, parentAlbum);
  int sz = parentAlbum->TrackCount();
//  emit layoutAboutToBeChanged();
  emit beginInsertRows(parentIdx, parentAlbum->TrackCount(), 
                       parentAlbum->TrackCount());
  parentAlbum->AddTrack(in_track);
  emit endInsertRows();
//  emit layoutChanged();
  assert(sz+1 == parentAlbum->TrackCount());
  qDebug() << "old size: " << sz << " new size: " << parentAlbum->TrackCount();
}

void AlbumModel::RemoveAlbum(MTP::Album* in_album)
{
  //invalid parent for root items
  qDebug() << "Called RemoveAlbum";
  assert(in_album);
  assert(in_album->GetRowIndex() < _albumList.size());
  QModelIndex parentIdx = QModelIndex();
  emit beginRemoveRows(parentIdx, in_album->GetRowIndex(),
                      in_album->GetRowIndex());
  for (int i =in_album->GetRowIndex()+1; i < _albumList.size(); i++)
    _albumList[i]->SetRowIndex(i -1);
  MTP::Album* deleteThisAlbum = _albumList[in_album->GetRowIndex()];
  _albumList.remove(in_album->GetRowIndex());
  delete deleteThisAlbum;
  emit endRemoveRows();
}

void AlbumModel::RemoveTrack(MTP::Track* in_track)
{
  qDebug() << "Called RemoveTrack";
  MTP::Album* parentAlbum = in_track->ParentAlbum();
  assert(parentAlbum);

  QModelIndex parentIdx = createIndex(parentAlbum->GetRowIndex(), 0,
                                      parentAlbum);
  //emit layoutAboutToBeChanged();
  emit beginRemoveRows(parentIdx, in_track->GetRowIndex(), 
                       in_track->GetRowIndex());
  parentAlbum->RemoveTrack(in_track->GetRowIndex());
  emit endRemoveRows();
 // emit layoutChanged();
}


/**
 * Adds a track to this model, it is this functions job to figure out if there
 * is an applicable Album to add, or to create an album if it doesn't exist
 * @param in_path the path to the track to add
 * @param in_track the track to add to this model
 * @return true if adding the device track was sucessful false otherwise
 */
/*
bool AlbumModel::AddTrack(const QString& in_path, MTP::Track* in_track)
{
  if (! _device->TransferTrack(in_path.toUtf8().data(), in_track) )
  {
    qDebug() << "Transfer track failed.. ";
    return false;
  }
                         
  MTP::Album* trackAlbum = NULL;
  QString findThisAlbum = QString::fromUtf8(in_track->AlbumName());
  for (count_t i = 0; i < _device->AlbumCount(); i++)
  {
    MTP::Album* album = _device->Album(i);
    if (QString::fromUtf8(album->Name()) == findThisAlbum)
    {
      trackAlbum = album;
      break;
    }
  }

  bool ret;
  if (!trackAlbum)
  {
    QModelIndex temp;
    //first try adding a new album to the device because one does not exist..
    if(!_device->NewAlbum(in_track, &trackAlbum))
    {
      qDebug() << "Failed to create new album";
      return false;
    }
    //Try and find some cover art
    QFileInfo cover;
    bool ret = discoverCoverArt(in_path, 
                                QString::fromUtf8(trackAlbum->Name()),
                                &cover);
    if (ret)
    {
      LIBMTP_filesampledata_t* sample = _device->DefaultJPEGSample();
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

    //if thats successful we can update the view..
    emit layoutAboutToBeChanged();
    emit beginInsertRows(temp, _device->AlbumCount(), 
                               _device->AlbumCount());
    _device->AddAlbum(trackAlbum);
    emit endInsertRows();
    emit layoutChanged();

    //now add the track to the found album and update it on the device
    if (!_device->AddTrackToAlbum(in_track, trackAlbum))
    {
      qDebug() << "Failed to add track to album";
      return false;
    }

    temp = createIndex(trackAlbum->GetRowIndex(), 0, trackAlbum);
    emit layoutAboutToBeChanged();
    emit beginInsertRows(temp, trackAlbum->TrackCount(), 
                         trackAlbum->TrackCount());
    trackAlbum->AddTrack(in_track);
    emit endInsertRows();
    emit layoutChanged();
    return true;
  }
  assert(trackAlbum);

  //now add the track to the found album and update it on the device
  if (!_device->AddTrackToAlbum(in_track, trackAlbum))
  {
    qDebug() << "Failed to add track to album";
    return false;
  }
  //if thats successfull we can update the view
  QModelIndex temp = createIndex(trackAlbum->GetRowIndex(), 0, trackAlbum);
  emit layoutAboutToBeChanged();
  emit beginInsertRows(temp, trackAlbum->TrackCount(), 
                       trackAlbum->TrackCount());
  trackAlbum->AddTrack(in_track);
  emit endInsertRows();
  emit layoutChanged();
  return true;
}
*/
