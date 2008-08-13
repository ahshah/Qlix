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

#ifndef __STRUCTS__
#define __STRUCTS__

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned int count_t;
typedef unsigned short ushort;

/*
 * @struct these structs are largely used by the DeviceIcon class
 * see also Icon.
 * Largely uncommented as it should be written as QImageDecoder
 */
struct Dimensions
{
    count_t Width;
    count_t Height;
};
struct IconHeader
{
    short Reserved;
    short Type;
    ushort Count;
}__attribute__ ((packed));

struct IconDirEntry
{
    ubyte Width;
    ubyte Height;
    ubyte ColorCount;
    byte Reserved;
    short Planes;
    ushort BitCount;
    count_t DataSize;
    int DataOffset;
}__attribute__ ((packed));

struct BmpFileHeader
{
    unsigned short type; //2
    unsigned int size;   //4
    ushort reserved1; //2
    ushort reserved2; //2
    unsigned int offset;      //4
} __attribute__ ((packed));

struct DibHeader
{
    int HeaderSize; //size of header
    int Width;
    int Height;
    short Planes;
    ushort BitCount;
    int Compression;
    int RawImageSize;
    int HRes;
    int VRes;
    int Colors;
    int ImportantColors;
}__attribute__ ((packed));

struct PaletteEntry
{
    byte Blue;
    byte Green;
    byte Red;
    byte Reserved;
}__attribute__ ((packed));

struct Pixel
{
    byte Blue;
    byte Green;
    byte Red;
    byte Alpha;
}__attribute__ ((packed));

struct Image
{
    Pixel image[128*128];
}__attribute__ ((packed));

struct MonochromeImage
{
    byte image[128][128];
}__attribute__ ((packed));


#endif


