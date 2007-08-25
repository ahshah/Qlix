//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#include "DeviceFileModel.h"
DeviceFileModel::DeviceFileModel (QObject* parent) : QAbstractListModel(parent)
{

    _mtpFileSystem = NULL;
    _dir = NULL;
    _listSize = 0;
    _defaultFont.setFamily("Arial");
    _sortedType = SortByNameUp;
}

void DeviceFileModel::Set (MtpFS* in_fs)
{
    _mtpFileSystem = in_fs;
    SetFolder(0);
}
    
void DeviceFileModel::Unset()
{
    _mtpFileSystem = NULL;
    _dir = NULL;
    _listSize = 0;
}

DirNode* DeviceFileModel::GetCurrentDirectory ( void )
{
    qDebug() << "Going to return: " << _dir ;
    return _dir;
}

void DeviceFileModel::SetFolder ( uint32_t folder_id)
{
    if (!_mtpFileSystem)
        return;
    _dir = _mtpFileSystem->GetDirectory(folder_id);
    _listSize = _dir->GetFileCount();
    sort();
}

void DeviceFileModel::AddFile (FileNode& File)
{
    _mtpFileSystem->AddFile(File);
    _listSize++;
}

uint32_t DeviceFileModel::CurrentFolder ()
{
    if (!_mtpFileSystem)
        return 0;
    return _dir->GetID();
}

int DeviceFileModel::rowCount (const QModelIndex& ) const
{
    if (!_mtpFileSystem)
        return 0;
    return _listSize;
}

int DeviceFileModel::columnCount (const QModelIndex& ) const
{
    return 3;
}


QVariant DeviceFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return "Name";
        else if (section == 1)
            return "Size";
        else if (section == 2)
            return "Type";
    }
    return QVariant();
}

QVariant DeviceFileModel::data(const QModelIndex &index, int role) const
{
    if (!_mtpFileSystem)
    {
        qDebug() << "Double Ding";
        return QVariant();
    }

    if (role == Qt::FontRole)
        return _defaultFont;


    if (role == Qt::SizeHintRole && index.column() == 1)
    {
        FileNode temp = FileFromIndex(&index);
        QString s = temp.GetFileName();
        QFontMetrics temp_metric(_defaultFont);
//        qDebug() << "Font Size hint:" << temp_metric.width(s);
        return QSize(temp_metric.width(s), 18);
    }

    if (role == Qt::SizeHintRole && index.column() == 2)
    {
        FileNode temp = FileFromIndex(&index);
        QString size_string = fileSize(temp.GetSize());

        QFontMetrics temp_metric(_defaultFont);
        qDebug() << "FileSize sring:" << size_string;
        qDebug() << "FileSize Size hint:" << temp_metric.width(size_string);
        return QSize(temp_metric.width(size_string), 18);
    }

    if (role == Qt::DecorationRole && index.column() == 0)
    {
        QIcon temp(":/pixmaps/file.png");
        QPixmap temp2 = temp.pixmap(18, 18);
        return temp2;
    }

    if (role != Qt::DisplayRole)
        return QVariant();
     
    FileNode file = _dir->GetFile(index.row());

    if (index.column() == 0)
    {
        QString s = file.GetFileName();
        qDebug() << "Saw filename as: " << s;
        return s;
    }

    if (index.column() == 1)
    {
        return fileSize(file.GetSize());
    }

    if (index.column() == 2)
    {
        QString s2 = file.GetType();
        return s2;
    }
    return QVariant();
}

QString DeviceFileModel::fileSize(qint64 in_size) const
{
    double f = (float) (in_size / (float) (1024 * 1024));
    if ((f*100) < (float) 1)
    {
       // qDebug() << file.GetName() << " is small return in kb" ;
        f = (float) (in_size / (float) (1024));
        QVariant s = f;
        QString ret = QString("%1 KB").arg(f, 0, 'f', 2);
        return ret;
    }
    QVariant s = f;
    QString s2 = QString("%1 MB").arg(f, 0, 'f', 2);
    return s2;
}
FileNode DeviceFileModel::FileFromIndex(const QModelIndex* in_index) const
{
    //do range checking here;
    return _dir->GetFile(in_index->row());
}

//public slots:
void DeviceFileModel::sort ( int column)
{    
    //qDebug() << "sort checking connection" ;
    if (!_mtpFileSystem)
        return;
    if (column == -1)
    {
        _sortedType = SortByNameUp;
        _dir->SortFiles(_sortedType);
    }

    if (column == 0)
    {
        //qDebug() << "clicked name" ;
        switch (_sortedType)
        {
            case SortByNameUp:
                _sortedType = SortByNameDown;
                _dir->SortFiles(_sortedType);
                break;

            case SortByNameDown:
                _sortedType = SortByNameUp;
                _dir->SortFiles(_sortedType);
                break;

            default:
                _sortedType = SortByNameUp;
                _dir->SortFiles(_sortedType);
        }
    }

    else if (column == 1)
    {
        //qDebug() << "clicked size" ;
        switch (_sortedType)
        {
            case SortBySizeUp:
                _sortedType = SortBySizeDown;
                _dir->SortFiles(_sortedType);
                break;
            case SortBySizeDown:
                _sortedType = SortBySizeUp;
                _dir->SortFiles(_sortedType);
                break;
            default:
                _sortedType = SortBySizeUp;
                _dir->SortFiles(_sortedType);
        }

    }
    emit layoutChanged();
}

void DeviceFileModel::FileTransferDone ( bool in )
{
    if (in)
    {
        _listSize = _dir->GetFileCount();
        emit layoutChanged();
    }
    return;
}

void DeviceFileModel::FileRemoved( bool in, index_t in_DirID, index_t in_FileID)
{
    qDebug() << "called file removed: " << in ;
    qDebug() << "removing file with ID: "<< in_FileID;
    if (in)
    {
        DirNode* Parent = _mtpFileSystem->GetDirectory(in_DirID);
        Parent->DeleteFile(in_FileID);
        _listSize = _dir->GetFileCount();
        emit layoutChanged();
    }
    return;
}
