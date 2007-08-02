//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License


#ifndef __TYPES__
#define __TYPES__

#include <string>
#include <iostream>
#include <libmtp.h>
#include <vector>
#include <map>
using namespace std;
class MtpFS;
typedef unsigned int count_t;
typedef unsigned int index_t;
typedef unsigned int size_t;
typedef char byte;
typedef unsigned char ubyte;
typedef int (MtpFS::*funcPtr) (uint64_t, uint64_t, void const* data);

typedef vector<count_t> DirectoryIDList;
enum FileSortType
{
    SortByNameUp,
    SortByNameDown,
    SortBySizeUp,
    SortBySizeDown,
    SortByTypeUp,
    SortByTypeDown
};

enum TrackSortType
{
    SortBySongUp,
    SortBySongDown,
    SortByArtistUp,
    SortByArtistDown,
    SortByTrackNumUp,
    SortByTrackNumDown
};
#endif


