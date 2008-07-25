#include <libmtp.h>
#include <QMainWindow>
#include <QApplication>
#include <QPlastiqueStyle>
#include <QtDebug>
#include <signal.h>



#include "mtp/MtpSubSystem.h"
#include "widgets/QlixMainWindow.h"
#ifdef LINUX_SIGNALS
#include "linuxsignals.h"
#endif

MtpSubSystem _subSystem;
int main(int argc, char* argv[])
{
//  installSignalHandlers();

  QCoreApplication::setOrganizationName("QlixIsOSS");
  QCoreApplication::setOrganizationDomain("Qlix.berlios.de");
  QCoreApplication::setApplicationName("Qlix");
  QSettings settings;
  QString ret = settings.value("DefaultDevice").toString();
  QApplication app(argc, argv);
  app.setStyle("Plastique");
  //app.setStyleSheet("QTreeView::branch:!adjoins-item{ border-color: none;}"); works
  //app.setStyleSheet("QTreeView::branch:!adjoins-item{ background: none}"); 
//  app.setStyleSheet("QTreeView::branch:!adjoins-item:!has-children{ foreground: red}"); 
 // app.setStyleSheet("QTreeView::branch:adjoins-item:has-children{ background: cyan}"); 

  QlixMainWindow qmw(&_subSystem);
  qmw.show();
  Q_INIT_RESOURCE(Qlix);
  app.exec();
  return 0;
} 
