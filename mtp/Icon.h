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

#ifndef __BMPICON__
#define __BMPICON__
#include <string>
#include <iostream>
#include <vector>
#include "BmpStructs.h"
#include "types.h"

using namespace std;
/**
 * @class this class parses the ICO format and pulls out the DIB files
 * from a specified buffer. 
 * This class is largely uncommented as it should be rewritten as a QImageDecoder
 */
class DeviceIcon
{
public:
    DeviceIcon (const void* in_b)
    {
        byte* in_buffer = (byte*) in_b;
        _file = in_buffer;
        _bestImage = 0;
        _bestImageSize = 0;
        _header = NULL;
        GetHeader();
        PopulateImages();
        //ReadImages(); //debug
        FindBestImage();
        LowLevelExtract();
    }

    ~DeviceIcon()
    {
        if (_header)
        {
            delete _header;
            _header = NULL;
        }
    }

    IconHeader* GetHeader ( void )
    {
        _header = new IconHeader();
        memcpy(_header, _file, sizeof(IconHeader));
        return _header;
    }

    void PopulateImages ( void )
    {
        count_t offset = sizeof(IconHeader);
        for (count_t i = 0; i < _header->Count; i++)
        {
            IconDirEntry* temp = new IconDirEntry();
            memcpy(temp, _file + offset, sizeof(IconDirEntry));
            _iconList.push_back(temp);
            offset += sizeof(IconDirEntry);
        }
     //   cout << "Headers end at: " << offset << endl;
    }
    
    void ReadImages()
    {
        for (count_t i = 0; i < _iconList.size(); i++)
        {
            IconDirEntry* temp = _iconList[i]; 
            cout <<"Icon: " << i << endl; cout <<"Dimensions: "<< (int)temp->Width << " x " << (int)temp->Height << endl; cout <<"Depth: " << (int)temp->ColorCount << endl; cout <<"Reserved: " << (int)temp->Reserved << endl; cout <<"Bit count: " << (int)temp->BitCount<< endl; cout <<"Byte count: " << (int)temp->DataSize<< endl; cout <<"Offset: " << (int) temp->DataOffset << endl; cout << endl;
        }
    }
    void Extract(void* temp)
    {
        byte* out = (byte*) temp;
        IconDirEntry* icon = _iconList[_bestImage];
        memcpy (out, _bestImagePtr, icon->DataSize-sizeof(DibHeader)); 
    }

    count_t GetBestImageSize()
    {
        return _bestImageSize;
    }
    Dimensions GetDimensions()
    {
        return _bestImageDimensions;
    }

    bool IsValid()
    {
        if (_bestImageDepth == 32 && _isValid)
            return true;
        return false;
    }

private:
    byte* _file;
    byte* _bestImagePtr;
    index_t _bestImage;
    count_t _bestImageSize;
    count_t _bestImageDepth;
    Dimensions _bestImageDimensions;
    bool _isValid;
    
    IconHeader* _header;
    vector <IconDirEntry*> _iconList;

    bool IsSquare(IconDirEntry* in_icon)
    {
      if (!in_icon || in_icon->Height != in_icon->Width)
        return false;
      return true;
    }
    void FindBestImage() 
    {
        size_t HighestResolution = 0;
        size_t HighestColorDepth = 0;
        index_t CurrentWinner    = 0;
        if (_iconList.size() == 0)
        {
            _isValid = false;
            return; 
        }
        for (count_t i = 0; i < _iconList.size(); i++)
        {
            IconDirEntry* temp = _iconList[i];
            if (temp->BitCount > HighestColorDepth && IsSquare(temp))
            {
//                cout << "Replacing entry: " << CurrentWinner << " with " <<  i << endl;
                CurrentWinner = i;
                HighestResolution = temp->Width;
                HighestColorDepth = temp->BitCount;
            }
            if (temp->Width > HighestResolution && IsSquare(temp))
            {
                if (temp->BitCount>= HighestColorDepth)
                {
                //    cout << "Replacing entry: " << CurrentWinner << " with " <<  i << endl;
                    CurrentWinner = i;
                    HighestResolution = temp->Width;
                    HighestColorDepth = temp->BitCount;
                }
            }
        }
 //       cout << "Picked index: " << CurrentWinner << endl;
        _bestImage = CurrentWinner;
        _bestImageSize = (_iconList[_bestImage]->DataSize) - sizeof(DibHeader); // Get rid of the dib header its useless
        _bestImageDimensions.Width = (_iconList[_bestImage])->Width;
        _bestImageDimensions.Height = (_iconList[_bestImage])->Height;
        _bestImageDepth = (_iconList[_bestImage])->BitCount;
        _isValid = true;
        return;
    }
    void LowLevelExtract()
    {
#ifdef QLIX_DEBUG
      cout << "Extracting with width: " << _bestImageDimensions.Width << " height: " << _bestImageDimensions.Height << endl; 
#endif

        _bestImagePtr = new byte[_bestImageSize];
        byte tempPtr[_bestImageSize];
        IconDirEntry* icon = _iconList[_bestImage];
        memcpy (&tempPtr, _file + (icon->DataOffset + sizeof(DibHeader)), icon->DataSize-sizeof(DibHeader)); 
        //now flip and store into _bestImagePtr;
        cout << "_bestImageDepth = " << _bestImageDepth << endl; 
        int pxlsize = _bestImageDepth / 8;
        int row = _bestImageDimensions.Width;
        int j = row-1;
        for (count_t i =0; i < _bestImageDimensions.Height; i++)
        {
            byte* tempRow = (byte*) (&tempPtr) + (i*row*pxlsize);
            byte* lastRow = _bestImagePtr +  (j*row* pxlsize);
            memcpy(lastRow, tempRow, row*pxlsize);
            j--;
        }
    }

};

#endif
