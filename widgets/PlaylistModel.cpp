//TODO figure out why we have to subtract -1 from data
#include "widgets/PlaylistModel.h"

PlaylistModel::PlaylistModel(MtpDevice* in_dev, QObject* parent) :
                       _device(in_dev)
{ }

QModelIndex PlaylistModel::index(int row, int col, 
                        const QModelIndex& parent) const
{ 
  if(!parent.isValid() && (row >= (int)_device->PlaylistCount()))
    return QModelIndex();

  else if (!parent.isValid() )
  {
    MTP::Playlist* pl = _device->Playlist(row);
    return createIndex(row, col, (void*) pl);
  }

  MTP::GenericObject* obj= (MTP::GenericObject*)parent.internalPointer();
  assert(obj->Type() == MtpPlaylist);

  MTP::Playlist* pl= (MTP::Playlist*) obj;

  if (row >= (int)pl->TrackCount())
    return QModelIndex();

  MTP::Track* track = pl->ChildTrack(row);
  assert (track->Type() == MtpTrack);

  return createIndex(row, col, track);
}

QModelIndex PlaylistModel::parent(const QModelIndex& idx) const
{
  if (!idx.isValid())
    return QModelIndex();

  MTP::GenericObject* obj=(MTP::GenericObject*) idx.internalPointer();

  if(obj->Type() == MtpTrack)
  {
    MTP::Playlist* parent = ((MTP::Track*)obj)->ParentPlaylist();
    QModelIndex ret = index((int)parent->GetRowIndex() - 1, 0, QModelIndex()); 

    return ret;
  }
  else if (obj->Type() == MtpPlaylist)
    return QModelIndex();
  else
  {
    qDebug() << "object is of type: " << obj->Type();
    qDebug() << "Requesting row: "<< idx.row() << "column: " << idx.column() << "of object " << (void*)obj;
    assert(false);
  }
  return QModelIndex();
}

int PlaylistModel::rowCount(const QModelIndex& parent) const 
{ 
  if (!parent.isValid() )
    return _device->PlaylistCount();
  MTP::GenericObject* obj= (MTP::GenericObject*)parent.internalPointer();
  if(obj->Type() == MtpTrack)
    return 0;
  else if (obj->Type() == MtpPlaylist)
    return ((MTP::Playlist*)obj)->TrackCount();
  else
  {
    qDebug() << "invalid reference of type: " << obj->Type();
    qDebug() << "Requesting row: "<< parent.row() << "column: " << parent.column() << "of object " << (void*)obj;
    assert(false);
  }
}

int PlaylistModel::columnCount(const QModelIndex& parent ) const 
{ 
    return 2;
}

QVariant PlaylistModel::data(const QModelIndex& index, int role ) const
{ 
  if (role == Qt::DisplayRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpPlaylist && index.column() == 0)
    {
        MTP::Playlist* tempPlaylist = (MTP::Playlist*)temp;
        QString first = QString::fromUtf8(tempPlaylist->Name());
        return (first);
    }
    else if (temp->Type() == MtpTrack && index.column() == 0)
    {
        MTP::Track* tempTrack = (MTP::Track*)temp;
        QString temp = QString::fromUtf8(tempTrack->Name());
        return temp;
    }
  }

  if (role == Qt::DecorationRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpPlaylist && index.column() == 0)
    {
          QPixmap ret(":/pixmaps/playlisticon.png");
          return ret.scaledToWidth(24, Qt::SmoothTransformation);
    }
    else if (temp->Type() == MtpTrack && index.column() == 0)
    {
        return QIcon(QPixmap (":/pixmaps/track.png"));
    }
    else
      return QVariant();
  }

  if (role == Qt::FontRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    //Its an album
    if (temp->Type() == MtpPlaylist && index.column() == 0)
    {
      QFont temp;
      temp.setBold(true);
      temp.setPointSize(8);
      return temp;
    }
  }
  return QVariant();
}
