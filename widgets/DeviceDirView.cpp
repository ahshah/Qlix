//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License
#include "DeviceDirView.h"
DeviceDirView::DeviceDirView (QWidget* parent = NULL, MtpThread* in_thread = NULL) : QTreeView(parent)
{
    _thread = in_thread;
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setupMenus();
    setSortingEnabled(true);
    setViewportMargins(0,0,0,0);
    sortByColumn(1, Qt::AscendingOrder);
}

void DeviceDirView::setupMenus (void)
{
    _transferFolderAct = new QAction("Transfer Folder", this);
    connect (_transferFolderAct, SIGNAL(triggered()), this, SLOT(transferFolder()));
    insertAction(NULL, _transferFolderAct);

    _createFolderAct = new QAction("Create folder", this);
    connect (_createFolderAct, SIGNAL(triggered()), this, SLOT(createFolder()));
    insertAction(NULL, _createFolderAct);

    _deleteFolderAct = new QAction("Delete folder", this);
    connect (_deleteFolderAct, SIGNAL(triggered()), this, SLOT(deleteFolder()));
    insertAction(NULL, _deleteFolderAct);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void DeviceDirView::reset (void )
{
    QItemSelectionModel* _dirSelection = selectionModel();
    DeviceDirModel* current_model= static_cast <DeviceDirModel*> (model());
    QModelIndex temp = current_model->index(0, 0, QModelIndex()); 
    _dirSelection->setCurrentIndex(temp, QItemSelectionModel::ClearAndSelect |QItemSelectionModel::Rows );
    QTreeView::reset();
}

//private slots:
void DeviceDirView::transferFolder ( void )
{ }
void DeviceDirView::createFolder ( void )
{ }
void DeviceDirView::deleteFolder ( void ) 
{
    QModelIndexList selected = selectedDirectories();
    DeviceDirModel* cur_model = static_cast<DeviceDirModel*> (model());
    for (int i =0; i < selected.size(); i++)
    {
        QModelIndex thisSelection = selected[i];
        DirNode* selected_node = reinterpret_cast <DirNode*> (thisSelection.internalPointer() );
        MtpCommandDelete* _cmd  = new MtpCommandDelete(selected_node->GetID(), -1);
        _thread->IssueCommand(_cmd);
    }
}
//private: 

QModelIndexList DeviceDirView::selectedDirectories()
{
    QItemSelectionModel* selection_model = selectionModel();
    QModelIndexList selected_list = selection_model->selectedRows();
    return selected_list;
}
