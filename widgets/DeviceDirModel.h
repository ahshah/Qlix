//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DEVICEDIRMODEL__
#define __DEVICEDIRMODEL__
#include <QAbstractItemModel>
#include <QObject>
#include <QIcon>
#include <QPixmap>
#include "MtpDevice.h"
#include "CommandCodes.h"

class DeviceDirModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    DeviceDirModel (QObject* parent);
    ~DeviceDirModel ( void );

    QModelIndex index (int row, int column, const QModelIndex& parent) const;

    QModelIndex parent (const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex& ) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void DeleteFile (const FileNode& in_file);
    void Set (MtpFS* in_fs);
    void Unset ();

    DirNode* GetDir(uint32_t id);
    void SetRootImage(QPixmap*);
public slots:
    void DirectoryAdded(DirNode* parent, index_t height);
    void DirectoryRemoved(bool, index_t in_folderID);

private:
    DirNode* dirFromIndex(const QModelIndex& index) const;
    MtpFS*     _mtpFileSystem;
    bool _validDeviceImage;
    QPixmap* _validImage;
};

#endif
