//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __MTPPARENT__
#define __MTPPARENT__
#include <iostream>
#include <string>
using namespace std;
class MtpParent
{
public:
    void GetErrors (LIBMTP_mtpdevice_t* in_device)
    {
        if (!in_device)
            return;
        LIBMTP_error_t* errors = LIBMTP_Get_Errorstack(in_device);
        while (errors)
        {
            string errstring = errors->error_text;
            cout <<"Qlix bridge error: " << errstring << endl;
            if (isTerminal(errors->errornumber) )
                exit(1);
            errors = errors->next;
        }
        LIBMTP_Clear_Errorstack(in_device);
    }
protected:
    bool isTerminal (LIBMTP_error_number_t in_err)
    {
        switch (in_err)
        {
            case LIBMTP_ERROR_USB_LAYER:
                return true;

            case LIBMTP_ERROR_MEMORY_ALLOCATION:
                return true;

            case LIBMTP_ERROR_NO_DEVICE_ATTACHED:
                return true;

            case LIBMTP_ERROR_CONNECTING:
                return true;
            default:
                return false;
        }
    }
};
#endif
