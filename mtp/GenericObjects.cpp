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
using namespace MTP;
/**
 * Creates a new GenericObject, a generic base class for MTP objects
 * @param in_type the MTP type of the new GenericObject to be created
 * @return a new GenericObject
 */
GenericObject::GenericObject(MtpObjectType in_type, uint32_t id) :
                             _type(in_type),
                             _id(id)
{ }

GenericObject::~GenericObject() {}

/**
 * @return the ID of this object
 */
count_t GenericObject::ID() const { return _id; }

/**
 * @return the storage ID this object resides on
 */
uint32_t GenericObject::StorageID() const
{
  return 0;
}
/**
 * Sets the ID of this object
 */
void GenericObject::SetID( count_t in_id) { _id = in_id; }

/**
 * Returns the depth of the current object. Must be overriden by child classes
 * to provide meaningful behaviour
 */
//TODO why doesn't every object just have a depth field
//Potential answer: because we don't really need to know every object's depth.. yet
count_t GenericObject::Depth() const { return 0; }

/**
 * Simple function to get the type of the current MTP object
 * @return returns the type of the GenericObject
 */
MtpObjectType GenericObject::Type() const { return _type; }


/**
 * @return the name of this object
 */
const char* GenericObject::Name() const { return ""; }

/**
 * Creates a new GenericFileObject, a generic base class for File MTP objects
 * That are crosslinked with each other
 * @param in_type the MTP type of the new GenericObject to be created
 * @return a new GenericObject
 */
GenericFileObject::GenericFileObject(MtpObjectType in_type, uint32_t in_id) :
                                     GenericObject(in_type, in_id),
                                     _association(NULL)
{}

/**
 * Sets the objects file association, a file cannot be associated with another
 * file.
 * This is used to associate container structures to their underlying file
 * counter parts since LIBMTP makes a distinction between a container's
 * specific structure (eg. folder, playlist, album) and its underlying
 * file/object representation.
 * Thus for each playlist there is a file associated with it as well.
 * The same goes for albums, but not for folders (they are a special type of
 * container)
 * Also- when deleting an album, one should take care to delete its associated
 * file/object reference since it is not guaranteed that it will be deleted.
 * @param in_obj the object to associate this object with
 */
void GenericFileObject::Associate(GenericFileObject* in_obj)
{
  if (Type() == MtpFile)
   assert(in_obj->Type() != MtpFile);

  //assert that we don't call this function twice on the same object
  assert(!_association);
   _association = in_obj;
}

/**
 * Retreives the file associated with this album
 */
GenericFileObject* GenericFileObject::Association() const
{
  return _association;
}
