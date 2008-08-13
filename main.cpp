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

#include <libmtp.h>
#include <QMainWindow>
#include <QApplication>
#include <QPlastiqueStyle>
#include <QtDebug>

#include "mtp/MtpSubSystem.h"
#include "widgets/QlixMainWindow.h"
#ifdef LINUX_SIGNALS
#include "linuxsignals.h"
#endif

MtpSubSystem _subSystem;
int main(int argc, char* argv[])
{
  installSignalHandlers();

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
