/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This file is part of the Qlix project on http://berlios.de
 *
 *   This file may be used under the terms of the GNU General Public
 *   License version 2.0 as published by the Free Software Foundation
 *   and appearing in the file COPYING included in the packaging of
 *   this file.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License version 2.0 for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __QMTPDEVICE__
#define __QMTPDEVICE__
#include <QThread>
#include <QIcon>
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QQueue>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfo>
#include <QModelIndex>
#include <QSortFilterProxyModel>

#include "mtp/MtpDevice.h"
#include "widgets/MtpWatchDog.h"
#include "widgets/CommandCodes.h"
#include "widgets/AlbumModel.h"
#include "widgets/PlaylistModel.h"
#include "widgets/DirModel.h"

#include "mtp/Icon.h"
#include "mtp/BmpStructs.h"
#include <tag.h>
#include <fileref.h>
#include <tfile.h>
using namespace MTPCMD;

class MtpWatchDog;
/*
 * A threaded version of MtpDevice representing device attributes using QT 
 * objects and models
 */
class QMtpDevice : public QThread
{
  Q_OBJECT
public:
  QMtpDevice(MtpDevice*, MtpWatchDog*, QObject* parent = NULL);
  virtual ~QMtpDevice();
  QString Name();
  QString Serial();
  QImage IconImage();

  void TransferTrack(QString filepath);
  void TransferFrom(MTP::GenericObject*, QString );
  void DeleteObject(MTP::GenericObject*);

  void IssueCommand (GenericCommand* in_command);
  QSortFilterProxyModel* GetAlbumModel() const;
  QSortFilterProxyModel* GetPlaylistModel() const;
  QSortFilterProxyModel* GetDirModel() const;
  void Progress(uint64_t const sent, uint64_t const total);
  void FreeSpace(uint64_t* , uint64_t*);
  void SetSelectedStorage(count_t);
  unsigned int SelectedStorage();
  unsigned int StorageDeviceCount();
  MtpStorage* StorageDevice(unsigned int);

signals:
  void Initialized(QMtpDevice*);
  void UpdateProgress(QString, count_t);
  void NotATrack(SendFileCmd*);
  void TrackTransferComplete(MTP::Track*);

  void AddedFile(MTP::File*);
  void RemovedFile(MTP::File*);

  void CreatedAlbum(MTP::Album*);
  void RemovedAlbum(MTP::Album*);

  void AddedTrack(MTP::Track*);
  void RemovedTrack(MTP::Track*);

  void RemovedFolder(MTP::Folder*);
  void AddedFolder(MTP::Folder*);


  void CreatedPlaylist(MTP::Playlist*);
  void RemovedPlaylist(MTP::Playlist*);

  void AddedTrackToPlaylist(MTP::Playlist*);
  void RemovedTrackFromPlaylist(MTP::ShadowTrack*);
protected:
  void run();

private:

  void findAndRetrieveDeviceIcon();
  void initializeDeviceStructures();

  void proccessJob(GenericCommand*);

  MTP::Track* SetupTrackTransfer(TagLib::FileRef tagFile, const QString&, 
                                 uint64_t, uint32_t,  LIBMTP_filetype_t);

  MTP::File* SetupFileTransfer(const char*,  uint64_t,  count_t, 
                               LIBMTP_filetype_t);

  void syncTrack(TagLib::FileRef, uint32_t parent); 
  void deleteObject(MTP::GenericObject*);
  void syncFile(const QString& path, uint32_t parent);

  void lockusb();
  void unlockusb();

  bool discoverCoverArt(const QString& in_path,
                        const QString& in_albumName,
                        QFileInfo* outFile);

  MtpDevice* _device;

  MtpWatchDog* _watchDog;
  QImage _icon;
  //delete this when done
  char* _iconBuf;
  QString _name;
  QString _serial;
  count_t _storageID;

  QQueue <GenericCommand*> _jobs;
  QMutex _jobLock;
  QWaitCondition _noJobsCondition;

  AlbumModel* _albumModel;
  DirModel* _dirModel;
  PlaylistModel* _plModel;

  static int progressWrapper(uint64_t const sent, uint64_t const total, 
                             const void* const data);
  /**
   * A private class to manage sorting of the Directory model
   * it sorts directories before files.
   */
  class MtpDirSorter  : public QSortFilterProxyModel
  {
  public:
    MtpDirSorter(QObject* parent = NULL) : QSortFilterProxyModel(parent) { }
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const
    {
      MTP::GenericObject* leftobj = (MTP::GenericObject*) left.internalPointer();
      MTP::GenericObject* rightobj = (MTP::GenericObject*) right.internalPointer();
      if (leftobj->Type() == MtpFolder && rightobj->Type() == MtpFolder)
      {
        MTP::Folder* leftFolder = (MTP::Folder*) leftobj;
        MTP::Folder* rightFolder = (MTP::Folder*) rightobj;
        return ( QString::fromUtf8(leftFolder->Name()  ) < 
                 QString::fromUtf8(rightFolder->Name() ) );
      }
      else if (leftobj->Type() == MtpFolder && rightobj->Type() == MtpFile)
        return true;
      else if (leftobj->Type() == MtpFile && rightobj->Type() == MtpFolder)
        return false;
      else if (leftobj->Type() == MtpFile && rightobj->Type() == MtpFile)
      {
        MTP::File* leftFile = (MTP::File*) leftobj;
        MTP::File* rightFile = (MTP::File*) rightobj;
        return ( QString::fromUtf8(leftFile->Name()  ) < 
                 QString::fromUtf8(rightFile->Name() ) );
       }
       assert(false);
      }
    };
    MtpDirSorter* _sortedFiles;
    QSortFilterProxyModel* _sortedAlbums;
    QSortFilterProxyModel* _sortedPlaylists;

};
#endif
