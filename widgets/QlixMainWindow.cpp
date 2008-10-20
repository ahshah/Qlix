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

//TODO Use svn build numbers instead of arbitrary build numbers
#include "QlixMainWindow.h"


QlixMainWindow::QlixMainWindow(MtpSubSystem* in_subsystem)
{
  setMinimumSize(800,500);
  QWidget::setWindowIcon(QIcon(":/pixmaps/qlix.xpm"));

  QString buildStr = QString("Qlix (Build %1)").arg(BUILD_VERSION);
  QWidget::setWindowTitle(buildStr);

  _watchDog = new MtpWatchDog(in_subsystem);
  _deviceChooser = new DeviceChooser(this);

  setupWatchDogConnections();
  _watchDog->start();
  _deviceChooser->resize(frameSize());
  setCentralWidget(_deviceChooser);
  connect (_deviceChooser, SIGNAL(DeviceSelected(QMtpDevice*)),
           this, SLOT(DeviceSelected(QMtpDevice*)));
}

void QlixMainWindow::setupWatchDogConnections()
{
  connect(_watchDog, SIGNAL(DefaultDevice(QMtpDevice*)),
          this, SLOT(DeviceSelected(QMtpDevice*)), Qt::QueuedConnection);
  connect(_watchDog, SIGNAL(NewDevice(QMtpDevice*)),
          _deviceChooser, SLOT(AddDevice(QMtpDevice*)), Qt::QueuedConnection);
  connect(_watchDog, SIGNAL(NoDevices(bool)),
          _deviceChooser, SLOT(NoDevices(bool)), Qt::QueuedConnection);
}

void QlixMainWindow::DeviceSelected(QMtpDevice* in_device)
{
  setupStatusBar();
  _deviceChooser->hide();
  //TODO this is not such a great idea..
  // _watchDog->quit();
  _currentDevice = in_device;

  _deviceExplorer = new DeviceExplorer(in_device, this);
  _deviceExplorer->SetProgressBar(_progressBar);
  setupToolBar();
  setupActions();
  setupConnections();
  setCentralWidget(_deviceExplorer);
  _deviceExplorer->setContentsMargins(0,-10,0,0);
  _albumlistAction->trigger();
}

void QlixMainWindow::setupStatusBar()
{
  statusBar()->setMaximumHeight(24);
  statusBar()->setContentsMargins(0,-2,0,0);
  _progressBar = new QProgressBar();
  _progressBar->setTextVisible(false);
  _progressBar->setRange(0, 100);
  _progressBar->setMaximumSize(240, 20);
  _progressBar->setMinimumSize(240, 20);
  _progressBar->setAlignment(Qt::AlignRight);
  statusBar()->insertPermanentWidget(0, _progressBar, 0);
  _progressBar->hide();
}

void QlixMainWindow::setupToolBar()
{
  _navBar = new QToolBar();
  _navBar->setOrientation(Qt::Vertical);
  _navBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  _navBar->setFloatable(false);
/*
  _playlistTools = new QToolBar(_navBar);
  _playlistTools->setOrientation(Qt::Vertical);
  _playlistTools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  _playlistTools->setFloatable(false);
  _playlistTools->setIconSize(QSize(12,12));
  _playlistTools->setMovable(false);

  _fileTools = new QToolBar(_navBar);
  _fileTools->setOrientation(Qt::Vertical);
  _fileTools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  _fileTools->setFloatable(false);
  _fileTools->setIconSize(QSize(12,12));
  _fileTools->setMovable(false);
  */

  addToolBar(Qt::TopToolBarArea,_navBar);
  /*
  addToolBar(Qt::BottomToolBarArea, _albumTools);
  addToolBar(Qt::BottomToolBarArea, _playlistTools);
  addToolBar(Qt::BottomToolBarArea, _fileTools);
  */
}

void QlixMainWindow::setupActions()
{
  _deviceExplorerActions = new QActionGroup(NULL);
  _deviceExplorerActions->setExclusive(true);

  _albumlistAction= new QAction( QIcon(":/pixmaps/albumlist.png"), QString("View Albums"), NULL); 
  _albumlistAction->setCheckable(true);
  _deviceExplorerActions->addAction(_albumlistAction);

  _playlistAction = new QAction( QIcon(":/pixmaps/playlist.png"), QString("View Playlists"), NULL); 
  _playlistAction->setCheckable(true);
  _deviceExplorerActions->addAction(_playlistAction);



  _filelistAction = new QAction( QIcon(":/pixmaps/filelist.png"), QString("View Files"), NULL); 
  _filelistAction->setCheckable(true);
  _deviceExplorerActions->addAction(_filelistAction);
   
  _preferencesAction = new QAction( QIcon(":/pixmaps/preferences.png"), QString("Preferences"), NULL); 
  _preferencesAction->setCheckable(true);
  _deviceExplorerActions->addAction(_preferencesAction);

  _manageDeviceAction = new QAction( QIcon(":/pixmaps/managedevice.png"), QString("Manage Device"), NULL); 
  _manageDeviceAction->setCheckable(true);
  _deviceExplorerActions->addAction(_manageDeviceAction);

  _showQueueSeparator = new QAction(NULL);
  _showQueueSeparator->setSeparator(true);
  _showQueue = new QAction( QIcon(":/pixmaps/ShowQueue.png"), QString("Show Queue"), NULL); 
  _showQueue->setCheckable(true);


  _navBar->addAction(_albumlistAction);
  _navBar->addAction(_playlistAction);
  _navBar->addAction(_filelistAction);
  _navBar->addAction(_manageDeviceAction);
  _navBar->addAction(_preferencesAction);
  _navBar->addAction(_showQueueSeparator);
  _navBar->addAction(_showQueue);

  _navBar->setIconSize(QSize(32,32));

//  setupAlbumActions();
//  setupPlaylistActions();
//  setupFileActions();
}
/*


void QlixMainWindow::showAlbumActions()
{
  _playlistTools->hide();
  _fileTools->hide();
  _albumTools->show();
  _currentView = Albums;
  return;
}

void QlixMainWindow::hideAlbumActions()
{}

void QlixMainWindow::showPlaylistActions()
{
  _albumTools->hide();
  _fileTools->hide();
  _playlistTools->show();
  _currentView = Playlists;
  return;
}

void QlixMainWindow::hidePlaylistActions()
{}


void QlixMainWindow::showFileActions()
{
  _playlistTools->hide();
  _albumTools->hide();
  _fileTools->show();
  _currentView = Files;
  return;
}

void QlixMainWindow::hideFileActions()
{
  for (int i =0; i < _fileActionList.size(); i++)
    _fileActionList[i]->setVisible(false);
}
*/
void QlixMainWindow::setupConnections()
{
  //The following connections change DeviceExplorer's views
  //And show the toolbar
  //And show the right actions
  connect(_albumlistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowAlbums()));

  connect(_playlistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowPlaylists()));
  
  connect(_filelistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowFiles()));
 
  //The following show the preferences menu and the device manager
  //And hides the toolbar

  connect(_manageDeviceAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowDeviceManager()));
  connect(_preferencesAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowPreferences()));

}
