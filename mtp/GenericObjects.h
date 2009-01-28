#ifndef GENERIC_OBJECTS
#define GENERIC_OBJECTS
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
 *   TODO error checking when returning char* ?
 *   TODO use multimap for track/file distinction
 *        RESOLVED by Association() parameter (I think)
 *   TODO Figure out if we need to actually implement removeFromRawPlaylist/album/folder etc
 *      RESOLVED: Yes we do- we removeFromRawPlaylist() then update the device by reuploading
 *      the raw container
 */



#include <libmtp.h>
#include <vector>
#include <assert.h>
#include "types.h"
#include <iostream>
#include <string>
#include <string.h>

namespace MTP
{
/**
 * @class Generic base class for other MTP object types
*/
class GenericObject
{
public:
  GenericObject(MtpObjectType, uint32_t);
  virtual ~GenericObject();
  count_t ID() const;
  void SetID(count_t);
  virtual count_t Depth() const;
  virtual uint32_t StorageID() const;
  MtpObjectType Type() const;
  virtual const char* Name() const;

private:
  MtpObjectType _type;
  count_t _id;
  count_t _depth;
};

class GenericFileObject : public GenericObject
{
  public:
  GenericFileObject(MtpObjectType, uint32_t);
  void Associate(GenericFileObject* );
  GenericFileObject* Association() const;

  private:
  GenericFileObject* _association;
};

}
#endif
