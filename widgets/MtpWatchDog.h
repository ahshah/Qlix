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
