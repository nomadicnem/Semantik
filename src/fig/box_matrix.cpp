// Thomas Nagy 2007-2018 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include \
  "matrix_dialog.h"
#include "box_matrix.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

box_matrix::box_matrix(box_view* view, int id) : box_item(view, id)
{
	setZValue(80);
	update_size();
}

void box_matrix::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setWidthF(0.01 + 1);
	l_oPen.setCosmetic(false);
	painter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);


	int l_iPos = 0;
	foreach (box_resize_point *l_oTmp, m_oRowSizers)
	{
		l_iPos += l_oTmp->m_iPosition;
		painter->drawLine(l_oRect.left()+pad, l_iPos, l_oRect.right()-pad, l_iPos);
	}
	l_iPos = 0;
	foreach (box_resize_point *l_oTmp, m_oColSizers)
	{
		l_iPos += l_oTmp->m_iPosition;
		painter->drawLine(l_iPos, l_oRect.top()+pad, l_iPos, l_oRect.bottom()-pad);
	}
	painter->drawRect(l_oRect);
}

void box_matrix::update_size() {
	m_iWW = m_oBox->m_iWW;
	m_iHH = m_oBox->m_iHH;

	prepareGeometryChange();
	setRect(0, 0, m_iWW, m_iHH);
	m_oChain->setPos(boundingRect().right() + 3, 0);
	update_links();
	update_sizers();
}

void box_matrix::properties()
{
	matrix_dialog l_o(m_oView);
	l_o.m_oRows->setValue(m_oBox->m_oRowSizes.length() + 1);
	l_o.m_oCols->setValue(m_oBox->m_oColSizes.length() + 1);
	if (l_o.exec() == QDialog::Accepted) {
		mem_matrix *mem = new mem_matrix(m_oView->m_oMediator, m_oView->m_iId);
		mem->init(m_oBox);

		while (mem->m_oNewRowSizes.size() < l_o.m_oRows->value() - 1)
		{
			int l_iTmp = mem->m_iNewHH;
			foreach (int l_i, mem->m_oNewRowSizes) {
				l_iTmp -= l_i;
			}
			mem->m_oNewRowSizes.push_back(l_iTmp);
			mem->m_iNewHH += l_iTmp;
		}
		while (mem->m_oNewRowSizes.size() > l_o.m_oRows->value() - 1)
		{
			mem->m_iNewHH = 0;
			foreach (int l_i, mem->m_oNewRowSizes) {
				mem->m_iNewHH += l_i;
			}
			mem->m_oNewRowSizes.removeLast();
		}

		while (mem->m_oNewColSizes.size() < l_o.m_oCols->value() - 1)
		{
			int l_iTmp = mem->m_iNewWW;
			foreach (int l_i, mem->m_oNewColSizes) {
				l_iTmp -= l_i;
			}
			mem->m_oNewColSizes.push_back(l_iTmp);
			mem->m_iNewWW += l_iTmp;
		}
		while (mem->m_oNewColSizes.size() > l_o.m_oCols->value() - 1)
		{
			mem->m_iNewWW = 0;
			foreach (int l_i, mem->m_oNewColSizes) {
				mem->m_iNewWW += l_i;
			}
			mem->m_oNewColSizes.removeLast();
		}

		mem->apply();
	}
}

