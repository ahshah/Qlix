#include "MtpPortal.h"
MtpPortal::MtpPortal ()
{
    _deviceCount = 0;
    LIBMTP_Init();
}

MtpPortal::~MtpPortal()
{
    for (count_t i = 0; i < _deviceList.size(); i++)
    {
        if (_deviceList[i])
        {
            delete _deviceList[i];
            _deviceList[i]= NULL;
        }
    }
}

bool MtpPortal::Connect()
{
   _connected = true;
   LIBMTP_mtpdevice_t* firstDev = LIBMTP_Get_First_Device();
   if (!firstDev)
       return false;
   MtpDevice* _device = new MtpDevice (firstDev, _deviceCount);
   _deviceCount++;
   _deviceList.push_back(_device);
   return true;
}

void MtpPortal::Disconnect()
{
    vector<MtpDevice*>::iterator iter;
    for (iter = _deviceList.begin(); iter != _deviceList.end(); iter++)
    {
        if ((*iter)->GetID() == _deviceCount-1)
        {
            delete _deviceList[0];
            _deviceList.erase(iter);
            if (iter == _deviceList.end() )
                break;
        }
    }
    _connected = false;
}
MtpDevice* MtpPortal::GetDevice ( void )
{
    if (_deviceList.size() > 0) // forthe time being
        return _deviceList[0];
    return NULL;
}
bool MtpPortal::IsConnected()
{
    return _connected;
}


