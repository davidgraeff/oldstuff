#include "desktopprofile_delegate.h"

#include <KDE/KLocale>
#include <QDebug>

#include <QPushButton>
#include <QRect>
#include <QFontMetrics>
#include <QColor>
#include <QPen>
#include <QPixmap>
#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QStyle>

#define icon_margin 5
#define icon_size 64

DesktopprofileDelegate::DesktopprofileDelegate(QAbstractItemView *itemView, QObject * parent)
: KWidgetItemDelegate(itemView, parent) {

}
DesktopprofileDelegate::~DesktopprofileDelegate() {
  
}

// paint the item at index with all it's attributes shown
void DesktopprofileDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
  
	if (!index.isValid())
		return;
    
	//begin painting
	painter->save();
	
	//Background
	QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);
	
	//icon
	index.data(Qt::DecorationRole).value<QIcon>().paint(painter,
	  option.rect.left() + icon_margin,
	  option.rect.top() + icon_margin, icon_size, icon_size);
	
	//change color if selected
	if (option.state & QStyle::State_Selected)
		painter->setPen(QPen( option.palette.color(QPalette::HighlightedText) ));

	QFontMetrics fm = QFontMetrics(QFont());

	//paint title line
	painter->drawText(
		option.rect.left()+icon_margin*2+icon_size,
		(option.rect.bottom()-option.rect.top()-fm.height())/2,
		index.data(Qt::DisplayRole).toString());

	//paint description text

	QRectF textrect(option.rect);
	textrect.setLeft(textrect.left() + icon_margin*2+icon_size);
	textrect.setTop(textrect.top() + icon_margin*2+fm.height());
	textrect.setRight(textrect.right() - icon_margin*2);
	textrect.setBottom(textrect.bottom() - icon_margin);

	painter->drawText(textrect, index.data(Qt::UserRole).toString());

	painter->restore();
	KWidgetItemDelegate::paintWidgets(painter, option, index);
    }

// get the list of widgets
QList<QWidget*> DesktopprofileDelegate::createItemWidgets() const {
        QList<QWidget*> list;
        return list;
}


// update the widgets
void DesktopprofileDelegate::updateItemWidgets(const QList<QWidget*> widgets,
                                    const QStyleOptionViewItem &option,
                                    const QPersistentModelIndex &index) const {
Q_UNUSED(widgets);
Q_UNUSED(option);
Q_UNUSED(index);
}

QSize DesktopprofileDelegate::sizeHint( const QStyleOptionViewItem &, const QModelIndex &) const {
        return QSize(350, 2 * icon_margin + icon_size);
}
