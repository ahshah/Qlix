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
#include "Track.h"
#include "ShadowTrack.h"
using namespace MTP;

ShadowTrack::ShadowTrack(Track* in_track, Playlist* in_pl, count_t in_idx) :
                         GenericObject(MtpShadowTrack, 0),
                         _track(in_track),
                         _parentPlaylist(in_pl),
                         _rowIndex(in_idx)
{
  _trackAssociationIndex = _track->AssociateShadowTrack(this);
}

ShadowTrack::~ShadowTrack()
{
  _track->DisassociateShadowTrack(_trackAssociationIndex);
}

count_t ShadowTrack::RowIndex() const
{
  return _rowIndex;
}

void ShadowTrack::SetRowIndex(count_t in_idx)
{
  _rowIndex = in_idx;
}

Playlist* ShadowTrack::ParentPlaylist() const
{
  return _parentPlaylist;
}

const Track* ShadowTrack::GetTrack() const
{
  return _track;
}