QPointF box_matrix::validate_point(box_resize_point *p, const QPointF & orig)
{
	if (p == m_oResize)
	{
		int l_iMinX = 2 * GRID;
		for (int i = 0; i < m_oColSizers.size(); ++i)
		{
			l_iMinX += m_oColSizers.at(i)->m_iPosition;
		}
		int l_iMinY = 2 * GRID;
		for (int i = 0; i < m_oRowSizers.size(); ++i)
		{
			l_iMinY += m_oRowSizers.at(i)->m_iPosition;
		}
		m_iLastStretchX = int_val(orig.x());
		if (m_iLastStretchX < l_iMinX) m_iLastStretchX = l_iMinX;
		m_iLastStretchY = int_val(orig.y());
		if (m_iLastStretchY < l_iMinY) m_iLastStretchY = l_iMinY;

		m_oChain->setPos(m_iLastStretchX + 3, 0);
		prepareGeometryChange();
		setRect(0, 0, m_iLastStretchX, m_iLastStretchY);
		update();
		update_links();

		int l_iYY = 0;
		for (int i = 0; i < m_oBox->m_oRowSizes.size(); ++i)
		{
			box_resize_point *l_oTmp = m_oRowSizers.at(i);
			l_iYY += l_oTmp->m_iPosition;
			l_oTmp->setPos(m_iLastStretchX, l_iYY);
		}

		int l_iXX = 0;
		for (int i = 0; i < m_oBox->m_oColSizes.size(); ++i)
		{
			box_resize_point *l_oTmp = m_oColSizers.at(i);
			l_iXX += l_oTmp->m_iPosition;
			l_oTmp->setPos(l_iXX, m_iLastStretchY);
		}

		m_oView->message(m_oView->trUtf8("Last colunn: %1px, last row: %2px (size: %3 x %4)").arg(
			QString::number(m_iLastStretchY - l_iYY),
			QString::number(m_iLastStretchX - l_iXX),
			QString::number(m_iLastStretchX),
			QString::number(m_iLastStretchY)
		), 5000);

		return QPointF(m_iLastStretchX, m_iLastStretchY);
	}
	else
	{
		int l_iColIdx = m_oColSizers.indexOf(p);
		int l_iRowIdx = m_oRowSizers.indexOf(p);
		int l_iPrev = 0;
		if (l_iColIdx >= 0)
		{
			for (int i = 0; i < l_iColIdx; ++i)
			{
				l_iPrev += m_oColSizers.at(i)->m_iPosition;
			}
			m_iLastStretchX = int_val(orig.x());
			if (m_iLastStretchX < l_iPrev + 2 * GRID) m_iLastStretchX = l_iPrev + 2 * GRID;

			p->m_iPosition = m_iLastStretchX - l_iPrev;

			int l_iAcc = m_iLastStretchX;
			for (int i = l_iColIdx + 1; i < m_oColSizers.size(); ++i)
			{
				box_resize_point *l_oNext = m_oColSizers.at(i);
				l_iAcc += l_oNext->m_iPosition;
				l_oNext->setPos(l_iAcc, m_oBox->m_iHH);
			}
			int l_iNewW = m_oBox->m_iWW + p->m_iPosition - m_oBox->m_oColSizes.at(l_iColIdx);
			m_oResize->setPos(l_iNewW, m_oBox->m_iHH);
			m_oResize->m_iPosition = l_iNewW;

			m_oView->message(m_oView->trUtf8("Column %1: %2px (size: %3 x %4)").arg(
				QString::number(l_iColIdx + 1),
				QString::number(p->m_iPosition),
				QString::number(l_iNewW),
				QString::number(m_oBox->m_iHH)
			), 5000);

			prepareGeometryChange();
			setRect(0, 0, l_iNewW, m_oBox->m_iHH);

			int l_iOff = 0;
			for (int i = 0; i < m_oBox->m_oRowSizes.size(); ++i)
			{
				box_resize_point *l_oTmp = m_oRowSizers.at(i);
				l_iOff += l_oTmp->m_iPosition;
				l_oTmp->setPos(l_iNewW, l_iOff);
			}

			m_oChain->setPos(l_iNewW + 3, 0);
			update();
			update_links();
			return QPointF(m_iLastStretchX, m_oBox->m_iHH);
		}
		else if (l_iRowIdx >= 0)
		{
			for (int i = 0; i < l_iRowIdx; ++i)
			{
				l_iPrev += m_oRowSizers.at(i)->m_iPosition;
			}
			m_iLastStretchY = int_val(orig.y());
			if (m_iLastStretchY < l_iPrev + 2 * GRID) m_iLastStretchY = l_iPrev + 2 * GRID;

			p->m_iPosition = m_iLastStretchY - l_iPrev;

			int l_iAcc = m_iLastStretchY;
			for (int i = l_iRowIdx + 1; i < m_oRowSizers.size(); ++i)
			{
				box_resize_point *l_oNext = m_oRowSizers.at(i);
				l_iAcc += l_oNext->m_iPosition;
				l_oNext->setPos(m_oBox->m_iWW, l_iAcc);
			}

			int l_iNewH = m_oBox->m_iHH + p->m_iPosition - m_oBox->m_oRowSizes.at(l_iRowIdx);
			m_oResize->setPos(m_oBox->m_iWW, l_iNewH);
			m_oResize->m_iPosition = l_iNewH;

			m_oView->message(m_oView->trUtf8("Row %1: %2px (size: %3 x %4)").arg(
				QString::number(l_iRowIdx + 1),
				QString::number(p->m_iPosition),
				QString::number(m_oBox->m_iWW),
				QString::number(l_iNewH)
			), 5000);

			prepareGeometryChange();
			setRect(0, 0, m_oBox->m_iWW, l_iNewH);

			int l_iOff = 0;
			for (int i = 0; i < m_oBox->m_oColSizes.size(); ++i)
			{
				box_resize_point *l_oTmp = m_oColSizers.at(i);
				l_iOff += l_oTmp->m_iPosition;
				l_oTmp->setPos(l_iOff, l_iNewH);
			}

			update();
			update_links();
			return QPointF(m_oBox->m_iWW, m_iLastStretchY);
		}
	}
	return orig;
}

