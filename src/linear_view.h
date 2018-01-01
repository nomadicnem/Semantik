// Thomas Nagy 2007-2018 GPLV3

#ifndef LINEAR_VIEW_H
#define LINEAR_VIEW_H

#include <QTreeWidget>
#include <QMap>

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QMouseEvent;
class QMimeData;
class sem_mediator;
class linear_view : public QTreeWidget
{
	Q_OBJECT
	public:
		linear_view(QWidget *i_oParent, sem_mediator *i_oControl);
		sem_mediator *m_oMediator;

		QMap<int, QTreeWidgetItem*> m_oItems;

		bool filter_item(QTreeWidgetItem * i_oItem, const QString & i_oS);

		void dropEvent(QDropEvent *);

		int m_iLockSelect;

	public slots:
		void filter_slot(const QString & i_oS);
		void selection_changed();

		void notify_add_item(int id);
		void notify_repaint(int id);
		void notify_delete_item(int id);
		void notify_link_items(int id1, int id2);
		void notify_unlink_items(int id1, int id2);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_sort(int, bool);
		void notify_edit(int id);
		void notify_datatype(int);
};

#endif

