//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#include "DeviceDirModel.h"
DeviceDirModel::DeviceDirModel (QObject* parent = NULL) : QAbstractItemModel(parent)
{
    _mtpFileSystem = NULL;
    _validDeviceImage = false;
    _validImage = NULL;
}

DeviceDirModel::~DeviceDirModel ( void )
{
}

QModelIndex DeviceDirModel::index (int row, int column, const QModelIndex& parent) const
{
    if (!_mtpFileSystem)
        return QModelIndex();

    DirNode* parentDirectory = dirFromIndex(parent);

    if (row == 0 && parentDirectory == NULL) 
        return createIndex(row, column, _mtpFileSystem->GetRoot());

    DirNode* tempReturn = _mtpFileSystem->GetDirectory(parentDirectory, row);
    if (!tempReturn)
        return QModelIndex();
    return createIndex(row, column, tempReturn); 
}

void DeviceDirModel::SetRootImage(QPixmap* in_image)
{
    _validDeviceImage = true;
    _validImage = in_image;
}

QModelIndex DeviceDirModel::parent (const QModelIndex &child) const
{
    if (!_mtpFileSystem)
        return QModelIndex();

    DirNode* dir = dirFromIndex(child);
    if (!dir) // that means we are at the root
        return QModelIndex();

    //otherwise get its parent
    DirNode* parentDirectory = dir->GetParentDir(); 
    if (!parentDirectory)
        return QModelIndex();

    return createIndex(parentDirectory->GetSortedOrder(), 0, parentDirectory);
}

int DeviceDirModel::rowCount(const QModelIndex &parent) const
{
    if (!_mtpFileSystem)
        return 0;
    DirNode* parentDirectory = dirFromIndex(parent);

    if (!parentDirectory)
    {
        return 1;
        parentDirectory = _mtpFileSystem->GetRoot();
    }

    count_t tempCount = parentDirectory->SubDirectoryCount();
//   cout << "Parent directory: "<< parentDirectory->GetName() << endl;
//    cout << "RowCount: "<<tempCount<< endl;
    return tempCount;
}

int DeviceDirModel::columnCount(const QModelIndex& ) const
{
    if (!_mtpFileSystem)
        return 0;
    return 1;
}

QVariant DeviceDirModel::data(const QModelIndex &index, int role) const
{
    if (!_mtpFileSystem)
        return QVariant();
    DirNode* dir = dirFromIndex(index);

    if (role == Qt::SizeHintRole && dir->GetID() ==0)
        return QSize(32, 32);

    if (role == Qt::SizeHintRole)
        return QSize(24, 24);
    if (role == Qt:: DecorationRole)
    {
        if (!dir)
            return QVariant();
        if (dir->GetID() == 0 && role == Qt::DecorationRole)
        {
            if (_validDeviceImage)
            {
                QIcon temp( (*_validImage));
                QPixmap temp2 = temp.pixmap(32, 32);
                return temp2;
            }
            QIcon temp("./pixmaps/rootDevice.png");
            QPixmap temp2 = temp.pixmap(32, 32);
            return temp2;
        }
        if (role == Qt::DecorationRole)
        {
            QIcon temp("./pixmaps/folder.png");
            QPixmap temp2 = temp.pixmap(18, 18);
            return temp2;
        }
    }
    if (role == Qt::DisplayRole && (index.column() == 0) )
            return QString((dir->GetName()).c_str());
    return QVariant();
}

QVariant DeviceDirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return "Name";
    }
    return QVariant();
}

void DeviceDirModel::DeleteFile (const FileNode& in_file)
{
    if (!_mtpFileSystem)
        return;
    _mtpFileSystem->DeleteFile(in_file);
}

void DeviceDirModel::Set (MtpFS* in_fs)
{
    _mtpFileSystem = in_fs;
}

void DeviceDirModel::Unset ()
{   
    _mtpFileSystem = NULL;
}

DirNode* DeviceDirModel::GetDir(uint32_t id)
{
    if (!_mtpFileSystem)
        return NULL;
    _mtpFileSystem->GetDirectory(id);
}

//public slots:
void DeviceDirModel::DirectoryAdded(DirNode* , index_t )
{
    emit layoutChanged();
}
void DeviceDirModel::DirectoryRemoved(bool in_success, index_t in_folderID)
{
    DirNode* childDir = _mtpFileSystem->GetDirectory(in_folderID);
    DirNode* parent = childDir->GetParentDir();
    assert(parent != NULL); // can't delete root.

    QModelIndex parentIndex = createIndex(parent->GetSortedOrder(), 0, parent);
    QModelIndex currentIndex = createIndex(childDir->GetSortedOrder(), 0, childDir);

    beginRemoveRows(parentIndex, childDir->GetSortedOrder(), childDir->GetSortedOrder());

    removeRow(childDir->GetSortedOrder(), parentIndex);
    if (in_success)
        _mtpFileSystem->DeleteFolder(in_folderID);
    cout << "Notfied about directory deletion: " << in_folderID << endl;
    endRemoveRows();
//    emit dataChanged(currentIndex, currentIndex);
//    revert();
//    emit layoutChanged();
}

//private:
DirNode* DeviceDirModel::dirFromIndex(const QModelIndex& index) const
{
    if (index.isValid() )
        return static_cast<DirNode*> (index.internalPointer());
    else
        return NULL; 
}

