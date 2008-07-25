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
