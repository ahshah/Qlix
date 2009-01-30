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

#include "widgets/DeviceButton.h"

/**
 * Creates a DeviceButton object
 * @param in_device The device that this button represents
 * @param parent The parent widget
 * @return Returns a DeviceButton object
 */
DeviceButton::DeviceButton (QMtpDevice* in_device, QWidget* parent) :
                            QVBoxLayout(parent),
                            _comboBox(NULL)
{
  _device = in_device;

  QString name = _device->Name();
  _checkBox = new QCheckBox("Connect on startup");
  _button = new QToolButton();
  const count_t deviceCount = in_device->StorageDeviceCount();
  if (deviceCount > 1)
  {
    _comboBox = new QComboBox();
    for (count_t i = 0; i < in_device->StorageDeviceCount(); i++ )
    {
      MtpStorage* storage_device = in_device->StorageDevice(i);
      assert(storage_device);
      QString description = QString::fromUtf8(storage_device->Description() );
      if (description.length() == 0)
        description = QString::fromUtf8(storage_device->VolumeID() );
      QVariant storageID = storage_device->ID();
      _comboBox->addItem(description, storageID);
    }
  }

  _button->setText(name);
  _button->setIcon(QIcon(QPixmap::fromImage(_device->IconImage(), Qt::ColorOnly)));
  _button->setMinimumSize(QSize(160,160));
  _button->setMaximumSize(QSize(160,160));
  _button->setIconSize(QSize(128,128));
  _button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  QSpacerItem* temp = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                            QSizePolicy::Expanding);
  addItem(temp);
  addWidget(_button);
  if (deviceCount > 1)
    addWidget(_comboBox);
  addWidget(_checkBox);
  temp = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                            QSizePolicy::Expanding);
  addItem(temp);
  setupConnections();
}

/**
 * Removes checked state of the check box if it is checked
 */
void DeviceButton::RemoveCheck()
{
  if (_checkBox->checkState() == Qt::Checked)
    _checkBox->setCheckState(Qt::Unchecked);
}

/**
 * Setup up the widgets internal connections
 * See also stateChanged()
 */
void DeviceButton::setupConnections()
{
  connect(_checkBox, SIGNAL(stateChanged(int)),
          this, SLOT(stateChanged (int)));
  connect(_button, SIGNAL(clicked(bool)),
          this, SLOT(buttonClicked()));
}

/**
 * Private slot that gets called when the button is clicked
 * it emits the Selected button
 */
void DeviceButton::buttonClicked()
{
  QSettings settings;
  count_t id;
  if (!_comboBox)
    id = _device->StorageDevice(0)->ID();
  else
    id = _comboBox->itemData(_comboBox->currentIndex()).toInt();

  _device->SetSelectedStorage(id);

//  settings.setStorage("DefaultStorage", id);
  settings.setValue("DefaultDevice", _device->Serial());
  settings.setValue("DefaultStorage", id);
  settings.sync();

  qDebug() << "Storing and syncing: " << _device->Serial();
  emit Selected(_device);
}

/**
 * Private slot that is called when the checkbox is checked
 * See also setupConnections()
 */
void DeviceButton::stateChanged(int in)
{
  if (in == Qt::Checked)
    emit Checked(this);
}
