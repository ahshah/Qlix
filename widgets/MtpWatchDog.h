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

#ifndef __MTPWATCHDOG__
#define __MTPWATCHDOG__
#include <QThread>
#include <QMutex>
#include <QtDebug>
#include <QVector>
#include "mtp/MtpSubSystem.h"
#include "types.h"
#include "widgets/QMtpDevice.h"
#include "QSettings"

#ifdef LINUX_DBUS
#include <QtDBus>
#include <QDBusInterface>
#endif

class QMtpDevice;
/**
 * @class This class will in the future be used to handle events such as
 * device disconnencts and connects while the application is in the middle of
 * a transaction
 */
class MtpWatchDog : public QThread
{
  Q_OBJECT
public:
  MtpWatchDog (MtpSubSystem*, QObject* parent = NULL);
  void Lock();
  void Unlock();

signals:
  void DeviceCountChanged(count_t );
  void NewDevice(QMtpDevice* Device);
  void DefaultDevice(QMtpDevice* Device);
  void NoDevices(bool);


protected:
  void run();

private:
  QMutex _subSystemLock;
  MtpSubSystem* _subSystem;

  bool findDefaultDevice();
  void createDevices();
  count_t _deviceCount;
#ifdef LINUX_DBUS
  void setupDBUS();
  //QDBusConnection _systemBus;
private slots:
  void DeviceAdded(QString);
#endif
};
#endif
