
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

#include "GenericObjects.h"
#include "File.h"
#include "Track.h"
#include "Folder.h"
#include "Album.h"

using namespace MTP;
/**
 * Creates a new File object and and stores its representative data
 * @param in_file A pointer to the LIBMTP_file_to wrap over
 * @param in_depth The depth of this file in the file system
 * @param in_sample A pointer to the LIBMTP_filesampledata_t
 * @return a new File object
 */
File::File(LIBMTP_file_t* in_file, count_t in_depth) :
           GenericFileObject(MtpFile, in_file->item_id),
           _depth(in_depth)
{
  _rawFile = in_file;
}

/**
 * Creates a new File object from a track and sets up the association.
 * @param in_track A pointer to the associated track
 * @param in_parent A pointer to the parent folder for this file
 * @param in_storage_id The storage ID that this file resides on
 * @return a new File object
 */
File::File(Track* in_track, Folder* in_parent) :
          GenericFileObject(MtpFile, in_track->ID()),
          _depth(in_parent->Depth())
{
  assert(in_parent->ID() == in_track->ParentFolderID());
  assert(in_parent->StorageID() == in_track->StorageID());
  _rawFile = new LIBMTP_file_t();
  _rawFile->parent_id = in_parent->ID();
  _rawFile->storage_id = in_track->StorageID();
  _rawFile->filename = strdup(in_track->FileName());
  _rawFile->filesize = in_track->FileSize();
  _rawFile->filetype = in_track->FileType();
}

/**
 * Creates a new File object from a track and sets up the association.
 * @param in_track A pointer to the associated track
 * @param in_parent A pointer to the parent folder for this file
 * @param in_storage_id The storage ID that this file resides on
 * @return a new File object
 */
File::File(Album* in_album, Folder* in_parent) :
          GenericFileObject(MtpFile, in_album->ID()),
          _depth(in_parent->Depth())
{
  assert(in_parent->ID() == in_album->ParentFolderID());
  assert(in_parent->StorageID() == in_album->StorageID());
  _rawFile = new LIBMTP_file_t();
  _rawFile->parent_id = in_parent->ID();
  _rawFile->filename = strdup(in_album->Name());
  //guess
  _rawFile->filesize = 54;
  _rawFile->filetype = LIBMTP_FILETYPE_UNKNOWN;
}

/**
 * Retreives the file's parent ID
 * @return the file's parent ID it is zero if its in the root folder
 */
count_t File::ParentID() const { return _rawFile->parent_id; }

/**
 * Retrieves the file's parent Folder
 * @return the file's parent Folder or NULL if it exists in the root dir
 */
Folder* File::ParentFolder() const { return _parent; }

/**
 * @return the storage ID that this file resides on
 */
uint32_t File::StorageID() const { return _rawFile->storage_id; }

/**
 * Sets the file's parent Folder
 */
void File::SetParentFolder(Folder const * in_parent)
{
  _parent = const_cast <Folder*>  (in_parent);
}

/*
 * Returns the row index of this file
 */
count_t File::GetRowIndex() const
{
  return _rowIndex;
}

/*
 * Returns the row index of this file
 */
void File::SetRowIndex(count_t in_idx)
{
  _rowIndex = in_idx;
}

/**
 * Returns the name of the file
 * @return The name of the file as a Utf8 string
 */
char const * File::Name() const
{
  //TODO error checking here?
  //     no, this should be the caller's duty
  return _rawFile->filename;
}

/** Returns the raw file that this object wraps around
 * @return the raw file;
 */
LIBMTP_file_t* File::RawFile() const
{
  return _rawFile;
}

