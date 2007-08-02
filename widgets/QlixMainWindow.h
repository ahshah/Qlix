//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __QLIXMAINWINDOW__
#define __QLIXMAINWINDOW__
#include "FileExplorer.h"
#include "DeviceExplorer.h"
#include "DeviceConsole.h"
#include <QWidget>
#include <QGridLayout>
#include <QSplitter>
#include <QMainWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QProgressBar>
#include <iostream>
#include <QSpacerItem>
using namespace std;
class QlixMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QlixMainWindow ();
    ~QlixMainWindow();
protected:

private slots:
    void connectDevice();

private:

/* Displayed Widgets */
    DeviceExplorer* _deviceExp;
    FileExplorer* _fileExp;
    DeviceConsole* _console; 
    QWidget*       _mainWidget;

/* Required Layout objects */
    QGridLayout* _layout;
    QSplitter* _splitter;
    QList<int>      _widgetSizes;
    QWidget* _parent;

/* Menu and ToolBars */
    QActionGroup* _alignmentGroup;
    QStatusBar* _statusBar;
    QProgressBar* _progressBar;
    QToolBar* _mainToolBar;
    QSpacerItem* _progressSpacer;
    QMenu* _menuMenu;
    QAction* _exitAct;
    QAction* _aboutAct;
    QAction* _connectAct;

    void setupFileList (void);
    void setupDeviceList (void);
    void setupConsole (void);
    void setupLayoutAndSplits ( void );
    void createToolBars();
    void createMenus();
    void createActions();
    void setupConnections ( void );
};
#endif
