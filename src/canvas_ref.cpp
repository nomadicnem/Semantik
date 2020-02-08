// Thomas Nagy 2018-2020 GPLV3

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include <QPen>
#include <QtDebug>

#include <cmath>

#include "sem_mediator.h" 
#include  "con.h"
#include "canvas_item.h"
#include  "canvas_ref.h"
#include "canvas_view.h"

	canvas_ref::canvas_ref(canvas_view *i_oGraphWidget, canvas_item *i_oFrom, canvas_item *i_oTo)
: canvas_link(i_oGraphWidget, i_oFrom, i_oTo)
{
	setZValue(40);
}

QColor 
canvas_ref::get_color()
{
return m_oGraph->m_oMediator->m_oAltArrowColor;
}
void canvas_ref::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *i_oStyle, QWidget *i_oWidget)
{
	QColor l_o = get_color();
	setBrush(l_o);
	setPen(l_o);
	QGraphicsPathItem::paint(i_oPainter, i_oStyle, i_oWidget);
}

