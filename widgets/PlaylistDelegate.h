#ifndef PLAYLIST_DELEGATE
#define PLAYLIST_DELEGATE
#include <QItemDelegate>
#include <QFontMetrics>
#include <MtpObject.h>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QSortFilterProxyModel>
//#include <ImageButton.h>

class  PlaylistDelegate : public QItemDelegate
{
Q_OBJECT

public:
  PlaylistDelegate(QSortFilterProxyModel*, QWidget* parent = NULL);
  void paint (QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;
  QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const;
private:
  QSortFilterProxyModel* _proxy;
};

Q_DECLARE_METATYPE(MTP::Track*);
Q_DECLARE_METATYPE(MTP::GenericObject*);
Q_DECLARE_METATYPE(MTP::Album*);

#endif
