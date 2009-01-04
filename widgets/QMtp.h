/*
 * QMtp.h
 */

#ifndef QMTP_H_
#define QMTP_H_
#include <QModelIndex>
#include "types.h"
#include "mtp/MtpObject.h"
namespace QMTP
{

/**
 * @param in_idx the index of the inquired object
 * @return the type of object pointed to by the index. This function returns
 * MtpUnknown if the index is invalid
 */
static MtpObjectType MtpType(const QModelIndex& in_idx)
{
  if (!in_idx.isValid() )
    return MtpInvalid;

  MTP::GenericObject* obj = (MTP::GenericObject*)in_idx.internalPointer();
  return obj->Type();
}

/**
 * @param in_idx the index of the inquired object
 * @return the raw MTP::GenericObject* of the object if the index is valid.
 * This function returns NULL If the index is not valid or if the index's
 * internal pointer is not an MtpFolder, MtpFile, MtpTrack, MtpAlbum, or
 * MtpPlayist
 */
static MTP::GenericObject* RawGenericObject(QModelIndex in_idx)
{
  if (!in_idx.isValid())
     return NULL;
  MtpObjectType theType = MtpType(in_idx);
  if (theType != MtpFolder ||
      theType != MtpFile ||
      theType != MtpTrack ||
      theType != MtpPlaylist ||
      theType != MtpAlbum ||
      theType != MtpTrack)
    {
      assert(false);
    }
  else
    return (MTP::GenericObject*) in_idx.internalPointer();
}

/**
 * @param in_idx the index of the inquired object
 * @return the raw MTP::Folder* of the object if the index is valid.
 * This function returns NULL If the index is not valid or if the index's
 * internal pointer is not an MtpFolder.
 */
static MTP::Folder* RawFolder(QModelIndex in_idx)
{
  if (!in_idx.isValid())
    return NULL;
  else if (MtpType(in_idx) != MtpFolder)
    return NULL;
  else
    return (MTP::Folder*) in_idx.internalPointer();
}

/**
 * @param in_idx the index of the inquired object
 * @return the raw MTP::File* of the object if the index is valid. If it is not
 *  valid return NULL. If the index is not an MtpFile return NULL.
 */
static MTP::File* RawFile(QModelIndex in_idx)
{
  if (!in_idx.isValid())
    return NULL;
  else if (MtpType(in_idx) != MtpFile)
    return NULL;
  else
    return (MTP::File*) in_idx.internalPointer();
}

/**
 * @param left the left index
 * @param right the right index
 * @return true of the left index is deeper in the folder tree than the right index
 */
static bool MtpFolderLessThan(const MTP::Folder* left, const MTP::Folder* right)
{
  assert( left->Type() == MtpFolder);
  assert( right->Type() == MtpFolder);
  return (left->Depth() > right->Depth());
}

}


#endif /* QMTP_H_ */
