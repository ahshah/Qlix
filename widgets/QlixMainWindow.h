#ifndef __QLIXMAINWINDOW__
#define __QLIXMAINWINDOW__

#include <QMainWindow>
#include <QVector>
#include <QMenu>
#include <QStatusBar>
#include <QString>
#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QIcon>
#include "libmtp.h"
#include "mtp/MtpSubSystem.h"
#include "widgets/DeviceChooser.h"
#include "widgets/DeviceExplorer.h"


/* This class first displays the device chooser widget. Then it displays either 
 * the batch-mode transfer widget
 */
class QlixMainWindow : public QMainWindow
{
Q_OBJECT
public:
  QlixMainWindow(MtpSubSystem*);

public slots:
  void DeviceSelected(QMtpDevice*);
  
protected:
//  void closeEvent (QCloseEvent* eventh

private slots:

private:
  enum ToolbarView
  {
    Invalid,
    Albums,
    Playlists,
    Files,
    DeviceManager,
    Preferences
  };
  
  void setupActions();
  void setupToolBar();
  void setupStatusBar();
  void setupWatchDogConnections();
  void setupConnections();

  void setupAlbumActions();
  void setupPlaylistActions();
  void setupFileActions();

  MtpWatchDog* _watchDog;
  DeviceChooser* _deviceChooser;
  QMtpDevice* _currentDevice;
  DeviceExplorer* _deviceExplorer;

  QToolBar* _navBar;
  QStatusBar* _statusBar;
  QProgressBar* _progressBar;

  //View Actions
  QAction* _albumlistAction;
  QAction* _filelistAction;
  QAction* _playlistAction;
  QMenu* _playlistMenu;
  QAction* _showDeviceTracks;
  QAction* _showFileSystem;
  QActionGroup* _deviceExplorerActions;
  
  QAction* _manageDeviceAction;
  QAction* _preferencesAction;
  QAction* _showQueueSeparator;
  QAction* _showQueue;

  /*
  //File Actions
  QAction* _transferFile;
  QAction* _deleteFile;
  QAction* _newFolder;
  QAction* _deleteFolder;
  QVector <QAction*> _fileActionList;

*/
};

#endif
