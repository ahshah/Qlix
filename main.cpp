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

//TODO Find out how to get TagLIB to print its version

#include <libmtp.h>
#include <QMainWindow>
#include <QApplication>
#include <QPlastiqueStyle>
#include <QString>
#include <QtDebug>

#include "mtp/MtpSubSystem.h"
#include "widgets/QlixMainWindow.h"
#ifdef LINUX_SIGNALS
#include "linuxsignals.h"
#endif

void printLibraryVersions();
CommandLineOptions ParseCommandLineOptions(unsigned int argc, char* argv[]);

MtpSubSystem _subSystem;
int main(int argc, char* argv[])
{
  installSignalHandlers();
  printLibraryVersions();

  /** Setup D-BUS information */
  QCoreApplication::setOrganizationName("QlixIsOSS");
  QCoreApplication::setOrganizationDomain("Qlix.berlios.de");
  QCoreApplication::setApplicationName("Qlix");
  /** Setup QSettings */
  QSettings settings;
  QString ret = settings.value("DefaultDevice").toString();
  
  /** Setup QApplication */
  QApplication app(argc, argv);
  app.setStyle("Plastique");

  /** Grab AutoFix options */
  CommandLineOptions opts = ParseCommandLineOptions(argc, argv);

  _subSystem.SetAutoFixOptions(opts); 
  if (opts.AutoFixPlaylists)
    cout << "Working" << endl;
  //app.setStyleSheet("QTreeView::branch:!adjoins-item{ border-color: none;}"); works
  //app.setStyleSheet("QTreeView::branch:!adjoins-item{ background: none}"); 
  //app.setStyleSheet("QTreeView::branch:!adjoins-item:!has-children{ foreground: red}"); 
  //app.setStyleSheet("QTreeView::branch:adjoins-item:has-children{ background: cyan}"); 

  QlixMainWindow qmw(&_subSystem);
  qmw.show();
  Q_INIT_RESOURCE(Qlix);
  app.exec();
  return 0;
} 

/**
 * This function will print the version of all dependent  libraries
 **/
void printLibraryVersions()
{ 
  cout << "LIBMTP VERSION: " + string(LIBMTP_VERSION_STRING) << endl;
}

CommandLineOptions ParseCommandLineOptions(unsigned int argc, char* argv[])
{
  bool AutoFixPlaylists = false;
  bool AutoFixAlbums= false;
  bool SimulateTransfers= false;
  bool DebugOutput = false;
  for (unsigned int i =0; i < argc; i ++)
  {
    QString str(argv[i]);
    str = str.toLower();
    cout << "param: " << str.toStdString() << endl;
    if (str == "--fixbadplaylists")
    {
      AutoFixPlaylists = true;
      cout << "FOUND PL FIX!!!" << endl;
    }
    else if (str == "--autofixalbums")
    {
      AutoFixAlbums = true;
      cout << "FOUND ALBUM FIX!!!" << endl;
    }
    else if (str == "--simulate")
    {
      SimulateTransfers = true;
      cout << "FOUND SIMULATE TRANSFERS FIX!!!" << endl;
    }
    else if (str == "--debug")
    {
      DebugOutput = true;
    }
  }

  return CommandLineOptions(AutoFixPlaylists, AutoFixAlbums, SimulateTransfers, DebugOutput);
}
