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
 *   This program is distributed in the hope that it will be useful, *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License version 2.0 for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __MTPSUBSYSTEM__
#define __MTPSUBSYSTEM__

#include <libmtp.h>
#include <vector>
#include "types.h"
#include "MtpDevice.h"
#include <iostream>
using namespace std;

typedef vector <MtpDevice*> MtpDeviceVector;
/** 
 * @class MtpSubSystem is a wrapper class around libmtp
*/
class MtpSubSystem
{
public:
  MtpSubSystem();
  ~MtpSubSystem();
  void ReleaseDevices();
  void Initialize();
  void SetAutoFixOptions(CommandLineOptions in_autofixopts);

  count_t DeviceCount() const;

  count_t RawDeviceCount (MtpDeviceVector* connected, 
                                      MtpDeviceVector* disconnected,
                                      MtpDeviceVector* newDevice);
  MtpDevice* Device(count_t);

private:
  vector <MtpDevice*> _devList;
  LIBMTP_mtpdevice_t* _deviceList;
  MTPErrorVector _errorList;
  CommandLineOptions _commandLineOpts; 
};
#endif
