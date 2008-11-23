#ifndef ALBUM_DELEGATE
#define ALBUM_DELEGATE
#include <QItemDelegate>
#include <QFontMetrics>
#include <MtpObject.h>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QSortFilterProxyModel>
//#include <ImageButton.h>

class  AlbumDelegate : public QItemDelegate
{
Q_OBJECT

public:
  AlbumDelegate(QSortFilterProxyModel*, QWidget* parent = NULL);
  void paint (QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;
  QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const;
private:
  QSortFilterProxyModel* _proxy;
//  ImageButton* _albumCoverButton;
};

Q_DECLARE_METATYPE(MTP::Track*);
Q_DECLARE_METATYPE(MTP::GenericObject*);
Q_DECLARE_METATYPE(MTP::Album*);

#endif
