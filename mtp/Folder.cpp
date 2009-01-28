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

#include "Folder.h"
#include "File.h"
using namespace MTP;

/** Creates a new Folder object
 * @param in_folder A pointer to the LIBMTP_folder_t wrap over
 * @param in_depth The depth of this folder in the filesystem
 * @param in_parent A pointer to this folder's parent
 * @return a new Folder object
 */
Folder::Folder(LIBMTP_folder_t* in_folder, Folder* in_parent,
               count_t in_depth) :
               GenericObject (MtpFolder, in_folder->folder_id),
               _depth(in_depth),
               _parent(in_parent)
{
  assert(in_folder);
//  cout << "Creating new folder " << in_folder->name << " with id:" << ID() << endl;
  _rawFolder = in_folder;
}
/**
 * Return's this folder's parent
 * @return the folder's parent
 */
Folder* Folder::ParentFolder() const
{
  return _parent;
}

/**
 * Get the raw LIBMTP folder
 * @return the raw LIBMTP folder for this Folder object
 */
LIBMTP_folder_t* Folder::RawFolder() const
{
  return _rawFolder;
}

/**
 * Returns the subfolder at the given index if it exists.
 * @return the subfolder at the given index or NULL if it doesn't exist
 */
Folder* Folder::ChildFolder(count_t idx) const
{
  if (idx >= _childFolders.size())
  {
    cerr << "Requesting invalid child index for folder" << endl;
    return NULL;
  }
  else
    return _childFolders[idx];
}

/**
 * Returns the subfolder at the given index if it exists.
 * @return the subfolder at the given index or NULL if it doesn't exist
 */
File* Folder::ChildFile(count_t idx) const
{
  if (idx >= _childFiles.size())
    return NULL;
  else
    return _childFiles[idx];
}


/**
 * Returns the number of files under this folder
 * @return the number of files under this folder
 */
count_t Folder::FileCount() const
{
  return _childFiles.size();
}

/**
 * @return the number of folders under this folder
 */
count_t Folder::FolderCount() const
{
  return _childFolders.size();
}

/** Retreives the name of the wrapped folder or "" if it doesn't exist
 * @return the folder's UTF8 name or a blank string if it doesn't exist
 */
const char* Folder::Name() const
{
  if (!_rawFolder)
    return "";
  else
    return _rawFolder->name;
}

/** Adds the passed folder as a subdirectory to this folder
 * @param in_folder the folder to add as a subfolder of this folder
 */
void Folder::AddChildFolder(Folder* in_folder)
{
  _childFolders.push_back(in_folder);
}

/** Adds the passed file as a leaf file of this
 * @param in_file the file to add as a leaf of this folder
 */
void Folder::AddChildFile(File* in_file)
{
  _childFiles.push_back(in_file);
}
void Folder::RemoveChildFolder(Folder* in_folder)
{
  vector<MTP::Folder*>::iterator iter = _childFolders.begin();
  count_t i = 0;
  for (; iter != _childFolders.end() && i < in_folder->GetRowIndex(); i++)
  {
    iter++;
  }
  if (i != in_folder->GetRowIndex())
  {
    cerr << "Deletion error: in_folder row index:" << in_folder->GetRowIndex()
         << " child folder count: " << _childFolders.size() << endl;
    assert(false);
  }

  _childFolders.erase(iter);
  return;
}
/**
 * @return The depth of this folder in the folder tree
 * */
count_t Folder::Depth() const { return _depth; }

/**
 * @return the storage ID that this folder resides on
 */
uint32_t Folder::StorageID() const
{
  return _rawFolder->storage_id;
}

/**
 * Remove a direct child file from the folder's list of child files.
 * @param in_file The file to remove.
 * */
void Folder::RemoveChildFile(File* in_file)
{
  vector<MTP::File*>::iterator iter = _childFiles.begin();
  count_t i = 0;
  for (; iter != _childFiles.end() && i < in_file->GetRowIndex(); i++)
  {
    iter++;
  }
  if (i != in_file->GetRowIndex())
  {
    cerr << "Deletion error: in_file row index:" << in_file->GetRowIndex()
         << " child file count: " << _childFiles.size() << endl;
    assert(false);
  }

  _childFiles.erase(iter);
  return;
}

/**
 * @return the visual row index for this folder
 * */
count_t Folder::GetRowIndex() const { return _rowIndex; }

/**
 * Sets the visual row index for this folder
 * @param in_row the new row of this folder
 * */
void Folder::SetRowIndex(count_t in_row) { _rowIndex = in_row; }

