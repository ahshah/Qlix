#include "QlixPreferences.h"
QlixPreferences::QlixPreferences(QObject* parent)
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
