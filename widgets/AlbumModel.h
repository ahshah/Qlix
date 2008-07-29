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
 */

#ifndef __ALBUMMODEL__
#define __ALBUMMODEL__
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QPixmap>
#include <mtp/MtpDevice.h>
#include "types.h"
#include <QtDebug>
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <libmtp.h>
#include <QFont>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <QMutex>
#include <QMutexLocker>
#include "modeltest/modeltest.h"
#include <vector>
#include <map>

/**
 * @class This class wraps over the MTP::Album structures and provides a
 * hierarchy that displays tracks underneath albums
 */
class AlbumModel : public QAbstractItemModel
{
Q_OBJECT
public:
  AlbumModel(vector<MTP::Album*>, QObject* parent = NULL);
  ~AlbumModel();
  QModelIndex index(int, int, const QModelIndex& parent= QModelIndex()) const;
  QModelIndex parent (const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent= QModelIndex() ) const ;
  int columnCount(const QModelIndex& parent= QModelIndex() ) const;
  QVariant data(const QModelIndex& , int role = Qt::DisplayRole ) const;

public slots:
  void AddAlbum(MTP::Album*);
  void AddTrack(MTP::Track* in_track);
  void RemoveAlbum(MTP::Album*);
  void RemoveTrack(MTP::Track*);

private:
  std::vector<MTP::Album*> _albumList;

  bool discoverCoverArt(const QString&, const QString&, QFileInfo*);
  MtpDevice* _device;
  QMutex* _modelLock;
};
#endif
