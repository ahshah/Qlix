//TODO SampleData for MTP::Files
#include "widgets/DirModel.h"

DirModel::DirModel(MtpDevice* in_dev, QObject* parent) :
                       _device(in_dev)
{ }

QModelIndex DirModel::index(int row, int col, 
                        const QModelIndex& parent) const
{ 
  if(!parent.isValid() )
  {
    int total = (int)_device->RootFolderCount() + _device->RootFileCount();
    if (row >= total || row < 0)
      return QModelIndex();

    if (row < (int) _device->RootFolderCount() ) 
    {
      MTP::Folder* ret = _device->RootFolder(row);
      assert(ret);
      return createIndex(row, col, ret); 
    }

    int idx = row - _device->RootFolderCount();
    MTP::File* ret = _device->RootFile(idx);
    assert(ret);
    return createIndex(row, col, ret);
  }

  MTP::GenericObject* temp = (MTP::GenericObject*)parent.internalPointer();
  if (temp->Type() == MtpFolder)
  {
    MTP::Folder* folder = (MTP::Folder*)parent.internalPointer();
//    qDebug() << "indexing row: "<< row << " col " << col << "under parent folder: " << QString::fromUtf8(folder->Name());
    int total = (int) folder->FolderCount() + folder->FileCount();
    if (row > (int) total)
    {
      qDebug() << "folder with too many children, should not happen!"; 
      return QModelIndex(); 
    }

    int idx = row - folder->FolderCount();
    if (row < (int) folder->FolderCount() )
    {
      MTP::Folder* ret = folder->ChildFolder(row);
      assert(ret);
//      qDebug() << "Found folder: " << QString::fromUtf8(ret->Name());
      return createIndex(row, col, ret); 
    }

    else if (idx < (int)folder->FileCount() && idx >= 0)
    {
      MTP::File* ret = folder->ChildFile(idx);
      assert(ret);
//      qDebug() << "Found file: " << QString::fromUtf8(ret->Name());
      return createIndex(row, col, ret);
    }
    assert(false);
  }
  else if (temp->Type() == MtpFile)
  {
    qDebug() << "file requesting sub index, should not happen!";
    return QModelIndex();
  }
  assert(false);
  return QModelIndex();
}

QModelIndex DirModel::parent(const QModelIndex& idx) const
{
  if (!idx.isValid())
    return QModelIndex();

  MTP::GenericObject* obj=(MTP::GenericObject*) idx.internalPointer();
  assert(obj);

  if(obj->Type() == MtpFolder)
  {
    MTP::Folder* parent = ((MTP::Folder*)obj)->ParentFolder();
    if (!parent) 
      return QModelIndex();
   MTP::Folder* fobj = (MTP::Folder*) obj;
   qDebug() << "folder " << QString::fromUtf8(fobj->Name()) << " 's parent is: " << QString::fromUtf8(parent->Name());
   return createIndex(parent->GetRowIndex()-1, 0, parent); 
  }
  else if (obj->Type() == MtpFile)
  {
    MTP::Folder* parent = ((MTP::File*)obj)->ParentFolder();
    if (!parent) 
      return QModelIndex();
     MTP::File* fobj = (MTP::File*) obj;
    qDebug() << "file" << QString::fromUtf8(fobj->Name()) << " 's parent is: " << QString::fromUtf8(parent->Name());
    return createIndex(parent->GetRowIndex()-1, 0, parent);
  }
  else
  {
    qDebug() << "object is of type: " << obj->Type();
    qDebug() << "Requesting row: "<< idx.row() << "column: " << idx.column() << "of object " << (void*)obj;
    assert(false);
  }
  return QModelIndex();
}

int DirModel::rowCount(const QModelIndex& parent) const 
{ 
  if (!parent.isValid() )
    return _device->RootFolderCount() + _device->RootFileCount();

  MTP::GenericObject* obj= (MTP::Album*)parent.internalPointer();
  if(obj->Type() == MtpFolder)
  {
    MTP::Folder* folder = (MTP::Folder*) obj;
    return (folder->FileCount() + folder->FolderCount());
  }

  else if (obj->Type() == MtpFile)
    return 0;
  else
  {
    qDebug() << "invalid reference of type: " << obj->Type();
    qDebug() << "Requesting row: "<< parent.row() << "column: " << parent.column() << "of object " << (void*)obj;
    assert(false);
  }
}

int DirModel::columnCount(const QModelIndex& parent ) const 
{ 
    return 2;
}

QVariant DirModel::data(const QModelIndex& index, int role ) const
{ 
  if (index.internalPointer() == NULL) 
    return QVariant();
  if (role == Qt::DisplayRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpFolder && index.column() == 0)
    {
        MTP::Folder* tempFolder = (MTP::Folder*)temp;
        return QString::fromUtf8(tempFolder->Name());
    }
    else if (temp->Type() == MtpFile && index.column() == 0)
    {
        MTP::File* tempFile = (MTP::File*)temp;
        QString temp = QString::fromUtf8(tempFile->Name());
        return temp;
    }
    return QVariant();
  }

  if (role == Qt::DecorationRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpFolder && index.column() == 0)
    {
      QPixmap ret(":/pixmaps/folder.png");
      return ret.scaledToWidth(24, Qt::SmoothTransformation);
    }
    else if (temp->Type() == MtpFile && index.column() == 0)
    {
      QPixmap ret;
      /* TODO SampleData for MTP::Files
      MTP::File* tempFile = (MTP::File*)temp;
      LIBMTP_filesampledata_t sample = tempFile->SampleData();
      if (sample.filetype == LIBMTP_FILETYPE_UNKNOWN && (sample.size > 0 && sample.data) )
      {
          ret.loadFromData( (const uchar*)sample.data, sample.size);
          return ret.scaledToWidth(24, Qt::SmoothTransformation);
      }
      */
      ret.load(":/pixmaps/file.png");
      return ret.scaledToWidth(24, Qt::SmoothTransformation);
    }
    return QVariant();
  }

  if (role == Qt::FontRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    //Its an album
    if (temp->Type() == MtpFolder && index.column() == 0)
    {
      QFont temp;
      temp.setBold(true);
      temp.setPointSize(8);
      return temp;
    }
  }
  return QVariant();
}

