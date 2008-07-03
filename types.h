#ifndef __TYPES__
#define __TYPES__
#include <vector>
using namespace std;
typedef vector<LIBMTP_error_t> MTPErrorVector;
typedef unsigned int count_t;
typedef unsigned int index_t;
typedef unsigned char ubyte;


enum MtpObjectType
{
  MtpTrack,
  MtpFile,
  MtpFolder,
  MtpAlbum,
  MtpPlaylist
};

enum QlixProgressType
{
  FreeSpaceUsage,
  TransferAmount
};
#endif
