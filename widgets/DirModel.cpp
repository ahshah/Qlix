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
 *   TODO SampleData for MTP::Files
 */
#include "widgets/DirModel.h"

DirModel::DirModel(MTP::Folder* in_rootFolder, QObject* parent) :
                                                      _rootFolder(in_rootFolder)
{ }

QModelIndex DirModel::index(int row, int col, 
                        const QModelIndex& parent) const
{ 
  if(col < 0 || row < 0)
    return QModelIndex();
  if(col >= columnCount(parent) || row >= rowCount(parent))
    return QModelIndex();
  
  if(!parent.isValid() )
  {
    assert(row == 0 && col == 0);
    return createIndex(row, col, _rootFolder);
  }
    
  MTP::GenericObject* temp = (MTP::GenericObject*)parent.internalPointer();

  if (temp->Type() != MtpFolder)
    assert(false);
    
  MTP::Folder* folder = (MTP::Folder*)temp;
  int total = (int) folder->FolderCount() + folder->FileCount();
  if (row >= (int) total)
  {
    qDebug() << "Requested row is out of bounds- not enough children!";
    return QModelIndex(); 
  }

  if (row < (int) folder->FolderCount() )
  {
    MTP::Folder* ret = folder->ChildFolder(row);
    assert(ret);
    return createIndex(row, col, ret); 
  }
  int idx = row - folder->FolderCount();
  assert(idx < (int)folder->FileCount() && idx >= 0);
  MTP::File* ret = folder->ChildFile(idx);
  assert(ret);
  return createIndex(row, col, ret);
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
//   MTP::Folder* fobj = (MTP::Folder*) obj;
//   qDebug() << "folder " << QString::fromUtf8(fobj->Name()) << " 's parent is: " << QString::fromUtf8(parent->Name());
   return createIndex(parent->GetRowIndex(), 0, parent); 
  }
  else if (obj->Type() == MtpFile)
  {
    MTP::Folder* parent = ((MTP::File*)obj)->ParentFolder();
    if (!parent) 
      return QModelIndex();
     MTP::File* fobj = (MTP::File*) obj;
    qDebug() << "file" << QString::fromUtf8(fobj->Name()) << " 's parent is: " << QString::fromUtf8(parent->Name());
    return createIndex(parent->GetRowIndex(), 0, parent);
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
  //return the fake root's row count- which should be 1
  if (!parent.isValid() )
    return 1; 

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
    qDebug() << "Requesting row: "<< parent.row() << "column: " 
              << parent.column() << "of object " << (void*)obj;
    assert(false);
  }
}

int DirModel::columnCount(const QModelIndex& parent ) const 
{ 
    return 1;
}

QVariant DirModel::data(const QModelIndex& index, int role ) const
{ 
  if (!index.isValid())
    return QVariant();
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

/**
 * Adds a file to this model
 * @param in_file the file to add to the model, the parent folder is determined through
 * in_file's parent field
 */
void DirModel::AddFile(MTP::File* in_file)
{
  qDebug() << "Called AddFile";
  MTP::Folder* parentFolder = in_file->ParentFolder();
  assert(parentFolder);
  QModelIndex parentIdx = createIndex(parentFolder->GetRowIndex(), 0, parentFolder);
  count_t sz = parentFolder->FileCount();

  emit beginInsertRows(parentIdx, parentFolder->FileCount(), 
                       parentFolder->FileCount());
  parentFolder->AddChildFile(in_file);
  emit endInsertRows();

  assert(sz+1 == parentFolder->FileCount());
  qDebug() << "old size: " << sz << " new size: " << parentFolder->FileCount();
}


/**
 * Adds a folder to this model
 * @param in_folder the folder to add to the model, the parent folder is 
 * determined through in_folder's parent field. If it is NULL it is added to 
 * the root folder
 */
void DirModel::AddFolder(MTP::Folder* in_folder)
{
  //invalid parent for root items
  qDebug() << "Called AddFolder" << endl;
  MTP::Folder* parentFolder = in_folder->ParentFolder();

  if (!parentFolder)
    parentFolder = _rootFolder;

  QModelIndex parentIdx = createIndex(parentFolder->GetRowIndex(), 0, parentFolder);
  count_t subFolderCount = parentFolder->FolderCount();

  emit beginInsertRows(parentIdx, subFolderCount, subFolderCount);
  in_folder->SetRowIndex(subFolderCount);
  parentFolder->AddChildFolder(in_folder);
  emit endInsertRows();
}

void DirModel::RemoveFolder(MTP::Folder* in_folder)
{
  qDebug() << "Called RemoveFolder";
  assert(in_folder);

  MTP::Folder* parentFolder = in_folder->ParentFolder();
  if(!parentFolder)
    parentFolder = _rootFolder;

  assert(in_folder->GetRowIndex() < parentFolder->FolderCount());

  QModelIndex parentIdx = createIndex(parentFolder->GetRowIndex(), 0, parentFolder);
  count_t subFolderCount = parentFolder->FolderCount();

  emit beginRemoveRows(parentIdx, in_folder->GetRowIndex(),
                       in_folder->GetRowIndex());
  for (count_t i =in_folder->GetRowIndex()+1; i < subFolderCount; i++)
  {
      parentFolder->ChildFolder(i)->SetRowIndex(i -1);
  }

  MTP::Folder* deleteThisFolder =
                           parentFolder->ChildFolder(in_folder->GetRowIndex());
  parentFolder->RemoveChildFolder(deleteThisFolder);
  
  delete deleteThisFolder;
  emit endRemoveRows();
}

void DirModel::RemoveFile(MTP::File* in_file)
{
  qDebug() << "RemoveFile stub!" << endl;
  assert(in_file);
  MTP::Folder* parentFolder = in_file->ParentFolder();
  QModelIndex parentIdx = createIndex(parentFolder->GetRowIndex(), 0,
                                      parentFolder);
  emit beginRemoveRows(parentIdx, in_file->GetRowIndex(), 
                       in_file->GetRowIndex());
  for (count_t i =0; i < parentFolder->FileCount();
      i++)
  {
    assert(parentFolder->ChildFile(i)->GetRowIndex() == i);
  }

  for (count_t i =in_file->GetRowIndex()+1; i < parentFolder->FileCount();
      i++)
  {
    parentFolder->ChildFile(i)->SetRowIndex(i-1);
  }

  parentFolder->RemoveChildFile(in_file);

  for (count_t i =0; i < parentFolder->FileCount();
      i++)
  {
    assert(parentFolder->ChildFile(i)->GetRowIndex() == i);
  }

  delete in_file;
  emit endRemoveRows();
}
