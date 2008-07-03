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
