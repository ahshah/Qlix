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
    DeviceFileModel (QObject* parent = 0);
    void Set (MtpFS* in_fs);
    void Unset();
    DirNode* GetCurrentDirectory ( void );
    void SetFolder ( uint32_t folder_id);
    void AddFile (FileNode& File);
    uint32_t CurrentFolder ();
    int rowCount (const QModelIndex& ) const;
    int columnCount (const QModelIndex& ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    FileNode FileFromIndex(const QModelIndex* in_index) const;

public slots:
    void sort ( int column = -1);
    void FileTransferDone ( bool in );
    void FileRemoved( bool in, index_t in_DirID, index_t in_FileID);

private:
    QString fileSize(qint64 in_size) const;

    FileSortType _sortedType;
    MtpFS*  _mtpFileSystem;
    DirNode* _dir;
    QFont _defaultFont;
    count_t _listSize;
};
#endif
