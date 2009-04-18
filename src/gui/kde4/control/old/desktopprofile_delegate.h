#ifndef LIRI_DESKTOPPROFILE_DELEGATE_H_
#define LIRI_DESKTOPPROFILE_DELEGATE_H_

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

class DesktopprofileDelegate: public KWidgetItemDelegate
{
Q_OBJECT
public:
	DesktopprofileDelegate(QAbstractItemView *itemView, QObject * parent = 0);
	~DesktopprofileDelegate();

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

private:

};

#endif
