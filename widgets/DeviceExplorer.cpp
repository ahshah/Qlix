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
 *
 *   TODO context/selection sensitive context menus..
 *   TODO serious bug in removeIndexDuplicates
 */

#include "DeviceExplorer.h"
/**
 * Constructs a new DeviceExplorer
 * @param in_device the device whos database to represent
 * @param parent the parent widget of this widget, should QlixMainWindow
 */
DeviceExplorer::DeviceExplorer(QMtpDevice* in_device, QWidget* parent) :
                              QWidget(parent),
                              _device(in_device),
                              _progressBar(NULL)
{
  qRegisterMetaType<count_t>("count_t");
  qRegisterMetaType<QModelIndex>("QModelIndex");
  _layout = new QGridLayout(this);

  _preferencesWidget = new QlixPreferences();
  _deviceManagerWidget = new QLabel("Device manager!");
  _preferencesWidget->hide();
  _deviceManagerWidget->hide();

  //Get the models
  _albumModel = _device->GetAlbumModel();
  //Album mode connections
  _unsortedAlbumModel = (AlbumModel*) _albumModel->sourceModel();

  _dirModel= _device->GetDirModel();
  _plModel = _device->GetPlaylistModel();
  setupDeviceView();
  _otherWidgetShown = false;
  _queueShown = false;


  setupFileSystemView();

  _fsDeviceSplit = new QSplitter();
  _queueSplit = new QSplitter();
  _queueSplit->setOrientation(Qt::Vertical);
  _fsDeviceSplit->setOrientation(Qt::Horizontal);

  //add the widgets to the layout
  _fsDeviceSplit->addWidget(_fsView);
  _fsDeviceSplit->addWidget(_albumView);
  _fsDeviceSplit->addWidget(_deviceRightView);
  _albumView->hide();
  _queueSplit->addWidget(_fsDeviceSplit);
  _queueSplit->addWidget(_queueView);

  _layout->addWidget(_queueSplit,1,0);
  _queueView->hide();
  _layout->addWidget(_preferencesWidget,1,1);
  _layout->addWidget(_deviceManagerWidget,1,2);

  _fsModel->setSorting( QDir::Name | QDir::DirsFirst);
  _fsModel->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden
                      |QDir::Readable);
  _fsModel->setLazyChildCount(true);

  QSettings settings;
  QString defaultDirLocation = settings.value("FSDirectory").toString();
  QFileInfo defaultDir(defaultDirLocation);
  if (!defaultDir.exists() || !defaultDir.isDir() || !defaultDir.isReadable())
    _fsView->setRootIndex(_fsModel->index(QDir::currentPath()));
  else
    _fsView->setRootIndex(_fsModel->index(defaultDirLocation));

  setupCommonTools();
  setupConnections();
  setupMenus();

  _deviceRightView->addActions(_commonDeviceActions->actions());
  _fsView->addActions(_commonFSActions->actions());
}


/**
 * Displays the AlbumModel and hides the preferences and devicemManager
 * widgets
 */
void DeviceExplorer::ShowAlbums()
{
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->hide();
    _albumView->hide();
    _fsView->show();
    _deviceRightView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }//  _sortedModel->setSourceModel(_albumModel);
  if (_deviceRightView->model() != _albumModel)
  {
    _albumView->hide();
    _fsView->show();
    _deviceRightView->setModel(_albumModel);
    _deviceRightView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }
//    _deviceRightView->setStyleSheet("QTreeView::branch:!adjoins-item, QTreeView::branch:!has-children:open{ background: none} QTreeView::branch:has-children:closed{ image: url(:/pixmaps/TreeView/branch-closed.png)} QTreeView::branch:has-children:open{ image: url(:/pixmaps/TreeView/branch-open.png)}");
/*    //To be continued
 *    _deviceRightView->setStyleSheet("QTreeView::branch{ background: none} \
                                QTreeView::branch:adjoins-item:!has-children{ image: url(:/pixmaps/TreeView/branch-end.png)} \
                                QTreeView::branch:has-children:closed{ image: url(:/pixmaps/TreeView/branch-closed.png)} \
                                QTreeView::branch:has-children:open{ image: url(:/pixmaps/TreeView/branch-open.png)}");
*/
}

void DeviceExplorer::setupToolBars()
{
  _tools = new QToolBar();
  _tools->setOrientation(Qt::Horizontal);
  _tools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  _tools->setFloatable(false);
  _tools->setIconSize(QSize(12,12));
  _tools->setMovable(false);
  _tools->setContentsMargins(0,0,0,-3);
  _fsDeviceSplit->setContentsMargins(0,-1,0,1);
// Incoroporates the progress bar as well

  _layout->addWidget(_tools, 0,0);
  _tools->addActions(_commonDeviceActions->actions());
}

