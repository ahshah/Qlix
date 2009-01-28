#ifndef MTP_FOLDER
#define MTP_FOLDER
#include "GenericObjects.h"

/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This file is part of the Qlix project on http://berlios.de
 *
 *   This file may be used under the terms of the GNU General Public
 *   License version 2.0 as published by the Free Software Foundation
 *   and appearing in the file COPYING included in the packaging of
 *   this file.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License version 2.0 for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
namespace MTP
{
  class File;
/**
 * @class Folder is a class that wraps around LIBMTP_folder_t
*/
class Folder : public GenericObject
{
public:
  Folder(LIBMTP_folder_t*, Folder*, count_t);
  count_t FileCount() const;
  count_t FolderCount() const;
  virtual count_t Depth() const;
  Folder* ParentFolder() const;

  virtual uint32_t StorageID() const;
  virtual char const * Name() const;
  Folder* ChildFolder(count_t ) const;
  File* ChildFile(count_t ) const;

  LIBMTP_folder_t* RawFolder() const;
  void AddChildFolder(Folder*);
  void AddChildFile(File*);
  void RemoveChildFolder(Folder*);
  void RemoveChildFile(File*);

  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

private:
  LIBMTP_folder_t* _rawFolder;
  count_t _depth;
  Folder* _parent;
  std::vector<Folder*> _childFolders;
  std::vector<File*> _childFiles;
  count_t _rowIndex;
};

}
#endif
