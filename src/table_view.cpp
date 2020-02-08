// Thomas Nagy 2007-2020 GPLV3

#include <QtDebug>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QSpinBox>
#include<QCoreApplication>
#include<QEvent>
#include <QPlainTextEdit>

#include "con.h"
#include "table_dialog.h"
#include "data_item.h"
#include "sem_mediator.h"
#include "table_view.h"
#include "mem_command.h"

numbered_action::numbered_action(QString x, QWidget *y) : QAction(x, y)
{

}

table_view::table_view(QWidget *i_oParent, sem_mediator *i_oControl) : QTableWidget(0, 0, i_oParent)
{
	m_oMediator = i_oControl;
	m_bFreeze = false;

	m_oAddRowAct = new numbered_action(i18n("Add Row"), this);
	m_oAddRowAct->setStatusTip(i18n("Add a row"));
        connect(m_oAddRowAct, SIGNAL(triggered()), this, SLOT(add_row()));
	insertAction(0, m_oAddRowAct);

	m_oAddColAct = new numbered_action(i18n("Add Column"), this);
	m_oAddColAct->setStatusTip(i18n("Add a column"));
        connect(m_oAddColAct, SIGNAL(triggered()), this, SLOT(add_column()));
	insertAction(0, m_oAddColAct);


	m_oRmRowAct = new numbered_action(i18n("Remove Row"), this);
	m_oRmRowAct->setStatusTip(i18n("Remove a row"));
        connect(m_oRmRowAct, SIGNAL(triggered()), this, SLOT(rm_row()));
	insertAction(0, m_oRmRowAct);

	m_oRmColAct = new numbered_action(i18n("Remove Column"), this);
	m_oRmColAct->setStatusTip(i18n("Remove a column"));
        connect(m_oRmColAct, SIGNAL(triggered()), this, SLOT(rm_column()));
	insertAction(0, m_oRmColAct);

	QAction *m_oClearAct = new QAction(i18n("Redimension"), this);
	m_oClearAct->setStatusTip(i18n("Change table dimensions"));
        connect(m_oClearAct, SIGNAL(triggered()), this, SLOT(resize_table()));
	insertAction(0, m_oClearAct);


        QPalette l_oPalette = palette();
        l_oPalette.setBrush(QPalette::Inactive, QPalette::Base, QColor(230, 230, 230));
        l_oPalette.setBrush(QPalette::Disabled, QPalette::Base, QColor(230, 230, 230));
	setPalette(l_oPalette);

	table_view_filter* l_oFilter = new table_view_filter(this);
	setItemDelegate(l_oFilter);
	connect(this, SIGNAL(cellChanged(int, int)), this, SLOT(cell_changed(int, int)));
}

void table_view::mousePressEvent(QMouseEvent *i_oEv)
{
	if (i_oEv->button() == Qt::RightButton)
	{
		bool l_b = currentRow()>=0 && currentColumn()>=0;
		m_oRmColAct->setEnabled(l_b);
		m_oRmRowAct->setEnabled(l_b);

		QTableWidgetItem *t = itemAt(i_oEv->pos());
		int i = 0;
		int j = 0;
		if (t != NULL)
		{
			i = t->row();
			j = t->column();
		}
		m_oRmColAct->row = m_oRmRowAct->row = m_oAddRowAct->row = m_oAddColAct->row = i;
		m_oRmColAct->col = m_oRmRowAct->col = m_oAddRowAct->col = m_oAddColAct->col = j;

		m_oAddRowAct->setText(QString("%1 (%2)").arg(i18n("Add Row"), QString::number(i)));
		m_oAddColAct->setText(QString("%1 (%2)").arg(i18n("Add Column"), QString::number(j)));
		m_oRmRowAct->setText(QString("%1 (%2)").arg(i18n("Remove Row"), QString::number(i)));
		m_oRmColAct->setText(QString("%1 (%2)").arg(i18n("Remove Column"), QString::number(j)));

		QMenu::exec(actions(), i_oEv->globalPos());
	}
	else
	{
		QTableWidget::mousePressEvent(i_oEv);
		resizeRowsToContents();
	}
}

