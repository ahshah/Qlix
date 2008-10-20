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

struct AutoFixOpts
{
  AutoFixOpts(bool in_AutoFixPlaylists) :
                                        AutoFixPlaylists(in_AutoFixPlaylists)
  { }

  AutoFixOpts(const AutoFixOpts& in_opts) :
                                        AutoFixPlaylists(in_opts.AutoFixPlaylists)
  { }

  AutoFixOpts& operator = (const AutoFixOpts& in_opts)
  {
    (const_cast<bool&> (this->AutoFixPlaylists)) = in_opts.AutoFixPlaylists;
    return *this;
  }

  const bool AutoFixPlaylists;
};

#endif
