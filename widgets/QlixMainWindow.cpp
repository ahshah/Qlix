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
        return;
    }

    cout << "disconnected.. connecting" <<endl;
    _deviceExp->ConnectDevice();
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
    _console->AddAction(_connectAct);
    cout << "Added Action" << endl;
//    _mainToolBar->setMaximumHeight(25);
//    _layout->addWidget(_console);
//        _splitter->setCollapsible(2, true);
}

void QlixMainWindow::setupLayoutAndSplits ( void )
{
/*
    _deviceExp->setContentsMargins(-10, -10, -10, -10);
    _deviceExp->setContentsMargins(-10, -10, -10, -10);
    _fileExp->setContentsMargins(-10, -10, -10, -10);
*/
    _console->setContentsMargins  (-8, -15, -5, -18);
    _fileExp->setContentsMargins  (-8, -8, -5, -8);
    _deviceExp->setContentsMargins(-8, -5, -5, -9);
    _statusBar->setContentsMargins(-8, -3, -5, 0);
//    _progressBar->setContentsMargins(0,-20,0,-20);
    _layout->setSpacing(0);
    _layout->addWidget(_console);
    _splitter->addWidget(_fileExp);
    _splitter->addWidget(_deviceExp);
/*
    _layout->addWidget(_console);
    _layout->addWidget(_deviceExp);
    _layout->addWidget(_fileExp);
    */
//    _widgetSizes.push_back(200);
//    _widgetSizes.push_back(200);
//    _widgetSizes.push_back(100);
//    _splitter->setSizes(_widgetSizes);
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

/*
    _mainToolBar = new QToolBar("Main ToolBar", this);
    _mainToolBar->setIconSize(QSize(16,16));
    QSizePolicy tempSize(QSizePolicy::Minimum,QSizePolicy::Minimum);
    _mainToolBar->setSizePolicy(tempSize);
    */
//    _mainToolBar->addWidget(_progressBar);
//    _layout->addWidget(_mainToolBar);
}

void QlixMainWindow::createMenus()
{
    _menuMenu = menuBar()->addMenu("Menu");
    _menuMenu->addAction(_exitAct);
    //_menuMenu->setFixedHeight(10);
}

void QlixMainWindow::createActions()
{
    _exitAct = new QAction("Exit", this);
    _exitAct->setShortcut(tr("Ctrl+Q"));
    _exitAct->setStatusTip("Quit Qlix");
    connect(_exitAct, SIGNAL(triggered() ), this, SLOT(close()));

    _connectAct = new QAction(QIcon(":/pixmaps/connect.png"),"Connect", this);
    _connectAct->setShortcut(tr("Ctrl+C"));
    _connectAct->setCheckable(true);
    _connectAct->setStatusTip("Connect to first MTP-enabled device");
//    _connectAct->set
    connect(_connectAct, SIGNAL(triggered() ), this, SLOT(connectDevice()));
}

void QlixMainWindow::setupConnections ( void )
{
    _deviceExp->SetFSFileView(_fileExp->GetFSFileView());
    _fileExp->SetDeviceFileView( _deviceExp->GetDeviceFileView() );
}

