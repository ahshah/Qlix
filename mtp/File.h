#ifndef MTP_FILE
#define MTP_FILE
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

/**
 * @class File is a class that wraps around LIBMTP_file_t
*/


namespace MTP
{
class Folder;
class Track;
class Album;

class File : public GenericFileObject
{
public:
  File(LIBMTP_file_t*, count_t);
  File(Track*, Folder* );
  File(Album*, Folder* );

  count_t ParentID() const;
  virtual const char * Name() const;

  void SetParentFolder(Folder const* );
  Folder* ParentFolder() const;

  virtual uint32_t StorageID() const;
  count_t GetRowIndex() const;
  void SetRowIndex(count_t);

  LIBMTP_file_t* RawFile() const;
private:
  LIBMTP_file_t* _rawFile;
  LIBMTP_filesampledata_t _sampleData;
  Folder* _parent;
  count_t _rowIndex;
  count_t _depth;
};

}
#endif
