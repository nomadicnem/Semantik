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
#include "box_sequence.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include"box_control_point.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

box_sequence::box_sequence(box_view* view, int id) : box_item(view, id)
{
	setZValue(80);

	m_oResize->setRect(-CTRLSIZE, 1-CTRLSIZE, CTRLSIZE, CTRLSIZE);

	m_oResizeStick = new box_resize_point(m_oView, this);
	m_oResizeStick->setCursor(Qt::SizeVerCursor);
	m_oResizeStick->setRect(-CTRLSIZE/2., -CTRLSIZE, CTRLSIZE, CTRLSIZE);
	m_oResizeStick->hide();
	m_oResizeStick->setParentItem(this);

	update_size();
}

void box_sequence::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setWidthF(0.01 + 1);
	l_oPen.setCosmetic(false);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	painter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);
	QPointF l_oMidBottom = (l_oRect.bottomLeft() + l_oRect.bottomRight()) / 2.;
	l_oRect.setBottomLeft(l_oRect.topLeft() + QPointF(0, m_iBoxHeight));

	QColor bc = m_oBox->getColor(m_oView->m_oMediator);
	if (m_oView->m_bDisableGradient)
	{
		painter->setBrush(bc);
	}
	else
	{
		QLinearGradient linearGradient(0, 0, l_oRect.width(), 0);
		linearGradient.setColorAt(0.0, bc);
		linearGradient.setColorAt(1.0, bc.darker(GRADVAL));
		painter->setBrush(linearGradient);
	}

	painter->drawRect(l_oRect);

	QPointF l_oBottom = (l_oRect.bottomLeft() + l_oRect.bottomRight()) / 2.;
	painter->drawLine(l_oBottom, l_oMidBottom);

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes
	QAbstractTextDocumentLayout * lay = doc.documentLayout();
	qreal yoff = lay->documentSize().height();

	painter->translate(OFF, OFF + (l_oRect.height() - 2 * OFF - yoff) / 2.);
	lay->draw(painter, ctx);
}

void box_sequence::update_size()
{
	m_iWW = m_oBox->m_iWW;
	m_iHH = m_oBox->m_iHH;
	m_iBoxHeight = m_oBox->m_iBoxHeight;

	QTextOption l_oOption = doc.defaultTextOption();
	l_oOption.setAlignment(m_oBox->m_iAlign);
	doc.setDefaultTextOption(l_oOption);
	doc.setPlainText(m_oBox->m_sText);
	doc.setTextWidth(m_iWW - 2 * OFF);

	prepareGeometryChange();
	setRect(0, 0, m_iWW, m_iHH);
	m_oChain->setPos(boundingRect().right() + 3, 0);
	update_links();
	update_sizers();

	// change the shape
	QRectF l_oRect = rect();
	QPointF l_oMidBottom = (l_oRect.bottomLeft() + l_oRect.bottomRight()) / 2.;
	l_oRect.setBottomLeft(l_oRect.topLeft() + QPointF(0, m_iBoxHeight));

	QPointF l_oMidUp = (l_oRect.bottomLeft() + l_oRect.bottomRight()) / 2.;
	QRectF l_o = QRectF(l_oMidBottom, l_oMidUp);

	QPainterPath p;
	p.addRect(l_oRect);
	l_o.adjust(-10, 0, 10, 0);
	p.addRect(l_o);
	inner_shape = p;

	update();
}

