//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License


#ifndef __MTPDIR__
#define __MTPDIR__
#include <QString>
#include <libmtp.h>
using namespace std;

class MtpDir
{
public:
    MtpDir(LIBMTP_folder_t* in_folder, count_t in_height)
            :_rawFolder(in_folder)
    {
        if (!in_folder)
        {
            cout << "BIG ERROR" << endl;
            exit(-1);
        }

        SortedOrder = in_height;
        FolderId = in_folder->folder_id;
        ParentId = in_folder->parent_id;
        Name = QString::fromUtf8(in_folder->name);
    }
    MtpDir(LIBMTP_folder_t* in_folder) :
        _rawFolder(in_folder)
    {
        cout << "Creating super root!" << endl;
        Name = "SuperRoot";
        SortedOrder = 0;
        FolderId = 0;
        return;
   }

    const LIBMTP_folder_t* _rawFolder;
    uint32_t FolderId;
    uint32_t ParentId;
    count_t SortedOrder;
    QString Name;
};

#endif
