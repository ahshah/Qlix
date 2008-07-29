/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *   TODO  check duplicate enteries when adding files to an album
 */

#include "widgets/AlbumModel.h"
/**
 * Construct a new AlbumModel
 */
AlbumModel::AlbumModel(vector<MTP::Album*> in_albums, QObject* parent) :
                                              _albumList(in_albums)
{  }

/**
 * Destructor for the AlbumModel
 */
AlbumModel::~AlbumModel()
{
  for (count_t i = 0; i < _albumList.size(); i++)
  {
    delete _albumList[i];
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

/**
 * Adds an album to this model
 * @param in_album the album to add to the model
 */
void AlbumModel::AddAlbum(MTP::Album* in_album)
{
  //invalid parent for root items
  qDebug() << "Called Add Album" << endl;
  QModelIndex parentIdx = QModelIndex();

  emit beginInsertRows(parentIdx, _albumList.size(), _albumList.size());
  in_album->SetRowIndex(_albumList.size());
  _albumList.push_back(in_album);
  emit endInsertRows();
}


/**
 * Adds an track to this model
 * @param in_track the track to add, the parent album is determined through
 * in_track's parent field
 */
void AlbumModel::AddTrack(MTP::Track* in_track)
{
  qDebug() << "Called AddTrack";
  MTP::Album* parentAlbum = in_track->ParentAlbum();
  assert(parentAlbum);
  in_track->SetRowIndex(parentAlbum->TrackCount());
  QModelIndex parentIdx = createIndex(parentAlbum->GetRowIndex(), 0, parentAlbum);
  unsigned int sz = parentAlbum->TrackCount();

  emit beginInsertRows(parentIdx, parentAlbum->TrackCount(), 
                       parentAlbum->TrackCount());
  parentAlbum->AddTrack(in_track);
  emit endInsertRows();

  assert(sz+1 == parentAlbum->TrackCount());
  qDebug() << "old size: " << sz << " new size: " << parentAlbum->TrackCount();
}

/**
 * Removes an album from the model 
 * @param in_album the album to remove
 */
void AlbumModel::RemoveAlbum(MTP::Album* in_album)
{
  //invalid parent for root items
  qDebug() << "Called RemoveAlbum";
  assert(in_album);
  assert(in_album->GetRowIndex() < _albumList.size());
  QModelIndex parentIdx = QModelIndex();

  emit beginRemoveRows(parentIdx, in_album->GetRowIndex(),
                      in_album->GetRowIndex());
  for (unsigned int i =in_album->GetRowIndex()+1; i < _albumList.size(); i++)
    _albumList[i]->SetRowIndex(i -1);
  MTP::Album* deleteThisAlbum = _albumList[in_album->GetRowIndex()];
  std::vector<MTP::Album*>::iterator iter= _albumList.begin() + in_album->GetRowIndex();
  _albumList.erase(iter);
  emit endRemoveRows();
}

/**
 * Removes a track from the model 
 * @param in_album the album to remove
 */
void AlbumModel::RemoveTrack(MTP::Track* in_track)
{
  qDebug() << "Called RemoveTrack";
  MTP::Album* parentAlbum = in_track->ParentAlbum();
  assert(parentAlbum);

  QModelIndex parentIdx = createIndex(parentAlbum->GetRowIndex(), 0,
                                      parentAlbum);
  emit beginRemoveRows(parentIdx, in_track->GetRowIndex(), 
                       in_track->GetRowIndex());
  parentAlbum->RemoveTrack(in_track->GetRowIndex());
  emit endRemoveRows();
}
