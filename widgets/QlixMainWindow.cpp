#include <QlixMainWindow.h>
QlixMainWindow::QlixMainWindow ()
{
    QWidget* _mainWidget = new QWidget();
    setCentralWidget(_mainWidget);
    _layout = new QVBoxLayout(_mainWidget);
//    _layout->addStretch(1);
    _splitter = new QSplitter(_mainWidget);
    _splitter->setOrientation(Qt::Vertical);
    setMinimumSize(800,500);

    createActions();
    createToolBars();
    setupFileList();
    setupDeviceList();
    setupConsole();
    setupLayoutAndSplits();
    setupConnections();

    _mainWidget->setLayout(_layout);

}

QlixMainWindow::~QlixMainWindow()
{
}
//protected:

//private slots:
void QlixMainWindow::connectDevice()
{
    if ( !_connectAct->isChecked() ) //then you are connected..
    {
        cout << "already connected.. disconnecting" <<endl;
        _deviceExp->DisconnectDevice();
        _connectAct->setIcon(QIcon(":/pixmaps/disconnected.png"));
        return;
    }

    cout << "disconnected.. connecting" <<endl;
    _deviceExp->ConnectDevice();
    _connectAct->setIcon(QIcon(":/pixmaps/connected.png"));
}

//private:
void QlixMainWindow::setupFileList (void)
{
    _fileExp= new FileExplorer();
}

void QlixMainWindow::setupDeviceList (void)
{
    _deviceExp = new DeviceExplorer();
}

void QlixMainWindow::setupConsole (void)
{
    _console = new DeviceConsole();
    _deviceExp->SetDeviceConsole(_console);
    _console->SetStatusAndProgressBar(_statusBar, _progressBar);
    _console->SetConnectAction(_connectAct);
/* Not implemented yet
    _console->SetSettingsAction(_editSettingsAct);
*/
    cout << "Added Action" << endl;
}

void QlixMainWindow::setupLayoutAndSplits ( void )
{
    _console->setContentsMargins  (-8, -15, -5, -18);
    _fileExp->setContentsMargins  (-8, -8, -5, -8);
    _deviceExp->setContentsMargins(-8, -5, -5, -9);
    _statusBar->setContentsMargins(-8, -3, -5, 0);
    _layout->setSpacing(0);
    _layout->addWidget(_console);
    _splitter->addWidget(_fileExp);
    _splitter->addWidget(_deviceExp);
    _layout->addWidget(_splitter);
}

void QlixMainWindow::createToolBars()
{
    _statusBar = statusBar(); 
    QFont font("Sans", 9);
    _statusBar->setFont(font);

    _progressSpacer = new QSpacerItem(700,25);
    _progressBar = new QProgressBar(_statusBar);
    _progressBar->setMaximumWidth(100);
    _progressBar->setMaximumHeight(15);
    _statusBar->insertPermanentWidget(0, _progressBar, 0);
}

void QlixMainWindow::createMenus()
{
    _menuMenu = menuBar()->addMenu("Menu");
    _menuMenu->addAction(_exitAct);
    _menuMenu->addAction(_connectAct);
    /* not implemented yet
    _menuMenu->addAction(_editSettingsAct);
    */
    //_menuMenu->setFixedHeight(10);
}

void QlixMainWindow::editSettings() 
{
    qDebug() << "Edit settings stub!" << endl;
}

void QlixMainWindow::createActions()
{
    _exitAct = new QAction("Exit", this);
    _exitAct->setShortcut(tr("Ctrl+Q"));
    _exitAct->setStatusTip("Quit Qlix");
    connect(_exitAct, SIGNAL(triggered() ), this, SLOT(close()));

    _connectAct = new QAction(QIcon(":/pixmaps/disconnected.png"),"Connect", this);
    _connectAct->setShortcut(tr("Ctrl+C"));
    _connectAct->setCheckable(true);
    _connectAct->setStatusTip("Connect to first MTP-enabled device");
/* Not implemented yet
    _editSettingsAct = new QAction(QIcon(":/pixmaps/settings.png"),"Settings", this);
    _editSettingsAct->setShortcut(tr("Ctrl+S"));
    _editSettingsAct->setStatusTip("Edit Qlix settings");
    connect(_editSettingsAct, SIGNAL(triggered()), this, SLOT(editSettings()));
*/
    connect(_connectAct, SIGNAL(triggered() ), this, SLOT(connectDevice()));
}

void QlixMainWindow::setupConnections ( void )
{
    _deviceExp->SetFSFileView(_fileExp->GetFSFileView());
    _fileExp->SetDeviceFileView( _deviceExp->GetDeviceFileView() );
}

