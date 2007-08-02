//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __DIRFILTER__
#define __DIRFILTER__
#include <QSortFilterProxyModel>
#include <QDirModel>

class DirFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    DirFilter(QObject* parent = NULL) : QSortFilterProxyModel (parent)
    {
        _dirFilter = false;
    }

    void setDirectoryFilter ( void )
    {
        _dirFilter = true;
    }
    void setFileFilter ( void )
    {
        _dirFilter = false;
    }
protected:
    bool filterAcceptsRow (int sourceRow, const QModelIndex& sourceParent) const
    {
        if (!_dirFilter)
            return fileFilter(sourceRow, sourceParent);
        QModelIndex par = sourceParent.parent();
        if (!par.isValid() && sourceRow == 0)
            return true;

        QModelIndex tempIndex = sourceParent.child(sourceRow, 0);
        if (!tempIndex.isValid())
            return false;
        QDirModel* src = (QDirModel*) sourceModel();
        if (src->isDir(tempIndex))
            return true;
        else
            return false;
    }


private:
    bool _dirFilter;
    bool fileFilter ( int sourceRow, const QModelIndex& sourceParent) const
    {
        if (_dirFilter)
            return false;

        QModelIndex par = sourceParent.parent();
        if (!par.isValid() && sourceRow == 0)
            return true;

        QModelIndex tempIndex = sourceParent.child(sourceRow, 0);
        if (!tempIndex.isValid())
            return false;

        QDirModel* src = (QDirModel*) sourceModel();
        if (! src->isDir(tempIndex))
            return true;
        else
            return false;
    }
        

};
#endif
