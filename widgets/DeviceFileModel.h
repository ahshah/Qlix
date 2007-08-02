//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DEVICEFILEMODEL__
#define __DEVICEFILEMODEL__
#include <QAbstractListModel>
#include <QObject>
#include "MtpDevice.h"
#include "types.h"
#include <QIcon>
#include <QDebug>
#include <iostream>
using namespace std;

class DeviceFileModel : public QAbstractListModel
{
    Q_OBJECT
public:
    DeviceFileModel (QObject* parent = 0) : QAbstractListModel(parent)
    {
        _mtpFileSystem = NULL;
        _dir = NULL;
        _listSize = 0;
        _sortedType = SortByNameUp;
    }

    void Set (MtpFS* in_fs)
    {
        _mtpFileSystem = in_fs;
        SetFolder(0);
    }
        
    void Unset()
    {
        _mtpFileSystem = NULL;
        _dir = NULL;
        _listSize = 0;
    }

    DirNode* GetCurrentDirectory ( void )
    {
        cout << "Going to return: " << _dir << endl;
        return _dir;
    }

    void SetFolder ( uint32_t folder_id)
    {
        if (!_mtpFileSystem)
            return;
        _dir = _mtpFileSystem->GetDirectory(folder_id);
        _listSize = _dir->GetFileCount();
        sort();
    }
    void AddFile (FileNode& File)
    {
        _mtpFileSystem->AddFile(File);
        _listSize++;
    }

    uint32_t CurrentFolder ()
    {
        if (!_mtpFileSystem)
            return 0;
        return _dir->GetID();
    }

    int rowCount (const QModelIndex& ) const
    {
        if (!_mtpFileSystem)
            return 0;
        return _listSize;
    }

    int columnCount (const QModelIndex& ) const
    {
        return 3;
    }


    QVariant headerData(int section, Qt::Orientation orientation, int role) const
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

    QVariant data(const QModelIndex &index, int role) const
    {
        if (!_mtpFileSystem)
            return QVariant();
        if (role == Qt::SizeHintRole)
            return QSize(18,18);

        if (role == Qt::DecorationRole && index.column() == 0)
        {
                QIcon temp("/home/ahshah/Qlix/pixmaps/file.png");
                QPixmap temp2 = temp.pixmap(18, 18);
                return temp2;
        }
        if (role != Qt::DisplayRole)
            return QVariant();
         
        FileNode file = _dir->GetFile(index.row());

        if (index.column() == 0)
        {
            QString s = QString::fromUtf8((file.GetName()).c_str());
            return s;
        }

        if (index.column() == 1)
        {
            //double f1 = (float) (file.GetSize() / (float) (1024));
            //cout << file.GetName() << " in kb: " << f1 << endl;
            double f = (float) (file.GetSize() / (float) (1024 * 1024));
            if ((f*100) < (float) 1)
            {
               // cout << file.GetName() << " is small return in kb" << endl;
                f = (float) (file.GetSize()) / (float) (1024);
                QVariant s = f;
                QString ret = QString("%1 KB").arg(f, 0, 'f', 2);
                return ret;
            }
            QVariant s = f;
            QString s2 = QString("%1 MB").arg(f, 0, 'f', 2);
            return s2;
        }
        if (index.column() == 2)
        {
            QString s2 = QString::fromUtf8(file.GetType().c_str()); 
            return s2;
        }
        return QVariant();
    }

    FileNode FileFromIndex(QModelIndex* in_index)
    {
        //do range checking here;
        return _dir->GetFile(in_index->row());
    }

public slots:
    void sort ( int column = -1)
    {    
        //cout << "sort checking connection" << endl;
        if (!_mtpFileSystem)
            return;
        if (column == -1)
        {
            _sortedType = SortByNameUp;
            _dir->SortFiles(_sortedType);
        }

        if (column == 0)
        {
            //cout << "clicked name" << endl;
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
            //cout << "clicked size" << endl;
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

    void FileTransferDone ( bool in )
    {
        if (in)
        {
            _listSize = _dir->GetFileCount();
            emit layoutChanged();
        }
        return;
    }

    void FileRemoved( bool in, index_t in_DirID, index_t in_FileID)
    {
        cout << "called file removed: " << in << endl;
        cout << "removing file with ID: "<< in_FileID<< endl;
        if (in)
        {
            DirNode* Parent = _mtpFileSystem->GetDirectory(in_DirID);
            Parent->DeleteFile(in_FileID);
            _listSize = _dir->GetFileCount();
            emit layoutChanged();
        }
        return;
    }


private:
    FileSortType _sortedType;
    MtpFS*  _mtpFileSystem;
    DirNode* _dir;
    count_t _listSize;
};

#endif
