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

#ifndef __DEVICECHOOSER__
#define __DEVICECHOOSER__
#include <QGridLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QToolButton>
#include <QSpacerItem>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QMetaType>
#include <QSettings>
#include <QVector>

#include <libmtp.h>
#include "types.h"
#include "mtp/MtpSubSystem.h"
#include "widgets/DeviceButton.h"
#include "widgets/MtpWatchDog.h"
#include "widgets/QMtpDevice.h"


typedef QVector<DeviceButton*> QButtonVector;

/**
 * @class This class displays the device chooser widget
 * which is a widget that display a series of buttons, one for each
 * device that has been detected by libMTP
 */
class  DeviceChooser : public QScrollArea
{
Q_OBJECT
public:
  DeviceChooser(QWidget*);
signals:
  void DeviceSelected(QMtpDevice* );
public slots:
  void ExclusivelySelected(DeviceButton*);
  void Reinitialize();
  void AddDevice(QMtpDevice*);
  void NoDevices(bool);

private slots:
  void deviceCountChanged();

private:
  //private forward declration
  class NoDevice;

  void addButton(DeviceButton*);
  void setupConnections(count_t idx);
  // Connects the WatchDog to this widget to inform it of updates
  void setupWatchDogConnections();
  void initialize();
  void createNoDeviceWidget();
  void createDetectDevicesWidget();
  void addButtonToLayout(DeviceButton*);
  void showNoDeviceWidget();
  void hideNoDeviceWidget();


  QVector <QMtpDevice*> _devices;

  /* Used when devices are detected */
  QButtonVector _deviceButtons;
  QGridLayout* _chooserLayout;
  QGroupBox*  _chooserGroupBox;
  QLabel*     _buildLabel;

  NoDevice* _noDeviceWidget;
  NoDevice* _detectDevicesWidget;


  /**
  * @class A private class to display the NoDevice Icon or alternatively
  * it shoulds the Detecting Devices Icon
  *
  */
  class NoDevice : public QWidget
  {
    public:
    NoDevice(bool nodevice, QWidget* parent = NULL) : QWidget(parent)
    {
      _layout = new QGridLayout();
      _top = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                              QSizePolicy::Expanding);
      _bottom = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                              QSizePolicy::Expanding);
      QString txt;
      if (nodevice)
        txt = QString("<h2>No devices detected!<h2>");
      else
        txt = QString("<h2>Detecting devices..<h2>");

      _text = new QLabel(txt);
      _text->setTextFormat(Qt::RichText);

      _image = new QLabel();
      if (nodevice)
        _image->setPixmap(QPixmap(":/pixmaps/noDevice.png"));
      else
        _image->setPixmap(QPixmap(":/pixmaps/DetectDevices.png"));

      _layout->addItem(_top, 0, 0, 1, -1);
      _layout->addWidget(_text, 1, 0, 1, -1, Qt::AlignCenter);
      _layout->addWidget(_image, 2, 0, 1, -1, Qt::AlignCenter);
      _layout->addItem(_bottom, 3, 0, 1, -1);
      setLayout(_layout);
    }
    /** This function changes the image to a recycle sign and tells the user
     * to connect a device as DBUS seems to be functioning
     */
    void SetDBusSearch()
    {
      QString txt ("<h2>No devices found. Please connect a device..<h2>");
      _text->setText(txt);
      _image->setPixmap(QPixmap(":/pixmaps/DetectDevices.png"));
    }

    private:
    QGridLayout* _layout;
    QSpacerItem* _top;
    QSpacerItem* _bottom;
    QLabel* _text;
    QLabel* _image;
    QLabel* _buildLabel;
  };
};
#endif
