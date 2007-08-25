//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DEVICECONSOLE__
#define __DEVICECONSOLE__
#include <QWidget>
#include <QPalette>
#include <QPainter>
#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include <QFileInfo>
#include <QDir>
#include <QProgressBar>
#include "Icon.h"
#include <iostream>
#include <QStatusBar>
#include <QToolButton>
#include <QString>
#include <QDebug>
#include <QAction>

using namespace std;
class DeviceConsole : public QFrame
{
    Q_OBJECT
public:
    DeviceConsole (QWidget* parent = NULL);
    QPixmap* GetDeviceImage ( void );
    void SetStatusAndProgressBar(QStatusBar*, QProgressBar*);
    void SetConnectAction(QAction* in_action);
    void SetSettingsAction(QAction* in_action);

public slots:
    void resetRootImage();
    void UpdateProgressBar(unsigned int percentage);
    void UpdateDeviceStats(quint64, quint64);
private:
    QPixmap _deviceImage;
    QGridLayout* _layout;
    QLabel* _devicePicture;
    QLabel* _deviceTotalSpace;
    QLabel* _deviceFreeSpace;

    void setupWidget();
    void changeWidgetBackground();
    void setupLabels();
    bool getRawImage(const QString& in_path, QPixmap& in_pixmap);
    QToolButton* _connectButton;
    QToolButton* _settingsButton;
    QProgressBar* _progressBar;
    QStatusBar* _statusBar;
};
#endif
