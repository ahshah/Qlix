#include "AlbumDelegate.h"

AlbumDelegate::AlbumDelegate(QSortFilterProxyModel* in_proxy, QWidget* parent ) :
    QItemDelegate(parent),
    _proxy(in_proxy)

{ }

void AlbumDelegate::paint (QPainter* painter, const QStyleOptionViewItem& opt,
        const QModelIndex& in_idx) const
{
    QModelIndex idx         = _proxy->mapToSource(in_idx);
    MTP::GenericObject* obj = (MTP::GenericObject*)idx.internalPointer();

    if (!in_idx.isValid())
        return QItemDelegate::paint(painter, opt, idx);
    else if (obj->Type() != MtpAlbum)
        return QItemDelegate::paint(painter, opt, idx);

    if (opt.state & QStyle::State_Selected)
        painter->fillRect(opt.rect, opt.palette.highlight());

    MTP::Album* alb = (MTP::Album*) obj;
    QSize szHint = sizeHint(opt, in_idx);

    QString albumTitle = idx.data().toString();
    albumTitle = albumTitle;
    QString artistTitle = alb->ArtistName();

    QPen theAlbumPen("black");
    QPen theArtistPen(QColor(88,88,88));

    QFont albumFont;
    QFont artistFont;
    albumFont.setBold(true);
    //albumFont.setPointSize(8.5);
    //artistFont.setPointSize(7);
    artistFont.setLetterSpacing(QFont::AbsoluteSpacing, 1);

    painter->setFont(albumFont);
    painter->save();

    QRect rect = opt.rect;
    QRect coverRect(rect.x(), rect.y(), 24, 24);
    coverRect= coverRect.adjusted(0, (szHint.height()-24) /2, 0, (szHint.height()-24)/2 );
    qDebug() << coverRect;
    QPixmap cover = (idx.data(Qt::DecorationRole)).value<QPixmap>();
    painter->drawPixmap(coverRect, cover);

    rect = rect.adjusted(32, (szHint.height()-28), 0, 0);
    painter->setPen(theAlbumPen);
    painter->drawText(rect, Qt::AlignLeft, albumTitle);
    rect = rect.adjusted(0, 13, 0, 0);

    painter->setFont(artistFont);
    painter->setPen(theArtistPen);
    painter->drawText(rect, Qt::AlignLeft, artistTitle);
    painter->restore();
}

QSize AlbumDelegate::sizeHint(const QStyleOptionViewItem& opt,
        const QModelIndex& in_idx) const
{
    if (!in_idx.isValid())
        return QSize(20,20);

    QModelIndex idx         = _proxy->mapToSource(in_idx);
    MTP::GenericObject* obj =  (MTP::GenericObject*)idx.internalPointer();

    if (!obj || obj->Type() != MtpAlbum)
        return QSize(20,20);

    return QSize(36, 36);
}
