/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __DEVICEBUTTON__
#define __DEVICEBUTTON__
#include <QToolButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QString>
#include <QSpacerItem>
#include "widgets/QMtpDevice.h"
#include "mtp/MtpStorage.h"
/** 
 * @class a button class that displays the device icon and auto connect box,
 * and depending on how many storage devices exist on the device, a combo box
*/
class DeviceButton : public QVBoxLayout
{
  Q_OBJECT
public:
  DeviceButton(QMtpDevice*, QWidget* temp = NULL);
  void RemoveCheck();

signals:
  void Checked(DeviceButton*, QMtpDevice* _device);
  void Selected(QMtpDevice*);

private slots:
  void stateChanged(int);
  void buttonClicked();
private:
  void setupConnections();
  QMtpDevice* _device;
  QCheckBox* _checkBox;
  QComboBox* _comboBox;
  QToolButton* _button;
};
#endif
