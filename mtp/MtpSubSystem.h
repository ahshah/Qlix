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

  count_t DeviceCount() const;

  count_t RawDeviceCount (MtpDeviceVector* connected, 
                                      MtpDeviceVector* disconnected,
                                      MtpDeviceVector* newDevice);
  MtpDevice* Device(count_t);

private:
  vector <MtpDevice*> _devList;
  LIBMTP_mtpdevice_t* _deviceList;
  MTPErrorVector _errorList;

};
#endif
