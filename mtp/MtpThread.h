//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __MTPTHREAD__
#define __MTPTHREAD__
#include "MtpFS.h"
#include "types.h"
#include "CommandCodes.h"
#include <QMutexLocker>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QDir>
#include <queue>

using namespace std;
typedef vector<MtpCommand*> MtpCommandList;
typedef vector<MtpUpdate*> MtpUpdateList;

class MtpThread : public QThread 
{
    Q_OBJECT
public:
    MtpThread (QObject*);
        
    void IssueCommand (MtpCommand* in_command);
    void run();

signals:
    void ConnectDone(MtpFS* FS);
    void TransferToDeviceDone(bool);
    void FileTransferDone(bool);
    void NewDeviceImage();
    void DirectoryAdded(DirNode* parent, index_t newfolderlocation);
    void FileRemoved(bool, index_t, index_t);
    void DirectoryRemoved(bool, index_t);

private:
    MtpPortal _portal;
    MtpDevice* _device;
    MtpFS* _mtpFileSystem;
    queue <MtpCommand*> _jobs;
    QMutex _lock;
    QWaitCondition cond;
};

#endif
