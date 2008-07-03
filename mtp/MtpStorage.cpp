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

