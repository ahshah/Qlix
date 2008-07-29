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
 */

#ifndef MTPSTORAGE
#define MTPSTORAGE
#include <libmtp.h>
#include <string.h>

/**
 * @class is an adapter class over LIBMTP_storage_t
 */
class MtpStorage
{
public:
  MtpStorage(LIBMTP_devicestorage_t* );
  ~MtpStorage();

  uint64_t TotalSpace() const;
  uint64_t FreeSpace() const;
  uint64_t FreeObjectSpace() const;
  unsigned int ID() const;

  const char* const  Description() const;
  const char* const VolumeID() const;

private:
  unsigned int _id;
  unsigned int _storageType;
  unsigned int _filesystemType;
  unsigned int _accessCapability;
  uint64_t _totalSpace;
  uint64_t _freeSpace;
  uint64_t _freeObjectSpace;
  char* _description;
  char* _volumeID;
};
#endif