void box_matrix::commit_size(box_resize_point *p)
{
	if (p == m_oResize)
	{
		QRect r_orig(m_oBox->m_iXX, m_oBox->m_iYY, m_oBox->m_iWW, m_oBox->m_iHH);
		QRect r_dest(m_oBox->m_iXX, m_oBox->m_iYY, m_iLastStretchX, m_iLastStretchY);
		if (r_orig != r_dest)
		{
			mem_size_box *mem = new mem_size_box(m_oView->m_oMediator, m_oView->m_iId);
			mem->prev_values[m_oBox] = r_orig;
			mem->next_values[m_oBox] = r_dest;
			mem->apply();
		}
	}
	else
	{
		int l_iColIdx = m_oColSizers.indexOf(p);
		int l_iRowIdx = m_oRowSizers.indexOf(p);
		if (l_iColIdx >= 0)
		{
			if (m_oColSizers.at(l_iColIdx)->m_iPosition != m_oBox->m_oColSizes.at(l_iColIdx))
			{
				mem_matrix *mem = new mem_matrix(m_oView->m_oMediator, m_oView->m_iId);
				mem->init(m_oBox);
				mem->m_oNewColSizes[l_iColIdx] = m_oColSizers.at(l_iColIdx)->m_iPosition;
				mem->m_iNewWW = m_oResize->m_iPosition;
				mem->apply();
			}
		}
		else if (l_iRowIdx >= 0)
		{
			if (m_oRowSizers.at(l_iRowIdx)->m_iPosition != m_oBox->m_oRowSizes.at(l_iRowIdx))
			{
				mem_matrix *mem = new mem_matrix(m_oView->m_oMediator, m_oView->m_iId);
				mem->init(m_oBox);
				mem->m_oNewRowSizes[l_iRowIdx] = m_oRowSizers.at(l_iRowIdx)->m_iPosition;
				mem->m_iNewHH = m_oResize->m_iPosition;
				mem->apply();
			}
		}
		else
		{
			Q_ASSERT(false);
		}
	}
}

void box_matrix::freeze(bool b)
{
	if (b)
	{
		setFlags(ItemIsSelectable);
		m_iLastStretchX = 0;
		m_iLastStretchY = 0;
	}
	else
	{
		setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
	}
}

void box_matrix::allocate_sizers()
{
	while (m_oRowSizers.size() < m_oBox->m_oRowSizes.size())
	{
		box_resize_point *l_oTmp = new box_resize_point(m_oView, this);
		l_oTmp->setCursor(Qt::SizeVerCursor);
		l_oTmp->setRect(-CTRLSIZE, 0.5-CTRLSIZE, CTRLSIZE, CTRLSIZE);
		l_oTmp->setParentItem(this);
		m_oRowSizers.append(l_oTmp);
	}
	while (m_oRowSizers.size() > m_oBox->m_oRowSizes.size())
	{
		delete m_oRowSizers.takeLast();
	}
	while (m_oColSizers.size() < m_oBox->m_oColSizes.size())
	{
		box_resize_point *l_oTmp = new box_resize_point(m_oView, this);
		l_oTmp->setCursor(Qt::SizeHorCursor);
		l_oTmp->setRect(0.5-CTRLSIZE, -CTRLSIZE, CTRLSIZE, CTRLSIZE);
		l_oTmp->setParentItem(this);
		m_oColSizers.append(l_oTmp);
	}
	while (m_oColSizers.size() > m_oBox->m_oColSizes.size())
	{
		delete m_oColSizers.takeLast();
	}
}

void box_matrix::fix_sizers_visibility()
{
	bool l_bSelected = isSelected();
	foreach(box_resize_point *l_oTmp, m_oRowSizers)
	{
		l_oTmp->setVisible(l_bSelected);
	}
	foreach(box_resize_point *l_oTmp, m_oColSizers)
	{
		l_oTmp->setVisible(l_bSelected);
	}
}

void box_matrix::update_sizers()
{
	allocate_sizers();
	m_oResize->setPos(m_oBox->m_iWW, m_oBox->m_iHH);

	int l_iOff = 0;
	for (int i = 0; i < m_oBox->m_oRowSizes.size(); ++i)
	{
		box_resize_point *l_oTmp = m_oRowSizers.at(i);
		l_oTmp->m_iPosition = m_oBox->m_oRowSizes.at(i);
		l_iOff += l_oTmp->m_iPosition;
		l_oTmp->setPos(m_oBox->m_iWW, l_iOff);
	}

	l_iOff = 0;
	for (int i = 0; i < m_oBox->m_oColSizes.size(); ++i)
	{
		box_resize_point *l_oTmp = m_oColSizers.at(i);
		l_oTmp->m_iPosition = m_oBox->m_oColSizes.at(i);
		l_iOff += l_oTmp->m_iPosition;
		l_oTmp->setPos(l_iOff, m_oBox->m_iHH);
	}
}

void box_matrix::update_selection()
{
	allocate_sizers();
	fix_sizers_visibility();
	m_oResize->setVisible(isSelected());
	m_oChain->setVisible(isSelected());
}

