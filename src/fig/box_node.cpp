// Thomas Nagy 2007-2018 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include <QLinearGradient>
#include "box_node.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_node::box_node(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setBold(true);
	doc.setDefaultFont(font);

	// FIXME move the resize handle by x-10
	setZValue(90);
	x_text_off = 2 * OFF + 20;
	y_text_off = 2 * OFF + 30;
}

void box_node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	doc.setDefaultFont(scene()->font());

	QColor bc = m_oBox->getColor(m_oView->m_oMediator);
	painter->setBrush(bc);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);


	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}

	painter->setPen(l_oPen);
	QRectF br = l_oRect.adjusted(0, 10, -10, 0);
	painter->drawRect(br);


	painter->setBrush(bc.lighter(GRADVAL));

	QPointF pts[4];
	pts[0] = br.topLeft();
	pts[1] = br.topLeft() + QPointF(10, -10);
	pts[2] = br.topRight() + QPointF(10, -10);
	pts[3] = br.topRight();

	painter->drawPolygon(pts, 4);

	painter->setBrush(bc.darker(GRADVAL));

	pts[0] = br.topRight();
	pts[1] = br.topRight() + QPointF(10, -10);
	pts[2] = br.bottomRight() + QPointF(10, -10);
	pts[3] = br.bottomRight();

	painter->drawPolygon(pts, 4);

	painter->translate((rect().width() - 10 - doc.size().width())/2, OFF + 10);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes

	doc.documentLayout()->draw(painter, ctx);
}

void box_node::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
	m_oLastPressPoint = e->pos();
	if (m_oLastPressPoint.x() > m_iWW - GRID - 10 && m_oLastPressPoint.x() < m_iWW - 10 && m_oLastPressPoint.y() > m_iHH - GRID)
	{
		setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
	}
	QGraphicsRectItem::mousePressEvent(e);
}


