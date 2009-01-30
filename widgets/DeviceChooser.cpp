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

#include "widgets/DeviceChooser.h"
/*
 * Creates a new DeviceChooser widget by first creatng a noDevice widget
 * and then creating container widgets for detected devices
 * Care should be taken with the no device widget as it is destroyed after
 * devices are added and should be recreated when devices are removed..
 */
DeviceChooser::DeviceChooser(QWidget* parent) : QScrollArea(parent)
{
  createNoDeviceWidget();
  createDetectDevicesWidget();
  initialize();
}

/*
 * Initializes a button for each device detected, or displays the No devices
 * detected screen
 */
void DeviceChooser::initialize()
{
  _chooserGroupBox = new QGroupBox("Choose a device");
  _chooserLayout = new QGridLayout(_chooserGroupBox);

  _chooserLayout->setVerticalSpacing(40);
  _chooserLayout->setColumnMinimumWidth(0, 160);

  QScrollArea::setWidgetResizable(true);
  QScrollArea::setWidget(_detectDevicesWidget);
  return;
}

/**
 * This slot iterates over all buttons and unchecks those that are not equal
 * to selected
 * @param selected the exclusively selected DeviceButton
 */
void DeviceChooser::ExclusivelySelected(DeviceButton* selected)
{
  for (int i = 0; i < _deviceButtons.size(); i++)
  {
    if (_deviceButtons[i] != selected)
      _deviceButtons[i]->RemoveCheck();
  }
}

/*
 * A function stub that might be needed for later use
 */
void DeviceChooser::Reinitialize()
{ return; }

/*
 * Adds a DeviceButton based on the passed device to this container widget
 * @param in_device the device to add a button for
 */
void DeviceChooser::AddDevice(QMtpDevice* in_device)
{
  _devices.push_back(in_device);
  DeviceButton* newDevice = new DeviceButton(in_device);
  _deviceButtons.push_back(newDevice);
  count_t row;
  count_t col;

  row = ((_deviceButtons.size() -1) / 3);

  col = (_deviceButtons.size() -1) % 3;

  _chooserLayout->addLayout(newDevice, row, col, 1, 1);

  if (_deviceButtons.size() > 1)
    _chooserLayout->setColumnMinimumWidth(1, 160);
  if (_deviceButtons.size() > 2)
    _chooserLayout->setColumnMinimumWidth(2, 160);
#ifdef QLIX_DEBUG
  qDebug() << "Added a new device!";
#endif
  if (_devices.size() == 1)
    QScrollArea::setWidget(_chooserGroupBox);
  QScrollArea::setWidgetResizable(true);
  //setup connections for the last button as it was pushed to the vector
  setupConnections(_deviceButtons.size()-1);
}


void DeviceChooser::NoDevices(bool in_dbusWorking)
{
  qDebug() << "Is dbus working: " << in_dbusWorking;
  if (in_dbusWorking)
    _noDeviceWidget->SetDBusSearch();
  QScrollArea::setWidget(_noDeviceWidget);
}

/**
 * Creates the noDevice  widget
 */
void DeviceChooser::createNoDeviceWidget()
{
  _noDeviceWidget = new NoDevice(true);
}


/**
 * Creates the Detecting Devices widget
 */
void DeviceChooser::createDetectDevicesWidget()
{
  _detectDevicesWidget = new NoDevice(false);
}

/*
 * Uncommented because of development
 */
void DeviceChooser::deviceCountChanged()
{ }

/*
 * Connects a button check box to the ExclusivelySelected() slot
 * @param idx the index of the button to connect
 */
void DeviceChooser::setupConnections(count_t idx)
{
  assert (idx < _deviceButtons.size());
  QObject::connect(_deviceButtons[idx],
                   SIGNAL(Checked(DeviceButton*) ),
                   this,
                   SLOT(ExclusivelySelected(DeviceButton*)));
  QObject::connect(_deviceButtons[idx], SIGNAL(Selected(QMtpDevice*)),
                   this, SIGNAL(DeviceSelected(QMtpDevice*)));
}
