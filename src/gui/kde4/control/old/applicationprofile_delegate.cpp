#include "remotes_delegate.h"
#include "remote/RemotesList.h"
#include "remote/Remote.h"

#include <KDE/KLocale>
#include <QDebug>
#include <ksqueezedtextlabel.h>

#include <exception>

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

#define ptitleLabel 0
#define pmakedefaultButton 1
#define icon_margin 5
#define icon_size 64
#define icon_emblem_size 16

RemotesDelegate::RemotesDelegate(QAbstractItemView *itemView, QObject * parent)
: KWidgetItemDelegate(itemView, parent) {
	remoteIcon = KIcon("network-wireless");
	aboutIcon = KIcon("help-about");
	emblemDesktop = KIcon("preferences-desktop");
	emblemCursor = KIcon("preferences-desktop-mouse");
	emblemMultimedia = KIcon("applications-multimedia");
	mButtonWidth = 0;
}
RemotesDelegate::~RemotesDelegate() {
  
}

// paint the item at index with all it's attributes shown
void RemotesDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
  
	if (!index.isValid())
		return;
    
	//begin painting
	painter->save();
	
	//Background
	QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);
	
	//icon
	remoteIcon.paint(painter,
	  option.rect.left() + icon_margin,
	  option.rect.top() + icon_margin, icon_size, icon_size);
	
	//capabilities through emblems
	QPoint pt(option.rect.width()-icon_margin-icon_emblem_size,
		    option.rect.bottom()-icon_margin-icon_emblem_size);

	int subsets = index.data(RemoteKeySubsetRole).toInt();

	painter->setOpacity(0.5);

	if (subsets & liri::Remote::Multimedia) {
		emblemMultimedia.paint(painter, pt.x(), pt.y(), icon_emblem_size, icon_emblem_size);
		pt.setX(pt.x() - icon_margin - icon_emblem_size);
	}
	if (subsets & liri::Remote::DesktopControl) {
		emblemDesktop.paint(painter, pt.x(), pt.y(), icon_emblem_size, icon_emblem_size);
		pt.setX(pt.x() - icon_margin - icon_emblem_size);
	}
	if (subsets & liri::Remote::Cursor) {
		emblemCursor.paint(painter, pt.x(), pt.y(), icon_emblem_size, icon_emblem_size);
		pt.setX(pt.x() - icon_margin - icon_emblem_size);
	}

	painter->setOpacity(1);

	//change color if selected
	if (option.state & QStyle::State_Selected)
		painter->setPen(QPen( option.palette.color(QPalette::HighlightedText) ));

	//paint title line
	/*
	QString string("<b>"+index.data(Qt::DisplayRole).toString()+"</b> ("+
              ((index.data(RemoteFileLocationRole).toBool()) ? i18n("system") : i18n("user"))+")");
	painter->drawText(option.rect.x()+icon_margin*2+icon_size, option.rect.y()+icon_margin, string);
	*/

	//paint description text
	QFontMetrics fm = QFontMetrics(QFont());

	QRectF textrect(option.rect);
	textrect.setLeft(textrect.left() + icon_margin*2+icon_size);
	textrect.setTop(textrect.top() + icon_margin*2+fm.height());
	textrect.setRight(textrect.right() - mButtonWidth-icon_margin*2);
	textrect.setBottom(textrect.bottom() - icon_margin);

	painter->drawText(textrect, index.data(RemoteDescriptionRole).toString());

	painter->restore();
	KWidgetItemDelegate::paintWidgets(painter, option, index);
    }

// get the list of widgets
QList<QWidget*> RemotesDelegate::createItemWidgets() const {
        QList<QWidget*> list;

        QLabel * titleLabel = new QLabel();
        list << titleLabel;

        QPushButton * makedefaultButton = new QPushButton();
        list << makedefaultButton;
        setBlockedEventTypes(makedefaultButton, QList<QEvent::Type>() << QEvent::MouseButtonPress
                    << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick);
        connect(makedefaultButton, SIGNAL(clicked()), this, SLOT(slotMakeDefaultClicked()));
	
        return list;
}

void RemotesDelegate::slotMakeDefaultClicked() {
        QModelIndex index = focusedIndex();
        const QSortFilterProxyModel * proxymodel =
            qobject_cast<const QSortFilterProxyModel*>(index.model());
        if (proxymodel == NULL) return;

        RemotesList * model =
            static_cast<RemotesList*>(proxymodel->sourceModel());
        if (model == NULL || !index.isValid())  return;

        liri::Remote* newre = (liri::Remote*) index.data(RemoteRole).value<void*>();
        //let the model swap the default receiver
        model->changeRemoteDefault(newre); 
}

// update the widgets
void RemotesDelegate::updateItemWidgets(const QList<QWidget*> widgets,
                                    const QStyleOptionViewItem &option,
                                    const QPersistentModelIndex &index) const {

        QPoint point(icon_margin*2+icon_size, icon_margin);
        
        //title label
        QLabel * titleLabel = qobject_cast<QLabel*>(widgets.at(ptitleLabel));
        if (titleLabel != NULL)
        {
            QString string("<b>"+index.data(Qt::DisplayRole).toString()+"</b> ("+
              ((index.data(RemoteFileLocationRole).toBool()) ? i18n("system") : i18n("user"))+")");
            titleLabel->setText(string);
            QPalette p = option.palette;
            if (option.state & QStyle::State_Selected)
              p.setColor(QPalette::WindowText, p.color(QPalette::HighlightedText) );
            titleLabel->setPalette(p);
            titleLabel->resize(titleLabel->sizeHint());
            titleLabel->move(point.x(), point.y());
        }

        QPushButton * makedefaultButton = qobject_cast<QPushButton*>(widgets.at(pmakedefaultButton));
	
        //makedefault Button
        if (makedefaultButton != NULL) {
          makedefaultButton->setText(i18n("Make Default"));
          makedefaultButton->resize(makedefaultButton->sizeHint());
          makedefaultButton->move(option.rect.width()-makedefaultButton->width()-icon_margin, point.y());
          makedefaultButton->setEnabled( ! index.data(RemoteDefaultRemoteRole).toBool() );
	  mButtonWidth = makedefaultButton->width();
        }

}

QSize RemotesDelegate::sizeHint( const QStyleOptionViewItem &, const QModelIndex &) const {
        return QSize(350, 2 * icon_margin + icon_size);
}
