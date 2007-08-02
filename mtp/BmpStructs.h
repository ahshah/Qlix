//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __STRUCTS__
#define __STRUCTS__

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned int count_t;
typedef unsigned short ushort;
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


