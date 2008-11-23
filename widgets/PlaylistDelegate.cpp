#include "PlaylistDelegate.h"

PlaylistDelegate::PlaylistDelegate(QSortFilterProxyModel* in_proxy, QWidget* parent ) :
                             QItemDelegate(parent),
                             _proxy(in_proxy)

{ 
  qDebug() << "Stub";
}

void PlaylistDelegate::paint (QPainter* painter, const QStyleOptionViewItem& opt,
                           const QModelIndex& in_idx) const
{
  qDebug() << "Stub";
}

QSize PlaylistDelegate::sizeHint(const QStyleOptionViewItem& opt, 
                              const QModelIndex& in_idx) const
{

  QModelIndex idx = _proxy->mapToSource(in_idx);
  MTP::GenericObject* obj=  (MTP::GenericObject*)idx.internalPointer();

  if ( obj->Type() == MtpAlbum)
    return QSize(36, 36);
  return QSize(20,20);
}

