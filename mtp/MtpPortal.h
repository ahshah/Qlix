//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __MTPPORTAL__
#define __MTPPORTAL__
#include "types.h"
#include "MtpDevice.h"
#include <libmtp.h>
#include <iostream>
#include <vector>

using namespace std;

class MtpPortal 
{
public:
    MtpPortal ();
    ~MtpPortal();

    bool Connect();
    void Disconnect();
    MtpDevice* GetDevice ( void );
    bool IsConnected();

private:
    bool _connected;
    vector<MtpDevice*> _deviceList;
    count_t _deviceCount;
};
#endif
