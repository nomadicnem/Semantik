// Thomas Nagy 2018 GPLV3

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
	m_oColor = QColor(REF_DEFAULT_COLOR);
	QBrush l_oBrush(m_oColor);
	setBrush(l_oBrush);
	setPen(m_oColor);
	setZValue(40);
}

