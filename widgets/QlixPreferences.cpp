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

#include "QlixPreferences.h"
QlixPreferences::QlixPreferences(QWidget* parent) : QWidget(parent)
{
  _layout = new QGridLayout(this);
  _defaultDeviceLabel = new QLabel("Default Device:");
  _defaultDeviceLine = new QLineEdit(_settings.value("DefaultDevice").toString());

  _defaultDirPathLabel = new QLabel("Default Filesystem Directory:");
  _defaultDirPathLine = new QLineEdit(_settings.value("FSDirectory").toString());

  _saveButton = new QToolButton();
  _saveAction = new QAction(QString("Save settings"), NULL);
  _saveButton->setDefaultAction(_saveAction);
  connect(_saveAction, SIGNAL(triggered(bool)), this, SLOT(saveSettings()));

  _layout->addWidget(_defaultDeviceLabel, 0, 0);
  _layout->addWidget(_defaultDeviceLine, 0, 1);
  _layout->addWidget(_defaultDirPathLabel, 1, 0);
  _layout->addWidget(_defaultDirPathLine, 1, 1);
  _layout->addWidget(_saveButton, 2,1);

}

void QlixPreferences::saveSettings()
{
  _settings.setValue("DefaultDevice", _defaultDeviceLine->text());
  _settings.setValue("FSDirectory", _defaultDirPathLine->text());
  _settings.sync();
  qDebug() << "Settings saved!" <<endl;
}