void DeviceExplorer::setupDeviceView()
{
  _deviceRightView = new QTreeView();
  _deviceRightView->setModel(_albumModel);
  _deviceRightView->setSelectionBehavior(QAbstractItemView::SelectRows);
  _deviceRightView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _deviceRightView->setSortingEnabled(true);
  _deviceRightView->sortByColumn(0, Qt::AscendingOrder);
  _deviceRightView->header()->hide();
  _deviceRightView->setAlternatingRowColors(true);

  _albumView = new QTreeView();
  _albumView->setItemDelegate(new AlbumDelegate(_albumModel));
  _albumView->setModel(_albumModel);
  _albumView->setSelectionBehavior(QAbstractItemView::SelectRows);
  _albumView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _albumView->setSortingEnabled(true);
  _albumView->sortByColumn(0, Qt::AscendingOrder);
  _albumView->header()->hide();
  _albumView->setAlternatingRowColors(true);
}



bool DeviceExplorer::QueueState() { return _queueShown;}

/**
 * Displays the PlaylistModel and hides the preferences and devicemManager
 * widgets
 */
void DeviceExplorer::ShowPlaylists()
{
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->hide();
    _fsView->hide();
    _deviceRightView->show();
    _albumView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }//  _sortedModel->setSourceModel(_plModel);

  if (_deviceRightView->model() != _plModel)
  {
    _fsView->hide();
    _albumView->show();
    _deviceRightView->setModel(_plModel);
    _deviceRightView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }
}

/**
 * Displays the DirModel and hides the preferences and devicemManager
 * widgets
 */
void DeviceExplorer::ShowFiles()
{
//  _sortedModel->setSourceModel(_dirModel);
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->hide();
    _albumView->hide();
    _fsView->show();
    _deviceRightView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }
  if (_deviceRightView->model() != _dirModel)
  {
    _albumView->hide();
    _fsView->show();
    _deviceRightView->setModel(_dirModel);
    _deviceRightView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }
  _deviceRightView->setStyleSheet("");
}
void DeviceExplorer::UpdateProgressBar(const QString& label,
                                       count_t percent)
{
  if (!_progressBar)
    return;
  else if (_progressBar->isHidden())
    _progressBar->show();
  _progressBar->setFormat(label);
  _progressBar->setValue(percent);
  if (percent == 100)
  {
    UpdateDeviceSpace();
  }
}


/** Sets the DeviceExplorers progress bar to @param in_progressbar
 * @param in_progerssbar the progress bar to update when an event happens
 * @see UpdateProgressBar;
 *
 */
void DeviceExplorer::SetProgressBar(QProgressBar* in_progressbar)
{
  _progressBar = in_progressbar;
  _progressBar->setTextVisible(true);
  UpdateDeviceSpace();
}



/**
 * Displays the preferences widget and hides the filesystem and device views
 * Also hides the devicemanager widget if it is being shown
 */
void DeviceExplorer::ShowPreferences()
{
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->show();
  }

  else
  {
    _deviceRightView->hide();
    _fsView->hide();
    _queueView->hide();
    _preferencesWidget->show();
    _otherWidgetShown = true;
  }
}

void DeviceExplorer::ShowQueue( bool showQueue )
{
  if (showQueue)
  {
    _queueView->show();
    _queueShown = true;
  }
  else
  {
    _queueView->hide();
    _queueShown = false;
  }
}

/**
 * Displays the DeviceManager widget and hides the filesystem and device views
 * Also hides the preferences widget if it is being shown
 */
void DeviceExplorer::ShowDeviceManager()
{
  if (_otherWidgetShown)
  {
    _preferencesWidget->hide();
    _deviceManagerWidget->show();
  }
  else
  {
    _deviceRightView->hide();
    _fsView->hide();
    _queueView->hide();
    _deviceManagerWidget->show();
    _otherWidgetShown = true;
  }
}

