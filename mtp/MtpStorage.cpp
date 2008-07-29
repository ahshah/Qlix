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

#include "MtpStorage.h"
MtpStorage::MtpStorage(LIBMTP_devicestorage_t* in_storage) :
                       _totalSpace(0),
                       _freeSpace(0)
{
  _description = new char[strlen(in_storage->StorageDescription)];
  _volumeID = new char[strlen(in_storage->VolumeIdentifier)];
  strcpy(_description, in_storage->StorageDescription);
  strcpy(_volumeID, in_storage->VolumeIdentifier);
  _totalSpace = in_storage->MaxCapacity;
  _freeSpace = in_storage->FreeSpaceInBytes;
  _id = in_storage->id;
}

MtpStorage::~MtpStorage()
{
  delete _description;
  _description = NULL;
  delete _volumeID;
  _volumeID= NULL;
}



uint64_t MtpStorage::TotalSpace() const { return _totalSpace; }
uint64_t MtpStorage::FreeSpace() const { return _freeSpace; }
uint64_t MtpStorage::FreeObjectSpace() const { return _freeObjectSpace; }
unsigned int MtpStorage::ID() const {  return _id; }

const char* const MtpStorage::Description() const { return _description; }
const char* const MtpStorage::VolumeID() const  { return _volumeID;  }

