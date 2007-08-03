//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#include "MtpThread.h"
MtpThread::MtpThread (QObject* parent) : QThread (parent)
{ }

void MtpThread::IssueCommand (MtpCommand* in_command)
{
    QMutexLocker locker(&_lock) ;
    cout << "issued command" << endl;
    _jobs.push(in_command); 

    if (!isRunning())
        start();
    else
        cond.wakeOne();
}

void MtpThread::run (void)
{
    while (true)
    {
        _lock.lock();
        while (_jobs.empty() )
        {
            cond.wait(&_lock);
        }

        MtpCommand* currentJob = _jobs.front();
        _jobs.pop();
        _lock.unlock();
        MtpCommandCode type = currentJob->GetCommand();
        switch (type)
        {
            case Connect:
            {
                MtpCommandConnect* _cmd = static_cast<MtpCommandConnect*> (currentJob);
                if (!_portal.Connect())
                {
                    continue;
                }
                _device = _portal.GetDevice();
                _mtpFileSystem = _device->GetFileSystem();
                delete _cmd;
//                    MtpUpdateConnect* ret = new MtpUpdateConnect(true, ());
                cout << "Done, emitting result" << endl;
                emit ConnectDone(_device->GetFileSystem());
                continue;
            }
            case Disconnect:
            {
                MtpCommandDisconnect* _cmd = static_cast<MtpCommandDisconnect*> (currentJob);
                _portal.Disconnect();
                _device = NULL;
                delete _cmd;
                cout << "Done disconnecting" << endl;
                continue;
            }
            case SendFile:
            {
                if (!_device)
                    continue;

                MtpCommandSendFile* _cmd = static_cast<MtpCommandSendFile*>  (currentJob);
                string in_filepath = _cmd->Path;
                uint32_t parentID = _cmd->ParentID;
                delete _cmd;
                
                DirNode* parentDir = _mtpFileSystem->GetDirectory(parentID);
                assert(parentDir);

                QFileInfo fromFile(in_filepath.c_str());
                if (parentDir->FileExists(fromFile.fileName().toStdString()))
                {
                    cout << "Duplicate file found.. ignroed" << endl;
                    continue;
                }

                QFileInfo newfile(in_filepath.c_str());
                bool ret = _device->SendFileToDevice(newfile, parentID);
                cout << "transfer done, emitting result" << endl;
                emit FileTransferDone(ret);
                continue;
            }

            case GetFile:
            {
                if (!_device)
                    continue;
                MtpCommandGetFile* _cmd = static_cast<MtpCommandGetFile*> (currentJob);
                string path = _cmd->FileName;
                uint32_t id = _cmd->ID;
                bool IsRootImage = _cmd->IsRootImage;
                delete _cmd;
                
                bool ret = _device->GetFileFromDevice(id, path);
                if (IsRootImage && ret)
                    emit NewDeviceImage();
                continue;
            }

            case Delete:
            {
                MtpCommandDelete* _cmd = static_cast<MtpCommandDelete*> (currentJob);
                uint32_t FolderID = _cmd->FolderID;
                int FileID = _cmd->FileID;
                delete _cmd;
                if (FileID == -1)
                {
                    DirectoryIDList joblist;
                    DirNode* thisFolder = _mtpFileSystem->GetDirectory(FolderID);
                    thisFolder->GetSubTree(&joblist);
                    for (count_t i =0; i < joblist.size(); i++)
                    {
                        uint32_t DeleteThisFolder = joblist[i]; 
                        DirNode* SubDir = _mtpFileSystem->GetDirectory(DeleteThisFolder);
                        for (count_t j =0; j < SubDir->GetFileCount(); j++)
                        {
                            FileNode DeleteThisFile = SubDir->GetFile(j);
                            bool ret = _device->DeleteObject(DeleteThisFolder, DeleteThisFile.GetID());
                            emit FileRemoved(ret, DeleteThisFolder, DeleteThisFile.GetID());
                        }

                        bool success = _device->DeleteObject(DeleteThisFolder, FileID);
                        emit DirectoryRemoved(success, DeleteThisFolder);
                    }
                    continue;
                }

                bool success = _device->DeleteObject(FolderID, FileID);
                emit FileRemoved(success, FolderID, FileID);
                continue;
            }

            case GetDeviceInfo:
            {
                cout << "GetDeviceInfo stuf" << endl;
                break;
            }

            case GetDirMetaData:
            {
                cout << "GetDeviceInfo stuf" << endl;
                break;
            }

            case GetFileMetaData:
            {
                cout << "Get file metadata stub" << endl;
                break;
            }

            case GetSampleData:
            {
                cout << "Get sample data stub" << endl;
                break;
            }

            case SendSampleData:
            {
                cout << "Send sample data stub" << endl;
                break;
            }

            case CreateFolder:
            {
                cout << "Create folder stub" << endl;
                break;
            }
            case TransferDeviceFolder:
            {
                cout << "Transfer device folder stub" << endl;
                break;
            }

            case TransferSystemFolder:
            {
                MtpCommandTransferSystemFolder* in_cmd = 
                static_cast<MtpCommandTransferSystemFolder*> (currentJob);
                cout << "Parent id: " << in_cmd->Parent->GetID();
//                cout << "Parent name from node: " << in_cmd->Parent->GetName() << endl; 
//                cout << "Parent name from fs: " << _mtpFileSystem->GetDirectoryByName(in_cmd->Parent->GetID()) << endl; //debug
                queue<MtpCommandTransferSystemFolder*> transferQueue;
                transferQueue.push(in_cmd);
                while (transferQueue.size() > 0)
                {
                    MtpCommandTransferSystemFolder* _cmd = transferQueue.front();
                    transferQueue.pop();
                    QFileInfoList curfiles = _cmd->Files;
                    DirNode* parent = _cmd->Parent;
                    if (parent == NULL)
                    {
                        cout << "Error create folder command's parent is NULL!" << endl;
                        continue;
                    }

                    if (parent->DirectoryExists(_cmd->DirName.toStdString()))
                    {
                        cout << "Duplicate name, ignored" << endl;
                        continue;
                    }

                    /*time to create the folder; */
                    int model_index;
                    int thisParent= _device->CreateFolder(_cmd->DirName.toStdString(), parent->GetID(), &model_index);
                    if (thisParent == 0)
                        continue;
                    if (model_index == -1)
                        cout << "invalid model index!" << endl;
                    else
                        emit DirectoryAdded(parent, model_index);

                    for (int j =0; j < curfiles.size(); j++)
                    {
                        QFileInfo thisFile = curfiles[j];
//                        cout << "Processing: " << thisFile.fileName().toStdString() <<endl;
                        if (!thisFile.exists())
                            continue;
                        if(thisFile.fileName() == "..")
                            continue;
                        if(thisFile.fileName() == ".")
                            continue;
                        if (thisFile.isDir()) //place it on the queue
                        {
                            cout <<"Found subdirectory: " << thisFile.fileName().toStdString() << endl;
                            QDir tempFSDir(thisFile.filePath());
                            MtpCommandTransferSystemFolder* temp =
                                new MtpCommandTransferSystemFolder(tempFSDir, parent);
                            transferQueue.push(temp);
                            continue;
                        }
 //                       cout << "Sending file: " <<thisFile.fileName().toStdString() << endl;
                        bool ret = _device->SendFileToDevice(thisFile, thisParent);
                        emit FileTransferDone(ret);

                    }
                    delete _cmd;
                }
                continue;
            }
            cout << "not of correct format" << endl;
        }
    
        //do job make sure to delete internal pointers here
        //now we are done, start over..
        delete currentJob;
    }
}

