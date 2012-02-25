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

//#include <libmtp.h>
#include <QApplication>
#include <QPlastiqueStyle>
#include "widgets/QlixMainWindow.h"

#ifdef LINUX_SIGNALS
#include "linuxsignals.h"
#endif

MtpSubSystem _subSystem;

/**
 * This function prints the version of all relevant libraries
 **/
void printLibraryVersions()
{ 
    fprintf(stderr, "LibMTP version: " LIBMTP_VERSION_STRING "\n");
    fprintf(stderr, "TagLIB version: %d.%d.%d\n", 
            TAGLIB_MAJOR_VERSION, TAGLIB_MINOR_VERSION, TAGLIB_PATCH_VERSION);
}

/* Parse out command line options:
 * --fix-playlists:
 * --fix-albums:
 * --simulate:
 * --debug: 
 */
CommandLineOptions ParseCommandLineOptions(unsigned int argc, char* argv[])
{
    bool AutoFixPlaylists  = false;
    bool AutoFixAlbums     = false;
    bool SimulateTransfers = false;
    bool DebugOutput       = false;
    for (unsigned int i =0; i < argc; i ++)
    {
        QString str(argv[i]);
        str = str.toLower();
        if (str == "--fix-playlists")
        {
            AutoFixPlaylists = true;
            cerr << "Automatically fixing bad playlists" << endl;
        }
        else if (str == "--fix-albums")
        {
            AutoFixAlbums = true;
            cerr << "Automatically fixing bad albums" << endl;
        }
        else if (str == "--simulate")
        {
            SimulateTransfers = true;
            cerr << "Simulating device transfers" << endl;
        }
        else if (str == "--debug")
        {
            DebugOutput = true;
            cerr << "Debug output enabled" << endl;
        }
    }

    return CommandLineOptions(AutoFixPlaylists, AutoFixAlbums, SimulateTransfers, DebugOutput);
}

int main(int argc, char* argv[])
{
    installSignalHandlers();
    printLibraryVersions();

    /** Setup Application information, used for settings */
    QCoreApplication::setOrganizationName("OSS");
    QCoreApplication::setOrganizationDomain("github.com/ahshah/Qlix");
    QCoreApplication::setApplicationName("Qlix");

    /** Setup QSettings */
    QSettings settings;
    QString ret = settings.value("DefaultDevice").toString();

    /** Setup QApplication */
    QApplication app(argc, argv);

    /* Plastique doesn't look so great on OSX */
#ifdef LINUX_UI 
    app.setStyle("Plastique"); 
#endif

    /** Grab AutoFix options */
    CommandLineOptions opts = ParseCommandLineOptions(argc, argv);
    _subSystem.SetAutoFixOptions(opts); 

    /** Let the games begin! **/
    Q_INIT_RESOURCE(Qlix);
    QlixMainWindow qmw(&_subSystem);
    qmw.show();
    app.exec();
    return 0;
}
