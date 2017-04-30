// Thomas Nagy 2007-2017 GPLV3

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
#include "box_decision.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_decision::box_decision(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setBold(true);
	doc.setDefaultFont(font);

	setZValue(90);
}

void box_decision::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QColor bc(m_oBox->color);
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

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}
	painter->setPen(l_oPen);

	QPointF pts[4]; // NESW
	QPointF topLeft = l_oRect.topLeft();
	pts[0] = topLeft + QPointF(l_oRect.width()/2, 0);
	pts[1] = topLeft + QPointF(l_oRect.width(),   l_oRect.height()/2);
	pts[2] = topLeft + QPointF(l_oRect.width()/2, l_oRect.height());
	pts[3] = topLeft + QPointF(0,                 l_oRect.height()/2);

	painter->drawPolygon(pts, 4);
}

QSize box_decision::best_size(const QPointF &dims)
{
	int x = dims.x();
	x = GRID * (x / GRID);
	if (x < 2 * GRID) x = 2 * GRID;
	return QSize(x, x);
}

