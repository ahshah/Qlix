//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#include "DeviceConsole.h"
DeviceConsole::DeviceConsole (QWidget* parent ) : QFrame(parent)
{
    QPixmap temp (":/pixmaps/device.png");
    _deviceImage = temp.scaled(48,48,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _connectButton = new QToolButton(this);
    _connectButton->setIconSize(QSize(32,32));
    _connectButton->setText("Connect");
    _layout = new QGridLayout(this);
    _layout->setRowMinimumHeight(0, 20);
    _layout->setRowMinimumHeight(1, 20);
    _layout->addWidget(_connectButton,0, 0, 2, 2);
    setupWidget();
    setupLabels();
}

QPixmap* DeviceConsole::GetDeviceImage ( void )
{
    return &_deviceImage;
}

void DeviceConsole::resetRootImage()
{
    cout <<"Resetting root image" << endl;
    QString path = QDir::tempPath() + "DevIcon.png";
    QFileInfo icon(path);
    icon.refresh();
    if (!icon.exists())
    {
        cout << "File: " << icon.filePath().toStdString() << " Didn't exist no change" << endl;
        return;
    }
    QPixmap temp; 
    if (!getRawImage(path, temp))
        return;
    _deviceImage = temp.scaledToHeight(48,Qt::SmoothTransformation);
    _devicePicture->setPixmap(_deviceImage);
    _devicePicture->repaint();
    repaint();
}
void DeviceConsole::UpdateProgressBar(unsigned int percentage)
{
    _progressBar->setValue(percentage);

    if (_progressBar->isHidden())
        _progressBar->show();
    if (percentage == 100)
        _progressBar->hide();
}

void DeviceConsole::UpdateDeviceStats(quint64 in_total, quint64 in_free)
{
    double displayTotal_kb = in_total/1024;
    double displayTotal_mb = displayTotal_kb/1024;
    double displayTotal_gb = displayTotal_mb/1024;

    QString displaySize = QString("%1 GB").arg(displayTotal_gb, 0, 'f', 2, QLatin1Char(' ' ));
    if (displayTotal_gb < 1)
        displaySize = QString("%1 MB").arg(displayTotal_mb, 0, 'f', 2, QLatin1Char( ' ' ));
    if (displayTotal_mb < 1)
        displaySize = QString("%1 KB").arg(displayTotal_kb, 0, 'f', 2, QLatin1Char( ' ' ));
    if (displayTotal_kb < 1)
        displaySize = QString("%1 B").arg(in_total, 0, 'f', 2, QLatin1Char( ' ' ));

    QString tempTotal = QString("Total space: " + displaySize);
    _deviceTotalSpace->setText(tempTotal);
    
    displayTotal_kb = in_free/1024;
    displayTotal_mb = displayTotal_kb/1024;
    displayTotal_gb = displayTotal_mb/1024;

    displaySize = QString("%1 GB").arg(displayTotal_gb, 0, 'f', 2, QLatin1Char( ' ' ));
    if (displayTotal_gb < 1)
        displaySize = QString("%1 MB").arg(displayTotal_mb, 0, 'f', 2, QLatin1Char( ' ' ));
    if (displayTotal_mb < 1)
        displaySize = QString("%1 KB").arg(displayTotal_kb, 0, 'f', 2, QLatin1Char( ' ' ));
    if (displayTotal_kb < 1)
        displaySize = QString("%1 B").arg(in_free, 0, 'f', 2, QLatin1Char( ' ' ));

    QString tempFree = QString("Free space: " + displaySize);
    _deviceFreeSpace->setText(tempFree);
}


void DeviceConsole::SetStatusAndProgressBar(QStatusBar* in_status, QProgressBar* in_progress)
{
    _statusBar = in_status;
    _progressBar = in_progress;
    _progressBar->hide();
}

void DeviceConsole::setupWidget()
{
    _layout->setSpacing(0);
    setMinimumSize(200, 50);
//    setFrameStyle(QFrame::Panel | QFrame::Sunken);
//    setFrameShape(QFrame::HLine);
//    setLineWidth(1);
    //changeWidgetBackground();
}

void DeviceConsole::changeWidgetBackground()
{
    QColor temp(0,100,255);
    QColor white(255,255,255);
    QPalette palette;
    setBackgroundRole(QPalette::Background);
    setForegroundRole(QPalette::Foreground);
    palette.setColor(backgroundRole(), temp); 
    palette.setColor(foregroundRole(), white); 
    setPalette(palette);
    setAutoFillBackground(true);
}

void DeviceConsole::setupLabels()
{
    QFont font("Sans", 8);
    _devicePicture = new QLabel(this);
    _devicePicture->setPixmap(_deviceImage);

    _deviceFreeSpace= new QLabel(this);
    _deviceFreeSpace->setText("Free space:");
    _deviceFreeSpace->setFont(font);
    _deviceFreeSpace->setAlignment(Qt::AlignBottom);

    _deviceTotalSpace = new QLabel(this);
    _deviceTotalSpace->setText("Total space:");
    _deviceTotalSpace->setFont(font);
    _deviceTotalSpace->setAlignment(Qt::AlignTop);

    QSpacerItem* _deviceSpacer = new QSpacerItem (700, 10 , QSizePolicy::MinimumExpanding);
    QSpacerItem* _tagSpacer = new QSpacerItem (10, 1, QSizePolicy::Fixed);
    QSpacerItem* _endTagSpacer = new QSpacerItem (40, 1, QSizePolicy::Minimum);

    _layout->addItem(_deviceSpacer, 0, 0, 1, 1);

    _layout->addWidget(_devicePicture, 0, 1, 2, 1);
    _layout->addItem(_tagSpacer, 0, 2, 1, 1);
    _layout->addWidget(_deviceFreeSpace, 0, 3);
    _layout->addWidget(_deviceTotalSpace, 1, 3);

    _layout->addItem(_endTagSpacer, 0, 4, 1, 1);
}

void DeviceConsole::AddAction(QAction* in_action)
{
    _connectButton->setDefaultAction(in_action);
    _connectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    qDebug() << in_action->iconText()<< endl;
}

bool DeviceConsole::getRawImage(const QString& in_path, QPixmap& in_pixmap)
{
    QFile img_file(in_path);
    img_file.open(QFile::ReadOnly);
    QByteArray buffer = img_file.readAll();
    Icon devIcon(buffer);
    if (devIcon.IsValid())
    {
        size_t temp = devIcon.GetBestImageSize();
        byte buf[temp];
        devIcon.Extract(buf);
        Dimensions dim = devIcon.GetDimensions();
        QImage tempImage( (uchar*)buf, dim.Width, dim.Height, QImage::Format_ARGB32);
        in_pixmap = (QPixmap::fromImage(tempImage));
        return true;
    }
    return false;
}


