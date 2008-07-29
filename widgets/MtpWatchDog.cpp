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

#include "widgets/MtpWatchDog.h"
  // Currently libmtp does not support polling 
  // In the future this will probably be supported by HAL/DBUS signals that
  // will inform us when a MTP device has been connected. At that point we would
  // ask LIBMTP to created a LIBMTP_device_t struct from a (potentially) USB port
  // number

/** 
 * Creates a new WatchDog over the given subsystem
 * @param in_subSystem the subsystem to watch over
 */
MtpWatchDog::MtpWatchDog(MtpSubSystem* in_subSystem, QObject* parent) :
                         _subSystem(in_subSystem),
                         _deviceCount(0)
{ }

/** 
 * Begin running the MtpWatchDog by polling LIBMTP for new devices
 */
void MtpWatchDog::run()
{
  Lock();
  _subSystem->Initialize();

  if (_subSystem->DeviceCount() == 0)
  {
#ifdef LINUX_DBUS
    setupDBUS();
    Unlock();
    exec();
#else
    emit NoDevices(false);
    Unlock();
    return;
#endif
  }
  else
  {
    qDebug() << "Device count > 0";
    if (!findDefaultDevice())
      createDevices();
    Unlock();
    return;
  }
}

/** 
 * One must lock up the WatchDog before working with the MTP subsystem
 */
void MtpWatchDog::Lock()
{
  _subSystemLock.lock();
}

/** 
 * One must unlock the WatchDog once done working with the MTP subsystem
 */
void MtpWatchDog::Unlock()
{
  _subSystemLock.unlock();
}

bool MtpWatchDog::findDefaultDevice()
{
  QSettings settings;
  QString defaultDev = settings.value("DefaultDevice").toString();

  for (count_t i = 0; i< _subSystem->DeviceCount() &&
                      !defaultDev.isEmpty(); i++)
  {
    if (QString::fromUtf8(_subSystem->Device(i)->SerialNumber()) == defaultDev)
    {
      QSettings settings;
      count_t defaultStorage = settings.value("DefaultStorage").toInt();
      QMtpDevice* threadedDev = new QMtpDevice(_subSystem->Device(i), this);
      threadedDev->SetSelectedStorage(defaultStorage);

//      threadedDev->moveToThread(QApplication::instance()->thread());

      connect(threadedDev, SIGNAL(Initialized (QMtpDevice*)),
              this, SIGNAL(DefaultDevice(QMtpDevice*)), Qt::QueuedConnection);
      assert(!threadedDev->isRunning());
      threadedDev->start();
      qDebug() << "Found the default device: " << defaultDev;
      return true;
    }
  }
  return false;
}

void MtpWatchDog::createDevices()
{
  for (count_t i = 0; i< _subSystem->DeviceCount(); i++)
  {
    QMtpDevice* threadedDev = new QMtpDevice(_subSystem->Device(i), this);
    threadedDev->moveToThread(QApplication::instance()->thread());
    connect(threadedDev, SIGNAL(Initialized (QMtpDevice*)),
            this, SIGNAL(NewDevice (QMtpDevice*)), Qt::QueuedConnection);
    assert(!threadedDev->isRunning());
    threadedDev->start();
  }
}

#ifdef LINUX_DBUS
void MtpWatchDog::setupDBUS()
{
  QDBusConnection _systemBus = QDBusConnection::systemBus();
  bool ret = _systemBus.connect("org.freedesktop.Hal", 
                                 "/org/freedesktop/Hal/Manager",
                                 "org.freedesktop.Hal.Manager",
                                 "DeviceAdded",
                                 this, SLOT(DeviceAdded(QString)));
  QDBusInterface hal("org.freedesktop.Hal",
                     "/org/freedesktop/Hal/Manager",
                     "org.freedesktop.Hal.Manager", _systemBus);

  bool dbusConnectionHealth =  (ret && _systemBus.isConnected() &&
                                hal.isValid());
  emit NoDevices(dbusConnectionHealth);
}

void MtpWatchDog::DeviceAdded(QString in_objRef)
{
  qDebug() << "connected: " << in_objRef;
  QDBusConnection _systemBus = QDBusConnection::systemBus();
  QDBusInterface usbDevice("org.freedesktop.Hal", in_objRef, 
                           "org.freedesktop.Hal.Device", _systemBus);

  QDBusReply<int> vendorReply = usbDevice.call("GetPropertyInteger",
                                                "usb.vendor_id");
  QDBusReply<int> productReply = usbDevice.call("GetPropertyInteger",
                                                "usb.product_id");
  QDBusReply<QStringList> protocolReply;
  protocolReply = usbDevice.call("GetPropertyStringList",
                              "portable_audio_player.access_method.protocols");

  QDBusReply<QString> protocolReply2;
  protocolReply2 = usbDevice.call("GetPropertyString",
                              "info.udi");
  if (vendorReply.isValid() && productReply.isValid())
  {
    QString vendorID = QString("%1").arg(vendorReply.value(), 0, 16);
    QString productID = QString("%1").arg(productReply.value(), 0, 16);
    qDebug() << "Vendor: " <<  vendorID;
    qDebug() << "Product: " <<  productID;
  }
  if(protocolReply.isValid())
  {
    qDebug() << "Protocol: " <<  protocolReply.value();
    qDebug() << "Protocol udi: " <<  protocolReply2.value();
  }
  Lock();
  if (_subSystem->DeviceCount() == 0)
  {
    _subSystem->Initialize();
   
    if (findDefaultDevice() )
    {
      Unlock();
      return;
    }
    else
     createDevices();
  }
  Unlock();
  //TODO potentially disconnect the DBUS connection
}
#endif