QPointF box_sequence::validate_point(box_resize_point *p, const QPointF & orig)
{
	if (p == m_oResize)
	{
		QSize s = best_size(orig);
		m_iLastStretchX = s.width();
		m_iBoxHeight = s.height();
		m_iLastStretchY = m_iBoxHeight + (m_oBox->m_iHH - m_oBox->m_iBoxHeight);
		m_oChain->setPos(m_iLastStretchX + 3, 0);

		prepareGeometryChange();
		setRect(0, 0, m_iLastStretchX, m_iLastStretchY);
		m_iWW = m_iLastStretchX;
		m_iHH = m_iLastStretchY;
		update();
		update_links();

		m_oResizeStick->setPos(m_iLastStretchX/2., m_iLastStretchY);

		m_oView->message(i18n("%1 x %2 (%3+%4)",
			QString::number(m_iLastStretchX),
			QString::number(m_iLastStretchY),
			QString::number(m_iBoxHeight),
			QString::number(m_iLastStretchY - m_iBoxHeight)), 1000);
		return QPointF(m_iLastStretchX, m_iBoxHeight);
	}
	else if (p == m_oResizeStick)
	{
		int l_iMinOff = 2 * GRID;
		foreach (box_link* l_oLink, m_oView->m_oLinks)
		{
			if (l_oLink->m_oInnerLink.m_iParent == m_iId)
			{
				int l_iTmp = l_oLink->m_oInnerLink.m_iParentPos / MUL;
				if (l_iTmp > l_iMinOff - GRID) {
					l_iMinOff = l_iTmp + GRID;
				}
			}
			else if (l_oLink->m_oInnerLink.m_iChild == m_iId)
			{
				int l_iTmp = l_oLink->m_oInnerLink.m_iChildPos / MUL;
				if (l_iTmp > l_iMinOff - GRID) {
					l_iMinOff = l_iTmp + GRID;
				}
			}
		}

		m_iHH = m_iLastStretchY = int_val(orig.y());
		if (m_iLastStretchY < m_iBoxHeight + l_iMinOff) m_iLastStretchY = m_iBoxHeight + l_iMinOff;

		prepareGeometryChange();
		setRect(0, 0, m_oBox->m_iWW, m_iLastStretchY);
		update();
		update_links();
		m_oView->message(i18n("%1 x %2 (%3+%4)",
			QString::number(m_oBox->m_iWW),
			QString::number(m_iLastStretchY),
			QString::number(m_iBoxHeight),
			QString::number(m_iLastStretchY - m_iBoxHeight)), 1000);
		return QPointF(m_oBox->m_iWW/2., m_iLastStretchY);
	}
	return orig;
}

QPainterPath box_sequence::shape() const
{
	return inner_shape;
}

void box_sequence::commit_size(box_resize_point *p)
{
	mem_size_sequence *mem = new mem_size_sequence(m_oView->m_oMediator, m_oView->m_iId, m_oBox);
	mem->m_oNextBox.m_iWW = m_iWW;
	mem->m_oNextBox.m_iHH = m_iHH;
	mem->m_oNextBox.m_iBoxHeight = m_iBoxHeight;
	mem->apply();
}

void box_sequence::freeze(bool b)
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

void box_sequence::update_data()
{
	m_iBoxHeight = m_oBox->m_iBoxHeight;
	box_item::update_data();
	update_size();
}

void box_sequence::update_sizers()
{
	m_oResize->setPos(m_iWW, m_iBoxHeight);
	m_oResizeStick->setPos(m_iWW / 2., m_iHH);
}

void box_sequence::update_selection()
{
	m_oResize->setVisible(isSelected());
	m_oResizeStick->setVisible(isSelected());
	m_oChain->setVisible(isSelected());
}

QPoint box_sequence::get_point(int i_oP)
{
	switch (i_oP & data_link::COORD) {
		case data_link::NORTH:
			return box_item::get_point(i_oP);
		case data_link::SOUTH:
		case data_link::WEST:
		case data_link::EAST:
			{
				QRectF l_oR = rectPos();
				QPointF l_oTopLeft = l_oR.topLeft();
				return QPoint(l_oTopLeft.x() + l_oR.width()/2., l_oTopLeft.y() + m_iBoxHeight + i_oP / MUL);
			}
	}
	Q_ASSERT(false);
	return QPoint(0, 0);
}

