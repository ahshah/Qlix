#include "DeviceExplorer.h"
//TODO context/selection sensative context menus..
/**
 * Constructs a new DeviceExplorer
 * @param in_device the device whos database to represent
 * @param parent the parent widget of this widget, should QlixMainWindow
 */
DeviceExplorer::DeviceExplorer(QMtpDevice* in_device, QWidget* parent) :
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
  _fsDeviceSplit->addWidget(_deviceView);
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

  _deviceView->addActions(_commonDeviceActions->actions());
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
    _fsView->show();
    _deviceView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }//  _sortedModel->setSourceModel(_albumModel);
  if (_deviceView->model() != _albumModel)
    _deviceView->setModel(_albumModel);
    _deviceView->setStyleSheet("QTreeView::branch:!adjoins-item, QTreeView::branch:!has-children:open{ background: none} QTreeView::branch:has-children:closed{ image: url(:/pixmaps/TreeView/branch-closed.png)} QTreeView::branch:has-children:open{ image: url(:/pixmaps/TreeView/branch-open.png)}"); 
/*    //To be continued
 *    _deviceView->setStyleSheet("QTreeView::branch{ background: none} \
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
  _deviceView = new QTreeView();
  _deviceView->setModel(_albumModel);
  _deviceView->setSelectionBehavior(QAbstractItemView::SelectRows);
  _deviceView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _deviceView->setSortingEnabled(true);
  _deviceView->sortByColumn(0, Qt::AscendingOrder);
  _deviceView->header()->hide();
  _deviceView->setAlternatingRowColors(true);
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
    _fsView->show();
    _deviceView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }//  _sortedModel->setSourceModel(_plModel);

  if (_deviceView->model() != _plModel)
    _deviceView->setModel(_plModel);
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
    _fsView->show();
    _deviceView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }
  if (_deviceView->model() != _dirModel)
    _deviceView->setModel(_dirModel);
  _deviceView->setStyleSheet("");
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
    updateDeviceSpace();
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
  updateDeviceSpace();
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
    _deviceView->hide();
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
    _deviceView->hide();
    _fsView->hide();
    _queueView->hide();
    _deviceManagerWidget->show();
    _otherWidgetShown = true;
  }
}

void DeviceExplorer::setupConnections()
{
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
  /*
  connect(_device, SIGNAL(AlbumCreated(MTP::Album*)),
          temp, SLOT(Beep()), Qt::QuuuedConnection);
  */
  /*
  connect(_device, SIGNAL(CreatedAlbum(MTP::Album*)),
          _albumModel, SLOT(invalidate()), Qt::BlockingQueuedConnection);

*/

  qDebug() << "_albumModel->thread() : " << _albumModel->sourceModel()->thread();
  connect(_device, SIGNAL(CreatedAlbum(MTP::Album*)),
          _albumModel->sourceModel(), SLOT(AddAlbum(MTP::Album*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(AddedTrackToAlbum(MTP::Track*)),
          _albumModel->sourceModel(), SLOT(AddTrack(MTP::Track*)),
          Qt::BlockingQueuedConnection);
/*
  connect(_device, SIGNAL(AddedTrackToAlbum(MTP::Track*)),
          _albumModel, SLOT(invalidate()),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(RemovedTrack(MTP::Track*)),
          _albumModel, SLOT(invalidate()), Qt::BlockingQueuedConnection);
  */

  connect(_device, SIGNAL(RemovedTrack(MTP::Track*)),
          _albumModel->sourceModel(), SLOT(RemoveTrack(MTP::Track*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(RemovedAlbum(MTP::Album*)),
          _albumModel->sourceModel(), SLOT(RemoveAlbum(MTP::Album*)),
          Qt::BlockingQueuedConnection);

  qDebug() << "_dirModel->sourceModel->thread() : " << _dirModel->sourceModel()->thread();
  connect(_device, SIGNAL(RemovedFolder(MTP::Folder*)),
          _dirModel->sourceModel(), SLOT(RemoveFolder(MTP::Folder*)),
          Qt::BlockingQueuedConnection);

  connect(_device, SIGNAL(RemovedFile(MTP::File*)),
          _dirModel->sourceModel(), SLOT(RemoveFile(MTP::File*)),
          Qt::BlockingQueuedConnection);

}

void DeviceExplorer::Beep(MTP::Track* in_track)
{
  qDebug() << "Beep from device Explorer";
  ((AlbumModel*)_albumModel->sourceModel())->AddTrack(in_track);
  _albumModel->invalidate();
}

void DeviceExplorer::updateDeviceSpace()
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

  qDebug() << "Free space reported: " << free;
  qDebug() << "Total space reported: " << total;
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
  _deviceView->setContextMenuPolicy(Qt::ActionsContextMenu);
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
    selctedModel = _deviceView->selectionModel();
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
  QItemSelectionModel* selectedModel = _deviceView->selectionModel();
  QModelIndexList idxList= selectedModel->selectedRows();
  if (idxList.empty())
  {
    qDebug() << "nothing selected!";
    return;
  }

  QAbstractItemModel* theModel = _deviceView->model();
  assert(theModel == _albumModel || theModel == _plModel ||
         theModel == _dirModel);
  idxList = removeIndexDuplicates(idxList, (QSortFilterProxyModel*)theModel);

  MTP::GenericObject* obj;

  QModelIndex temp;
  foreach(temp, idxList)
  {
    obj = (MTP::GenericObject*) temp.internalPointer();
    assert(temp.isValid());
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
  QItemSelectionModel* selectedModel = _deviceView->selectionModel();
  QModelIndexList idxList = selectedModel->selectedRows();
  if (idxList.empty())
  {
    qDebug() << "nothing selected!";
    return;
  }
  QAbstractItemModel* theModel = _deviceView->model();
  assert(theModel == _albumModel || theModel == _plModel ||
         theModel == _dirModel || theModel == _unsortedAlbumModel);
  idxList = removeIndexDuplicates(idxList, (QSortFilterProxyModel*)theModel);

  QModelIndex temp;
  foreach(temp, idxList)
  {
    MTP::GenericObject* obj = (MTP::GenericObject*) temp.internalPointer();

    if (!confirmDeletion() )
      continue;
    qDebug() << "Deleting object";

    if (obj->Type() != MtpFile && obj->Type() != MtpTrack)
    {
      if (!confirmContainerDeletion(obj) )
        continue;
    }

    assert(temp.isValid());
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
 * transfering and deleting files/tracks/folders.
 * @param in_list the list of indicies selected
 * @param in_model the model that in_list applies to
 */
QModelIndexList DeviceExplorer::removeIndexDuplicates(
                                const QModelIndexList& in_list, 
                                const QAbstractItemModel* in_model)
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
               tempObj->Type() == MtpAlbum || tempObj->Type() == MtpFolder);
      }
      else
        ret.push_back(first);
    }
  }
  qDebug() << "Found :" << dupCount << " duplicates" << endl;
  qDebug() << "ret size:" <<ret.size();
#ifdef ALBUMDEBUG
  QModelIndex temp;
  qDebug() << "__Selection order__";
  int i = 0;
  foreach(temp, ret)
  {
    QString tempOut = (((AlbumModel*)_albumModel->sourceModel())->data(temp, Qt::DisplayRole)).toString();
    qDebug()<< i << ": " << tempOut;
    i++;
  }
#endif
  return ret;
}

/**
 * This function creates a popup and requests the user to confirm the deletion 
 * of an object
 */
bool DeviceExplorer::confirmDeletion()
{
  if (QMessageBox::question(this, "Confirm Deletion",
                               "Pleaes confirm object deletion",
                               "&Delete", "&Cancel",
                               QString::null, 0, 1) == 0)
    return true;
  else
    return false;
}

/**
 * This function creates a popup and requests the user to confirm the deletion 
 * of a container object
 * @param in_obj the container object that is about to be deleted
 */
bool DeviceExplorer::confirmContainerDeletion(MTP::GenericObject* in_obj)
{
  if (QMessageBox::question(this, "Confirm Container Deletion",
                               QString("%1 is a container object, all contained \
                               objects will be deleted!").arg( QString::fromUtf8(in_obj->Name() ) ),
                               "&Delete", "&Cancel",
                               QString::null, 0, 1) == 0)
    return true;
  else
    return false;
}
