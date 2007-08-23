//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#include "MtpThread.h"
MtpThread::MtpThread (QObject* parent) : QThread (parent)
{ }

void MtpThread::IssueCommand (MtpCommand* in_command)
{
    QMutexLocker locker(&_lock) ;
    qDebug() << "issued command" ;
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
                qDebug() << "Done, emitting result" ;
                emit ConnectDone(_device->GetFileSystem());
                continue;
            }
            case Disconnect:
            {
                MtpCommandDisconnect* _cmd = static_cast<MtpCommandDisconnect*> (currentJob);
                _portal.Disconnect();
                _device = NULL;
                delete _cmd;
                qDebug() << "Done disconnecting" ;
                continue;
            }
            case SendFile:
            {
                if (!_device)
                    continue;

                MtpCommandSendFile* _cmd = static_cast<MtpCommandSendFile*>  (currentJob);
                QString in_filepath = _cmd->Path;
                uint32_t parentID = _cmd->ParentID;
                bool IsTrack = _cmd->IsTrack;
                delete _cmd;

                //Get prarent, make sure it exists 
                DirNode* parentDir = _mtpFileSystem->GetDirectory(parentID);
                assert(parentDir);
                QFileInfo fromFile(in_filepath);


                // Does this file/track already exist on the device?
                if (parentDir->FileExists(fromFile.fileName()) )
                {
                    qDebug() << "Duplicate file found.. ignroed" ;
                    continue;
                }

                QFileInfo newfile(in_filepath);
                // If you are trying to send a track, send it through the device sendTrack interface
                bool ret = false;
                if (IsTrack)
                    ret = _device->SendTrackToDevice(newfile, parentID);
                else
                    ret = _device->SendFileToDevice(newfile, parentID);

                qDebug() << "transfer done, emitting result" ;
                emit FileTransferDone(ret);
                continue;
            }

            case GetFile:
            {
                if (!_device)
                    continue;
                MtpCommandGetFile* _cmd = static_cast<MtpCommandGetFile*> (currentJob);
                QString path = _cmd->FileName;
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
                        while(SubDir->GetFileCount() > 0)
                        {
                            FileNode DeleteThisFile = SubDir->GetFile(0);
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
                qDebug() << "GetDeviceInfo stuf" ;
                break;
            }

            case GetDirMetaData:
            {
                qDebug() << "GetDeviceInfo stuf" ;
                break;
            }

            case GetFileMetaData:
            {
                qDebug() << "Get file metadata stub" ;
                break;
            }

            case GetSampleData:
            {
                qDebug() << "Get sample data stub" ;
                break;
            }

            case SendSampleData:
            {
                qDebug() << "Send sample data stub" ;
                break;
            }

            case CreateDirectory:
            {
                MtpCommandNewDirectory* in_cmd =
                    static_cast<MtpCommandNewDirectory*> (currentJob);

                QString name = in_cmd->Name;
                uint32_t parentID = in_cmd->ParentID;
                delete in_cmd;
                int model_index;
                int newParentID = _device->CreateFolder(name, parentID, &model_index);
                if (model_index == -1)
                    qDebug() << "invalid model index!" ;
                if (newParentID== 0)
                    continue;
                DirNode* tempParent = _device->GetDirectory(parentID);
                emit DirectoryAdded(tempParent, model_index);
                break;
            }

            case TransferDeviceFolder:
            {
                qDebug() << "Transfer device folder stub" ;
                break;
            }

            case TransferSystemFolder:
            {
                MtpCommandTransferSystemFolder* in_cmd = 
                static_cast<MtpCommandTransferSystemFolder*> (currentJob);
                qDebug() << "Parent id: " << in_cmd->Parent->GetID();
//                qDebug() << "Parent name from node: " << in_cmd->Parent->GetName() ; 
//                qDebug() << "Parent name from fs: " << _mtpFileSystem->GetDirectoryByName(in_cmd->Parent->GetID()) ; //debug
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
                        qDebug() << "Error create folder command's parent is NULL!" ;
                        delete _cmd;
                        continue;
                    }

                    if (parent->DirectoryExists(_cmd->DirName) )
                    {
                        qDebug() << "Duplicate name, ignored" ;
                        delete _cmd;
                        continue;
                    }

                    /*time to create the folder; */
                    int model_index;
                    int newParentID= _device->CreateFolder(_cmd->DirName, parent->GetID(), &model_index);
                    if (newParentID == 0)
                        continue;
                    if (model_index == -1)
                        qDebug() << "invalid model index!" ;
                    else
                        emit DirectoryAdded(parent, model_index);

                    for (int j =0; j < curfiles.size(); j++)
                    {
                        QFileInfo thisFile = curfiles[j];
//                        qDebug() << "Processing: " << thisFile.fileName().toStdString();
                        if (!thisFile.exists())
                            continue;
                        if(thisFile.fileName() == "..")
                            continue;
                        if(thisFile.fileName() == ".")
                            continue;
                        if (thisFile.isDir()) //place it on the queue
                        {
                            qDebug() <<"Found subdirectory: " << thisFile.fileName() ;
                            DirNode* new_Parent_Dir= _device->GetDirectory(newParentID);
                            assert (new_Parent_Dir);
                            QDir tempFSDir(thisFile.filePath());
                            MtpCommandTransferSystemFolder* temp =
                                new MtpCommandTransferSystemFolder(tempFSDir, new_Parent_Dir);
                            transferQueue.push(temp);
                            continue;
                        }
 //                       qDebug() << "Sending file: " <<thisFile.fileName().toStdString();
                        bool ret = _device->SendTrackToDevice(thisFile, newParentID);
                        emit FileTransferDone(ret);

                    }
                    delete _cmd;
                }
                continue;
            }
            qDebug() << "not of correct format" ;
        }
    
        //do job make sure to delete internal pointers here
        //now we are done, start over..
        delete currentJob;
    }
}

