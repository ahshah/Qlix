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

#ifndef QLIXPREFERENCES
#define QLIXPREFERENCES
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSettings>
#include <QToolButton>
#include <QAction>
#include <QDebug>
//This should be a private class but its not because MOC doesn't support nested classes
class QlixPreferences: public QWidget
{
  Q_OBJECT
public:
  QlixPreferences(QWidget* parent = NULL);

private slots:
  void saveSettings();
private:
  QSettings _settings;
  QToolButton* _saveButton;
  QAction* _saveAction;
  QGridLayout* _layout;

  QLabel* _defaultDeviceLabel;
  QLineEdit* _defaultDeviceLine;

  QLabel* _defaultDirPathLabel;
  QLineEdit* _defaultDirPathLine;
};
#endif