void table_view::cell_changed(int i_iRow, int i_iCol)
{
	if (m_bFreeze) return;
	Q_ASSERT(m_iId != NO_ITEM);

	QString l_sText;
	QTableWidgetItem *l_oItem = item(i_iRow, i_iCol);
	if (l_oItem) l_sText = l_oItem->text();
	else l_sText = "";

	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	QHash<QPair<int, int>, QString> changed;

	for (int i=0; i < rowCount(); ++i)
	{
		for (int j=0; j < columnCount(); ++j)
		{
			changed[QPair<int, int>(i, j)] = l_oData.m_oTableData[QPair<int, int>(i, j)];
		}
	}
	changed[QPair<int, int>(i_iRow, i_iCol)] = l_sText;

	mem_table *tmp = new mem_table(m_oMediator);
	tmp->m_iId = m_iId;
	tmp->oldNRows = tmp->newNRows = l_oData.m_iNumRows;
	tmp->oldNCols = tmp->newNCols = l_oData.m_iNumCols;
	tmp->oldData = l_oData.m_oTableData;
	tmp->newData = changed;
	tmp->apply();
}

void table_view::add_row()
{
	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	QHash<QPair<int, int>, QString> changed;
	for (int i=0; i < rowCount() + 1; ++i)
	{
		int ii = i;
		if (ii >= m_oAddRowAct->row) ii++;
		for (int j=0; j < columnCount(); ++j)
		{
			changed[QPair<int, int>(m_oAddRowAct->row, j)] = "";
			changed[QPair<int, int>(ii, j)] = l_oData.m_oTableData[QPair<int, int>(i, j)];
		}
	}

	mem_table *tmp = new mem_table(m_oMediator);
	tmp->m_iId = m_iId;
	tmp->oldNRows = l_oData.m_iNumRows;
	tmp->oldNCols = l_oData.m_iNumCols;
	tmp->newNRows = rowCount() + 1;
	tmp->newNCols = columnCount();
	tmp->oldData = l_oData.m_oTableData;
	tmp->newData = changed;
	tmp->apply();
}

void table_view::add_column()
{
	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	QHash<QPair<int, int>, QString> changed;
	for (int i=0; i < rowCount(); ++i)
	{
		for (int j=0; j < columnCount() + 1; ++j)
		{
			int jj = j;
			if (jj >= m_oAddRowAct->col) jj++;

			changed[QPair<int, int>(i, m_oAddRowAct->col)] = "";
			changed[QPair<int, int>(i, jj)] = l_oData.m_oTableData[QPair<int, int>(i, j)];
		}
	}

	mem_table *tmp = new mem_table(m_oMediator);
	tmp->m_iId = m_iId;
	tmp->oldNRows = l_oData.m_iNumRows;
	tmp->oldNCols = l_oData.m_iNumCols;
	tmp->newNRows = rowCount();
	tmp->newNCols = columnCount() + 1;
	tmp->oldData = l_oData.m_oTableData;
	tmp->newData = changed;
	tmp->apply();
}

void table_view::rm_row()
{
	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	QHash<QPair<int, int>, QString> changed;
	for (int i=0; i < rowCount(); ++i)
	{
		int ii = i;
		if (ii == m_oRmRowAct->row) continue;
		if (ii > m_oRmRowAct->row) ii--;
		for (int j=0; j < columnCount(); ++j)
		{
			changed[QPair<int, int>(ii, j)] = l_oData.m_oTableData[QPair<int, int>(i, j)];
		}
	}

	mem_table *tmp = new mem_table(m_oMediator);
	tmp->m_iId = m_iId;
	tmp->oldNRows = l_oData.m_iNumRows;
	tmp->oldNCols = l_oData.m_iNumCols;
	tmp->newNRows = rowCount() - 1;
	tmp->newNCols = columnCount();
	tmp->oldData = l_oData.m_oTableData;
	tmp->newData = changed;
	tmp->apply();
}

