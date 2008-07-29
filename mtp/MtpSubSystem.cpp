/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
TODO better error handling
*/

#include "MtpSubSystem.h"
/**
 * Does nothing for now, besides initializing LIBmtp
*/
MtpSubSystem::MtpSubSystem()
{
  LIBMTP_Init();
}


MtpSubSystem::~MtpSubSystem()
{
  ReleaseDevices();
}

/**
  * Initializes the MTP subsytem and retrive device list
  */
void MtpSubSystem::Initialize()
{
  LIBMTP_error_number_t tempNum;
#ifndef MULTIPLE_DEVICES
  tempNum = LIBMTP_Get_Connected_Devices(&_deviceList);
  LIBMTP_mtpdevice_t* _dlist = _deviceList;
  while(_dlist)
  {
    MtpDevice* dev = new MtpDevice(_dlist);
    _devList.push_back(dev);
    _dlist = _dlist->next;
  }
#else
  LIBMTP_mtpdevice_t* first = LIBMTP_Get_First_Device();
  if (first == NULL)
    return;
  for (int i =0; i < 7; i++)
  {
    MtpDevice* dev = new MtpDevice(first);
    _devList.push_back(dev);
  }
#endif
  //cout << " sub system Detected " << _devList.size() << " devices" << endl;
}

/**
 * Releases all MTP devices for a gracefull shutdown
 */
void MtpSubSystem::ReleaseDevices()
{
#ifndef MULTIPLE_DEVICES
  for (count_t i = 0; i < _devList.size(); i++)
  {
    if (_devList[i])
    {
      delete _devList[i];
      _devList[i] = NULL;
    }
  }
  _devList.clear();
  LIBMTP_mtpdevice_t* rawDeviceList = _deviceList;
  while (rawDeviceList)
  {
    LIBMTP_Release_Device(rawDeviceList);
    rawDeviceList = rawDeviceList->next;
  }
  _deviceList = NULL;
#else
  for (count_t i=0; i < _devList.size(); i++)
  {
    //Release the first device as all the rest are clones of it..
    if (i == 0)
      _devList[0]->ReleaseDevice();
    delete _devList[i];
    _devList[i] = NULL;
  }
  _devList.clear();
#endif
}

/**
 * @return the number of discovered devices
 */
count_t MtpSubSystem::DeviceCount() const
{
  return _devList.size();
}

/**
 * Returns the requested device by index ID
 * @param idx the index of the device to reteive
 * @return Returns the requested device
 */
MtpDevice* MtpSubSystem::Device(count_t idx)
{
  if (idx >= _devList.size())
    return NULL;
  else
    return _devList[idx];
}
