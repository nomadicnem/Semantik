// Thomas Nagy 2007-2018 GPLV3

#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <QTableWidget>

#in\
clude "con.h"

#include <QStyledItemDelegate>
#include <QAction>
#include <QList>

class QEvent;
class sem_mediator;

class numbered_action : public QAction
{
	public:
		numbered_action(QString, QWidget*);
		int row;
		int col;
};

class table_view : public QTableWidget
{
	Q_OBJECT
	public:
		table_view(QWidget *i_oParent, sem_mediator *i_oControl);
		sem_mediator *m_oMediator;

		void mousePressEvent(QMouseEvent *i_oEv);

		int m_iId;

		numbered_action *m_oRmRowAct;
		numbered_action *m_oRmColAct;
		numbered_action *m_oAddRowAct;
		numbered_action *m_oAddColAct;

		bool m_bFreeze;

	public slots:
		void rm_row();
		void rm_column();
		void add_row();
		void add_column();
		void resize_table();
		void cell_changed(int i_iRow, int i_iCol);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_table(int id);
};

class table_view_filter : public QStyledItemDelegate
{
	Q_OBJECT
	public:
		table_view* m_oParent;
		table_view_filter(QWidget*);
		QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;
		void setEditorData(QWidget*, const QModelIndex &) const;
		void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;
};

#endif