int box_sequence::choose_position(const QPointF& i_oP, box_link* i_oLink, box_control_point* i_oControl)
{
	QRectF l_oR = rectPos();
	if (i_oP.y() < l_oR.top() + m_iBoxHeight)
	{
		return box_item::choose_position(QPointF(i_oP.x(), l_oR.top()), i_oLink, i_oControl);
	}
	else
	{
		QPointF l_oMid = (i_oLink->m_oStartPoint->pos() + i_oLink->m_oEndPoint->pos()) / 2.;
		int l_iRet = 0;
		if (l_oMid.x() > l_oR.x() + l_oR.width() / 2.)
		{
			l_iRet = data_link::EAST;
		}
		else
		{
			l_iRet = data_link::WEST;
		}

		if (i_oLink->m_oStartPoint == i_oControl && i_oLink->m_oInnerLink.m_iChild == m_iId)
		{
			int l_i = i_oLink->m_oInnerLink.m_iChildPos & data_link::COORD;
			if (l_i == data_link::EAST || l_i == data_link::WEST)
			{
				l_iRet = l_i;
			}
		}
		else if (i_oLink->m_oEndPoint == i_oControl && i_oLink->m_oInnerLink.m_iParent == m_iId)
		{
			int l_i = i_oLink->m_oInnerLink.m_iParentPos & data_link::COORD;
			if (l_i == data_link::EAST || l_i == data_link::WEST)
			{
				l_iRet = l_i;
			}
		}

		int l_iDiff = int_val(i_oP.y() - l_oR.top() - m_iBoxHeight);
		if (l_iDiff < GRID)
		{
			l_iDiff = GRID;
		}

		int l_iRet2 = l_iRet + l_iDiff * MUL;
		return l_iRet2;
	}
}

int box_sequence::optimize_position(const QPointF& i_oP)
{
	QRectF l_oR = rectPos();
	int l_iDiff = int_val(i_oP.y() - l_oR.top() - m_iBoxHeight);
	if (l_iDiff < GRID)
	{
		l_iDiff = GRID;
	}
	else if (l_iDiff >= l_oR.height() - m_iBoxHeight - GRID)
	{
		l_iDiff = int_val(l_oR.height() - m_iBoxHeight - GRID);
	}

	if (i_oP.x() > l_oR.x() + l_oR.width() / 2.)
	{
		return l_iDiff * MUL + data_link::EAST;
	}
	else
	{
		return l_iDiff * MUL + data_link::WEST;
	}
}

int box_sequence::may_use(const QPair<int, int>& i_oA, const QPair<int, int>& i_oB, const int i_iPos, const QPoint& i_oP) const
{
	QRectF l_oR1 = QRectF(rectPos().topLeft(), rectPos().topRight() + QPointF(0, m_iBoxHeight));
	if (!connectable::may_use(i_oA, i_oB, l_oR1, i_iPos, i_oP))
	{
		return false;
	}
	QRectF l_oR2 = QRectF((l_oR1.bottomLeft() + l_oR1.bottomRight()) / 2.,
			(rectPos().bottomLeft() + rectPos().bottomRight()) / 2.);
	if (!connectable::may_use(i_oA, i_oB, l_oR2, i_iPos, i_oP))
	{
		return false;
	}
	if (i_oA.first == i_oB.first && i_oA.first >= l_oR2.left() - 2 && i_oA.first <= l_oR2.right() + 2)
	{
		int l_oUp = qMin(i_oA.second, i_oB.second);
		int l_oDown = qMax(i_oA.second, i_oB.second);
		if (l_oDown <= l_oR2.top() && l_oUp >= l_oR2.bottom())
		{
			return false;
		}
	}

	if (i_oA.second == i_oB.second && i_oA.second == i_oP.y())
	{
		int l_iDir = i_iPos & data_link::COORD;
		if (l_iDir == data_link::WEST)
		{
			if (qMin(i_oA.first, i_oB.first) == l_oR2.right())
			{
				return false;
			}
		}
		else if (l_iDir == data_link::EAST)
		{
			if (qMax(i_oA.first, i_oB.first) == l_oR2.left())
			{
				return false;
			}
		}
	}

	return true;
}

