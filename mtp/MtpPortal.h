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
    MtpPortal ()
    {
        _deviceCount = 0;
        _device = NULL;
        LIBMTP_Init();
    }

    ~MtpPortal()
    {
        if (_device)
        {
            delete _device;
            _device = NULL;
        }
    }

    void Connect()
    {
        _connected = true;
       _device = new MtpDevice (LIBMTP_Get_First_Device());
       _deviceCount++;
    }
    
    void Disconnect()
    {
        if (!_device)
        {
            _connected = false;
            return;
        }

        delete _device;
        _device = NULL;
        _connected = false;

    }
    MtpDevice* GetDevice ( void )
    {
        return _device;
    }
    bool IsConnected()
    {
        return _connected;
    }

private:
    bool _connected;
    MtpDevice* _device;
    count_t _deviceCount;
};
#endif