void table_view::rm_column()
{
	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	QHash<QPair<int, int>, QString> changed;
	for (int i=0; i < rowCount(); ++i)
	{
		for (int j=0; j < columnCount(); ++j)
		{
			int jj = j;
			if (jj == m_oRmRowAct->col) continue;
			if (jj > m_oAddRowAct->col) jj--;

			changed[QPair<int, int>(i, jj)] = l_oData.m_oTableData[QPair<int, int>(i, j)];
		}
	}

	mem_table *tmp = new mem_table(m_oMediator);
	tmp->m_iId = m_iId;
	tmp->oldNRows = l_oData.m_iNumRows;
	tmp->oldNCols = l_oData.m_iNumCols;
	tmp->newNRows = rowCount();
	tmp->newNCols = columnCount() - 1;
	tmp->oldData = l_oData.m_oTableData;
	tmp->newData = changed;
	tmp->apply();
}

void table_view::resize_table()
{
	table_dialog l_oGen(this);
	l_oGen.m_oRows->setValue(rowCount());
	l_oGen.m_oCols->setValue(columnCount());
	if (l_oGen.exec() == QDialog::Accepted)
	{
		data_item& l_oData = m_oMediator->m_oItems[m_iId];
		mem_table *tmp = new mem_table(m_oMediator);
		tmp->m_iId = m_iId;
		tmp->oldNRows = l_oData.m_iNumRows;
		tmp->oldNCols = l_oData.m_iNumCols;
		tmp->newNRows = l_oGen.m_oRows->value();
		tmp->newNCols = l_oGen.m_oCols->value();
		tmp->oldData = tmp->newData = l_oData.m_oTableData;
		tmp->apply();
	}
}

void table_view::notify_table(int id)
{
	if (m_bFreeze)
	{
		return;
	}
	m_bFreeze = true;

	data_item& l_oData = m_oMediator->m_oItems[id];

	if (l_oData.m_iNumRows != rowCount() || l_oData.m_iNumCols != columnCount())
	{
		setRowCount(l_oData.m_iNumRows);
		setColumnCount(l_oData.m_iNumCols);
	}

	QPair<int, int> t;
	foreach (t, l_oData.m_oTableData.keys())
	{
		QTableWidgetItem *it = item(t.first, t.second);
		if (!it)
		{
			it = new QTableWidgetItem();
			setItem(t.first, t.second, it);
		}
		it->setText(l_oData.m_oTableData[t]);
	}
	resizeRowsToContents();
	repaint();
	m_bFreeze = false;
}

void table_view::notify_select(const QList<int>& unsel, const QList<int>& sel)
{
	m_bFreeze = true;

	bool one = (sel.size() == 1);
	if (one) {
		m_iId = sel.at(0);
		Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
		data_item& l_oData = m_oMediator->m_oItems[m_iId];

		setRowCount(l_oData.m_iNumRows);
		setColumnCount(l_oData.m_iNumCols);

		for (int i=0; i < rowCount(); ++i)
		{
			for (int j=0; j < columnCount(); ++j)
			{
				QTableWidgetItem *l_oItem = new QTableWidgetItem();
				l_oItem->setText(l_oData.m_oTableData[QPair<int, int>(i, j)]);
				setItem(i, j, l_oItem);
			}
		}
	} else {
		m_iId = NO_ITEM;
		setRowCount(0);
		setColumnCount(0);
	}
	resizeRowsToContents();
	repaint();
	m_bFreeze = false;
}

table_view_filter::table_view_filter(QWidget* i_oParent): QStyledItemDelegate(i_oParent)
{
	m_oParent = dynamic_cast<table_view*>(i_oParent);
}

QWidget* table_view_filter::createEditor(QWidget *i_oParent, const QStyleOptionViewItem&, const QModelIndex&) const
{
	QPlainTextEdit *l_oEditor = new QPlainTextEdit(i_oParent);
	l_oEditor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	l_oEditor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	return l_oEditor;
}

void table_view_filter::setEditorData(QWidget *i_oEditor, const QModelIndex& i_oModelIndex) const
{
	QPlainTextEdit *l_oEditor = dynamic_cast<QPlainTextEdit *>(i_oEditor);
	l_oEditor->setPlainText(i_oModelIndex.data().toString());
}

void table_view_filter::setModelData(QWidget *i_oEditor, QAbstractItemModel* i_oModel, const QModelIndex& i_oIndex) const
{
	QPlainTextEdit* l_oEditor = dynamic_cast<QPlainTextEdit *>(i_oEditor);
	i_oModel->setData(i_oIndex, l_oEditor->toPlainText());
}

