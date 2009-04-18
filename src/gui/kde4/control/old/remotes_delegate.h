#ifndef LIRI_REMOTE_DELEGATE_H_
#define LIRI_REMOTE_DELEGATE_H_

#include <kwidgetitemdelegate.h>
#include <kicon.h>

#include <QtCore/QMap>
#include <QtCore/QModelIndex>
#include <QtCore/QObject>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QVariant>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QStyleOptionViewItem>


namespace liri { class Remote; }

class RemotesDelegate: public KWidgetItemDelegate
{
Q_OBJECT
public:
	RemotesDelegate(QAbstractItemView *itemView, QObject * parent = 0);
	~RemotesDelegate();

	// paint the item at index with all it's attributes shown
	void paint(QPainter * painter, const QStyleOptionViewItem & option,
		const QModelIndex & index) const;
	
	// get the list of widgets
	QList<QWidget*> createItemWidgets() const;

	// update the widgets
	void updateItemWidgets(const QList<QWidget*> widgets,
		const QStyleOptionViewItem &option,
		const QPersistentModelIndex &index) const;

	QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index) const;

private slots:
	void slotMakeDefaultClicked();

private:
	KIcon remoteIcon;
	KIcon aboutIcon;
	KIcon emblemDesktop;
	KIcon emblemCursor;
	KIcon emblemMultimedia;
	mutable int mButtonHeight;
	mutable int mButtonWidth;
	//for animating emblems
	QPainter * t_painter;
	QStyleOptionViewItem* t_option;
	QModelIndex* t_index;
};

#endif