void DeviceExplorer::setupConnections()
{
  /**********************************
   *
   *  Widget connections
   *
   **********************************/
  connect(_fsView, SIGNAL( doubleClicked ( const QModelIndex& )),
          this, SLOT(SwitchFilesystemDir(const QModelIndex&)));

  connect(_transferFromDevice, SIGNAL(triggered(bool)),
          this, SLOT(TransferFromDevice()));

  connect(_delete, SIGNAL(triggered(bool)),
          this, SLOT(DeleteFromDevice()));

  connect(_device, SIGNAL(UpdateProgress(QString, count_t)),
          this, SLOT(UpdateProgressBar(QString, count_t)));

  connect(_transferToDevice, SIGNAL(triggered(bool)),
          this, SLOT(TransferToDevice()));

  connect(_device, SIGNAL(RemovedTrack(MTP::Track*)),
          this, SLOT( UpdateDeviceSpace()));


  /**********************************
   *
   *  DirModel connections
   *
   **********************************/
  connect(_device, SIGNAL(RemovedFolder(MTP::Folder*)),
          _dirModel->sourceModel(), SLOT(RemoveFolder(MTP::Folder*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(RemovedFile(MTP::File*)),
          _dirModel->sourceModel(), SLOT(RemoveFile(MTP::File*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(AddedFile(MTP::File*)),
          _dirModel->sourceModel(), SLOT(AddFile(MTP::File*)),
          Qt::BlockingQueuedConnection);


  /**********************************
   *
   * AlbumModel connections
   *
   **********************************/
  connect(_device, SIGNAL(RemovedTrack(MTP::Track*)),
          _albumModel->sourceModel(), SLOT(RemoveTrack(MTP::Track*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(RemovedAlbum(MTP::Album*)),
          _albumModel->sourceModel(), SLOT(RemoveAlbum(MTP::Album*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(AddedTrack(MTP::Track*)),
          _albumModel->sourceModel(), SLOT(AddTrack(MTP::Track*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(CreatedAlbum(MTP::Album*)),
          _albumModel->sourceModel(), SLOT(AddAlbum(MTP::Album*)),
          Qt::BlockingQueuedConnection);


  /**********************************
   *
   * Playlist connections
   *
   **********************************/
  connect(_device, SIGNAL(AddedTrackToPlaylist(MTP::ShadowTrack*)),
          _plModel->sourceModel(), SLOT(AddedTrack(MTP::ShadowTrack*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(RemovedTrackFromPlaylist(MTP::ShadowTrack*)),
          _plModel->sourceModel(), SLOT(RemoveTrack(MTP::ShadowTrack*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(CreatedPlaylist(MTP::Playlist*)),
          _plModel->sourceModel(), SLOT(AddPlaylist(MTP::Playlist*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(RemovedPlaylist(MTP::Playlist*)),
          _plModel->sourceModel(), SLOT(RemovePlaylist(MTP::Playlist*)),
          Qt::BlockingQueuedConnection);


}

void DeviceExplorer::Beep(MTP::Track* in_track)
{
  qDebug() << "Beep from device Explorer";
  ((AlbumModel*)_albumModel->sourceModel())->AddTrack(in_track);
  _albumModel->invalidate();
}

void DeviceExplorer::UpdateDeviceSpace()
{
  if (!_progressBar)
    return;
  else if (_progressBar->isHidden())
    _progressBar->show();

  uint64_t total;
  uint64_t free;
  _device->FreeSpace(&total, &free);
  uint64_t used = total - free;

  double displayTotal_kb = total/1024;
  double displayTotal_mb = displayTotal_kb/1024;
  double displayTotal_gb = displayTotal_mb/1024;

  QString totalDisplaySize = QString("%1 GB").arg(displayTotal_gb, 0, 'f', 2,
                                              QLatin1Char(' ' ));
  if (displayTotal_gb < 1)
  {
      totalDisplaySize = QString("%1 MB").arg(displayTotal_mb, 0, 'f', 2,
                                         QLatin1Char( ' ' ));
  }
  else if (displayTotal_mb < 1)
  {
      totalDisplaySize = QString("%1 KB").arg(displayTotal_kb, 0, 'f', 2,
                                          QLatin1Char( ' ' ));
  }
  else if (displayTotal_mb < 1 && displayTotal_gb < 1)
  {
    totalDisplaySize = QString("%1 B").arg(total, 0, 'f', 2,
                                      QLatin1Char( ' ' ));
  }

  double displayUsed_kb = used/1024;
  double displayUsed_mb = displayUsed_kb/1024;
  double displayUsed_gb = displayUsed_mb/1024;

  QString usedDisplaySize = QString("%1 GB").arg(displayUsed_gb, 0, 'f', 2,
                                             QLatin1Char( ' ' ));
  if (displayUsed_gb < 1)
  {
      usedDisplaySize = QString("%1 MB").arg(displayUsed_mb, 0, 'f', 2,
                                         QLatin1Char( ' ' ));
  }
  else if (displayUsed_mb < 1)
  {
      usedDisplaySize = QString("%1 KB").arg(displayUsed_kb, 0, 'f', 2,
                                         QLatin1Char( ' ' ));
  }
  else if (displayUsed_mb <1 && displayUsed_gb < 1)
  {
      usedDisplaySize = QString("%1 B").arg(used, 0, 'f', 2,
                                        QLatin1Char( ' ' ));
  }
  QString label =  usedDisplaySize + " of " + totalDisplaySize ;
  count_t percent = (count_t)(((double) used / (double) total) * 100);

  _progressBar->setFormat(label);
  _progressBar->setValue(percent);
}

//Must be done before all others
void DeviceExplorer::setupCommonTools()
{
  _commonDeviceActions = new QActionGroup(this);
  _commonFSActions = new QActionGroup(this);
  _transferFromDevice = new QAction(
    QIcon(":/pixmaps/ActionBar/TransferFromDevice.png"),
    QString("Transfer From Device"), NULL);

  _commonDeviceActions->addAction(_transferFromDevice);

  _addToQueue= new QAction(
    QIcon(":/pixmaps/ActionBar/AddToQueue.png"),
    QString("Add to queue"), NULL);

  _commonDeviceActions->addAction(_addToQueue);

  _sync = new QAction(
    QIcon(":/pixmaps/ActionBar/SyncQueue.png"),
    QString("Sync"), NULL);

  _commonDeviceActions->addAction(_sync);

  QAction* _deleteSeperator = new QAction(NULL);
  _deleteSeperator->setSeparator(true);
  _commonDeviceActions->addAction(_deleteSeperator);

  _delete = new QAction(
    QIcon(":/pixmaps/ActionBar/DeleteFile.png"),
    QString("Delete"), NULL);
  _commonDeviceActions->addAction(_delete);

  _newFolder = new QAction(
    QIcon(":/pixmaps/ActionBar/NewFolder.png"),
    QString("New Folder"), NULL);

  _commonDeviceActions->addAction(_newFolder);

//FS actions
  _transferToDevice = new QAction(
                          QIcon(":/pixmaps/ActionBar/TransferFile.png"),
                          QString("Transfer"), NULL);
  _commonFSActions->addAction(_transferToDevice);
}


void DeviceExplorer::setupFileSystemView()
{
  //setup the filesystem view
  _fsModel = new QDirModel();
  _fsView = new QListView();
  _fsView->setModel(_fsModel);
  _fsView->setAlternatingRowColors(true);
  _fsView->setSelectionBehavior(QAbstractItemView::SelectRows);
  _fsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _queueView = new QListView();
  _queueView->setModel(_fsModel);
}


void DeviceExplorer::setupMenus()
{
  _fsView->setContextMenuPolicy(Qt::ActionsContextMenu);
  _deviceRightView->setContextMenuPolicy(Qt::ActionsContextMenu);
  //setContextMenuPolicy(Qt::ActionsContextMenu);
}

void DeviceExplorer::TransferToDevice()
{
  QList<QString> fileList;
  QItemSelectionModel* selectedModel = _fsView->selectionModel();
  QModelIndexList  idxList = selectedModel->selectedRows();
  if (idxList .empty())
  {
    qDebug() << "Nothing selected!";
    return;
  }

  while(!idxList.empty())
  {
    QString fpath = _fsModel->filePath(idxList.front());
    qDebug() << "Fpath is: " << fpath;
    fileList.push_back(fpath);
    idxList.pop_front();
  }
/*
    selctedModel = _deviceRightView->selectionModel();
    idxList = selectedModel->selectedRows();
*/
    while (!fileList.empty())
    {
      _device->TransferTrack(fileList.front());
      fileList.pop_front();
    }
}

void DeviceExplorer::TransferFromDevice()
{
  qDebug() << "called Transfer from device";
  QModelIndex idx = _fsView->rootIndex();

  if (!idx.isValid())
  {
    qDebug() << "Current directory is invalid" ;
    return;
  }

  QFileInfo info = _fsModel->fileInfo(idx);
  assert(info.isDir());
  if (!info.isWritable())
  {
    qDebug() << "Current directory is not writable:"  << info.canonicalFilePath();
    return;
  }

  QString transferPath = info.canonicalFilePath();
  QItemSelectionModel* selectedModel = _deviceRightView->selectionModel();
  QModelIndexList idxList= selectedModel->selectedRows();
  if (idxList.empty())
  {
    qDebug() << "nothing selected!";
    return;
  }

  QAbstractItemModel* theModel = _deviceRightView->model();
  assert(theModel == _albumModel || theModel == _plModel ||
         theModel == _dirModel);

  //TODO pass the returned list by memory rather than copy it in/out
  //TODO fix this part of the code for transfers from device
  //removeIndexDuplicates(idxList, (QSortFilterProxyModel*)theModel);

  MTP::GenericObject* obj;
  //TODO fix this
  QModelIndex temp;
  foreach(temp, idxList)
  {
    assert(temp.isValid());
    obj = (MTP::GenericObject*) temp.internalPointer();
    _device->TransferFrom(obj, transferPath);
  }
}

/**
 * This function retrieves the current selection from the UI and proceedes
 * to first confirm the deletion of each object selected and then issues an
 * asynchornous delete command to MtpDevice
*/
void DeviceExplorer::DeleteFromDevice()
{
  qDebug() << "Deleting form device..";
  QItemSelectionModel* selectedModel = _deviceRightView->selectionModel();
  QModelIndexList idxList = selectedModel->selectedRows();
  if (idxList.empty())
  {
    qDebug() << "nothing selected!";
    return;
  }
  QAbstractItemModel* theModel = _deviceRightView->model();
  assert(theModel == _albumModel || theModel == _plModel ||
         theModel == _dirModel || theModel == _unsortedAlbumModel);

  QGenericObjectList objList;
  removeIndexDuplicates(idxList, objList, (QSortFilterProxyModel*)theModel);

  MTP::GenericObject* obj;
  qDebug() << "Deleting: " << objList.size() << " objects";
  foreach(obj, objList)
  {
    if (!confirmDeletion() )
      continue;
    qDebug() << "Deleting object";
    assert(obj->Type() != MtpInvalid);
    if (obj->Type() != MtpFile && obj->Type() != MtpTrack &&
        obj->Type() != MtpShadowTrack)
    {
      if (!confirmContainerDeletion(obj) )
        continue;
    }
    _device->DeleteObject(obj);
  }
}

void DeviceExplorer::SwitchFilesystemDir(const QModelIndex& tmpIdx)
{
  if (!_fsView || !_fsModel)
    return;

  QFileInfo switchDir = _fsModel->fileInfo(tmpIdx);
  if (!switchDir.isDir() || !switchDir.exists())
    return;
  _fsView->setRootIndex (tmpIdx);
}

/*
QModelIndexList DeviceExplorer::removeAlbumDuplicates
                (const QModelIndexList& in_list)
{
  QModelIndexList ret;
  QModelIndexList::const_iterator iter = in_list.constBegin();
  for( ;iter != in_list.end(); iter++)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) (*iter).internalPointer();
    if (temp->Type() == MtpAlbum)
    {
      ret.push_back(*iter);
    }
  }

  qDebug() << "Potential problem Album count: " <<ret.size();
  iter = in_list.begin();
  int dupcount = 0;
  for( ;iter != in_list.end(); iter++)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) (*iter).internalPointer();
    if(temp->Type() == MtpAlbum)
      continue;
    QModelIndex parent = _albumModel->parent(*iter);
    QModelIndex  potentialParent;
    bool duplicate = false;
    foreach (potentialParent, ret)
    {
      if (parent == potentialParent)
      {
        duplicate = true;
        dupcount ++;
        break;
      }
    }
    if (!duplicate)
      ret.push_back(*iter);
  }
  qDebug() << "Duplicates found: " << dupcount;
  return ret;
}
*/

/**
 * This function removes duplicate selection implied by selecting a parent
 * widget and one or many of its children. The parent widget's selection
 * implies all widgets under it are selected, thus it gets precedence when
 * Transferring and deleting files/tracks/folders.
 * This function figures out which model should be used to remove index
 * duplicates from and calls the appropriate helper function to remove it.
 * @param in_list the list of indices selected
 * @param in_model the model that in_list applies to
 */
//TODO why is in_model a QAbstractItemModel instead of a QSortFilterProxy model?
//TODO why does the return list contain indices to the QSortModel rather than
//     the underlying model?
//TODO potential optimization using a map structure
void DeviceExplorer::removeIndexDuplicates(
                                QModelIndexList& in_list,
                                QGenericObjectList& out_list,
                                const QSortFilterProxyModel* in_model)

{
  QAbstractItemModel* theModel = in_model->sourceModel();
  if(in_model == _albumModel || in_model== _plModel)
    return removeTrackBasedIndexDuplicates(in_list, out_list, theModel);
  //TODO fix dirmodel is not a proxymodel?
  else if (in_model == _dirModel)
    return removeFileIndexDuplicates(in_list, out_list);
  else
    assert (false);
}

void DeviceExplorer::removeTrackBasedIndexDuplicates(
                                QModelIndexList& in_list,
                                QGenericObjectList& out_list,
                                const QAbstractItemModel* in_model)
{
  qDebug() << "Called removeTrackBasedIndexDuplicates";
  count_t dup = 0;

  if (in_model == _albumModel || in_model == _plModel)
  {
    QModelIndexList ParentList;
    for(QModelIndexList::iterator iter =in_list.begin(); iter != in_list.end();)
    {
      QModelIndex mapped = ((QSortFilterProxyModel*)in_model)->mapToSource(*iter);
      MTP::GenericObject* tempObj = (MTP::GenericObject*) mapped.internalPointer();
      if(tempObj->Type() == MtpAlbum || tempObj->Type() == MtpPlaylist)
      {
        ParentList.push_back(mapped);
        iter = in_list.erase(iter);
        continue;
      }
      //we iterate here as QList::erase out_listurns the next item inline
      iter++;
    }

    QModelIndex temp;
    //Place albums on out_listurn list
    foreach(temp, ParentList)
      out_list.push_back((MTP::GenericObject*)temp.internalPointer());

    //Find redundant tracks by iterating over each one and checking its parent
    //in the parent list
    for (QModelIndexList::iterator iter = in_list.begin();
                                                  iter != in_list.end(); iter++)
    {
      QModelIndex mapped = ((QSortFilterProxyModel*)in_model)->mapToSource(*iter);
      MTP::GenericObject* tempObj = (MTP::GenericObject*) mapped.internalPointer();
      if (in_model == _albumModel)
      {
        assert(tempObj->Type() == MtpTrack);
        bool skip = false;
        MTP::Track* tempTrack = (MTP::Track*) tempObj;
        for (QModelIndexList::const_iterator piter = ParentList.begin();
             piter != ParentList.end(); piter++)
        {
          MTP::Album* potentialParent = (MTP::Album*) piter->internalPointer();
          if (potentialParent == tempTrack->ParentAlbum())
           skip = true;
        }
        //if skip we increment dup, otherwise we place it on the removal queue
        if (skip)
          dup++;
        else
          out_list.push_back((MTP::GenericObject*)mapped.internalPointer());
      }
      else if (in_model == _plModel)
      {
        assert(tempObj->Type() == MtpShadowTrack);
        bool skip = false;
        MTP::ShadowTrack* tempStrack = (MTP::ShadowTrack*) tempObj;
        for (QModelIndexList::const_iterator piter = ParentList.begin();
             piter != ParentList.end(); piter++)
        {
          MTP::Playlist* potentialParent = (MTP::Playlist*)
                                            piter->internalPointer();
          if (potentialParent == tempStrack->ParentPlaylist())
           skip = true;
        }
        //if skip we increment dup, otherwise we place it on the removal queue
        if (skip)
          dup++;
        else
          out_list.push_back((MTP::GenericObject*)mapped.internalPointer());
      }
     }
    }
    qDebug() << "__Selection order__ | dup count:" << dup;
    int i = 0;
    MTP::GenericObject* temp;
    foreach(temp, out_list)
    {
        QString tempOut = temp->Name();
        qDebug()<< i << ": " << tempOut;
        i++;
    }
    //Uncomment this to enable sorted deletions, this will delete items from the
    //bottom of the containers list up
    //qSort(out_list.begin(), out_list.end(), modelLessThan);
}

/*
 * TODO consider how we deal with shadow tracks during deletions
 * [done] shadow tracks are dealt with in QMtpDevice upon playlist deletions
 *
 * TODO consider passing a QList that contains the list of tobe deleted objects
 * [ignored] we just convert the QModelIndicies to GenericObjects for deletion
 * keeping inline with the current way of doing things.
 *
 * TODO research how QMtpDevice handles deletions of complex containers
 * [done] QMtpDevice deletes complex containers by deleting the child contents
 * before deleting the container itself
 *
 * TODO this function is also called to removeIndexDuplicates when retrieving
 * files from the device, how should we handle that?
 * [Still valid] hrm..
 */

/**
 * This function removes index duplicates from the Directory view of Qlix.
 * Complex types are not expanded. That is if a file is selected that is
 * associated with a playlist, we replace the file's selection with the
 * playlist object, however we do not iterate over the playlist's child
 * ShadowTracks as they are iterated over in QMtpDevice upon deletion.
 *
 * This function separates files and folders. It then breadth first searches
 * its folders, iterating over all child files and replaces any
 * complex files with their associated type.
 * Finally it sorts the folders by depth and then joins the file and folder
 * lists in the output list provided.
 */
void DeviceExplorer::removeFileIndexDuplicates(QModelIndexList& in_list,
                                              QGenericObjectList& out_list)
{
  QModelIndex idx;
  QLinkedList<QModelIndex>dirList;
  QLinkedList<QModelIndex>albumList;
  QLinkedList<QModelIndex>fileList;

  //create a list of files and directories for deletion
  foreach(idx, in_list)
  {
    idx = _dirModel->mapToSource(idx);
    MTP::GenericObject* tempObj = (MTP::GenericObject*) idx.internalPointer();
    MtpObjectType type = QMTP::MtpType(idx);
    if (type ==  MtpFolder)
      dirList.push_back(idx);
    else if (type ==  MtpFile)
    {
      MTP::GenericFileObject* association;
      association = ((MTP::GenericFileObject*)idx.internalPointer())->Association();
      if (!association || association->Type() == MtpPlaylist)
        fileList.push_back(idx);
      else if (association->Type() == MtpAlbum)
        albumList.push_back(idx);
    }
    else
      assert(false);
  }
  int discoveredChildFiles = 0;
  int discoveredChildFolders= 0;

  //retrieve recursive implications
  QLinkedList<QModelIndex>::iterator iter;
  for(iter = dirList.begin(); iter != dirList.end(); iter++)
  {
    QModelIndex idx = *iter;
    if (!idx.isValid())
      continue;
    QModelIndex curParent = idx;
    count_t childCount = _dirModel->sourceModel()->rowCount(idx);
    for (count_t i = 0; i < childCount; i++)
    {
      QModelIndex childIdx = _dirModel->sourceModel()->index(i, 0, curParent);
      //should not happen
      if (!childIdx.isValid())
        assert(false);
      //Potential source of bugs,
      //Not sure if QLinkedList guarantees that the foreach clause extend to
      //dynamic looping caused by the following statement:
      if (QMTP::MtpType(childIdx) == MtpFolder)
      {
        dirList.push_back(childIdx);
        discoveredChildFolders++;
      }
      else if (QMTP::MtpType(childIdx) == MtpFile)
      {
        MTP::GenericFileObject* association;
        association = ((MTP::GenericFileObject*)childIdx.internalPointer())->Association();
        if (!association || association->Type() == MtpPlaylist ||
            association->Type() == MtpTrack)
          fileList.push_back(childIdx);
        else if (association->Type() == MtpAlbum)
          albumList.push_back(childIdx);
       discoveredChildFiles++;
      }
    }
  }
  qDebug() << "Discovered child files: " << discoveredChildFiles;
  qDebug() << "Discovered child folders: " << discoveredChildFolders;

  QMap<uint32_t, MTP::Folder*> dirMap;
  QMap<uint32_t, MTP::GenericFileObject*> fileMap;

  //Build folder map
  foreach(idx, dirList)
  {
    MTP::Folder* currentDir = (MTP::Folder*) idx.internalPointer();
    if (dirMap.contains(currentDir->ID()))
      continue;
    dirMap[currentDir->ID()] = currentDir;
  }

  //Build file map- first add all the albums, as they are containers and their
  //children might be selected

  foreach(idx, albumList)
  {
    MTP::File* currentFile = (MTP::File*) idx.internalPointer();
    assert(currentFile->Association()->Type() == MtpAlbum);

    //just to be safe..this should not be possible
    if (fileMap.contains(currentFile->ID()))
      assert(false);
    fileMap[currentFile->ID()] = currentFile;
  }

  /*
   * Here we replace files by there associated type
   * maybe we should call associated types "complex types"
   * We do not expand complex types any further than their association.
   * That is, should we find a MtpPlaylist, we do not add its ShadowTrack
   * children as they are automatically expanded in QMtpDevice
   */
  foreach(idx, fileList)
  {
    MTP::File* currentFile = (MTP::File*) idx.internalPointer();
    if (fileMap.contains(currentFile->ID()))
      continue;

    /*
     * If you are a file that is a complex type then we check to make sure you
     * are a playlist or a track. If you are a track then we make sure your
     * parent album is not already selected. If it is, we do not add the track
     * Otherwise we add the complex type to the map instead of the generic file
     */
    if (currentFile->Association())
    {
      MTP::GenericFileObject* currentObj = currentFile->Association();
      assert(currentObj->Type() == MtpPlaylist || currentObj->Type() == MtpTrack);
      if (currentObj->Type() == MtpTrack)
      {
          MTP::Album* parentAlbum = ((MTP::Track*) currentObj)->ParentAlbum();
          if (parentAlbum && fileMap.contains(parentAlbum->ID()))
            continue;
      }
      fileMap[currentObj->ID()] = currentObj;
    }
    else
    {
      assert(currentFile->Type() == MtpFile);
      fileMap[currentFile->ID()] = currentFile;
      continue;
    }
  }


/* no longer necessary
  QMap<uint32_t, MTP::GenericFileObject*>::iterator iter;
  for(iter = fileMap.begin(); iter != fileMap.end(); iter++)
  {
    if ((*iter)->Association())
      fileMap[(*iter)->ID()] = (*iter)->Association();
  }
*/

  dirList.clear();
  fileList.clear();
  MTP::Folder* genericFolder;
  MTP::GenericFileObject* genericFile;

  QList<MTP::Folder*> retDirList;
  foreach(genericFolder, dirMap)
    retDirList.push_back(genericFolder);

  MTP::Folder* tempstart = retDirList.front();
  qSort(retDirList.begin(), retDirList.end(), QMTP::MtpFolderLessThan);
  tempstart = retDirList.front();

  foreach(genericFile, fileMap)
  {
    out_list.push_back(genericFile);
    assert(genericFile->Type() != MtpInvalid);
  }
  foreach(genericFolder, retDirList)
  {
    out_list.push_back(genericFolder);
    assert(genericFolder->Type() != MtpInvalid);
    qDebug() << "Pushed folder: " << genericFolder->Name();
  }
}

/**
 * This function creates a popup and requests the user to confirm the deletion
 * of an object
 */
bool DeviceExplorer::confirmDeletion()
{
  return true;
  /*
  if (QMessageBox::question(this, "Confirm Deletion",
                               "Pleaes confirm object deletion",
                               "&Delete", "&Cancel",
                               QString::null, 0, 1) == 0)
    return true;
  else
    return false;
    */
}

/**
 * This function creates a popup and requests the user to confirm the deletion
 * of a container object
 * @param in_obj the container object that is about to be deleted
 */
bool DeviceExplorer::confirmContainerDeletion(MTP::GenericObject* in_obj)
{
  return true;
  /*
  QString msg = QString("%1 is a container object, all contained\
    objects will be deleted!").arg( QString::fromUtf8(in_obj->Name() ) );

  if (QMessageBox::question(this, "Confirm Container Deletion",
                               msg, "&Delete", "&Cancel",
                               QString::null, 0, 1) == 0)
    return true;
  else
    return false;
    */
}

bool DeviceExplorer::modelLessThan(const QModelIndex& left, const QModelIndex& right)
{
  if (left.row() < right.row())
    return false;
  else
    return true;
}


/*
{
  qDebug() << "Called removeIndexDuplicates";
  QModelIndexList ret;
  QModelIndexList tempList = in_list;
  count_t dupCount = 0;
  while (!tempList.empty())
  {
    QModelIndex first = tempList.front();
    tempList.pop_front();
    QModelIndex parent = first;
    bool found = false;
    while (parent.isValid())
    {
      QModelIndex rightOfFirst;
      QModelIndex leftOfFirst;
      foreach(rightOfFirst, tempList)
      {
        if (rightOfFirst == parent)
        {
          found = true;
          dupCount++;
          break;
        }
      }
      foreach(leftOfFirst, ret)
      {
        if (leftOfFirst == parent)
        {
          found = true;
          dupCount++;
          break;
        }
      }
      if (found)
        break;
      parent = in_model->parent(parent);
    }
    if (!found)
    {
      if (in_model != _unsortedAlbumModel)
      {
        QModelIndex mapped = ((QSortFilterProxyModel*)in_model)->mapToSource(first);
        ret.push_back(mapped);
        MTP::GenericObject* tempObj = (MTP::GenericObject*) mapped.internalPointer();
        assert(tempObj->Type() == MtpTrack ||  tempObj->Type() == MtpFile ||
               tempObj->Type() == MtpAlbum || tempObj->Type() == MtpFolder ||
               tempObj->Type() == MtpPlaylist ||
               tempObj->Type() == MtpShadowTrack);
      }
      else
        ret.push_back(first);
    }
  }
  qDebug() << "Found :" << dupCount << " duplicates" << endl;
  qDebug() << "ret size:" <<ret.size();
#define ALBUMDEBUG
#ifdef ALBUMDEBUG
#endif
  return ret;
}
*/
