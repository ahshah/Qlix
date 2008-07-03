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
  QlixPreferences(QObject* parent = NULL);

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
