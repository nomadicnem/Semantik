/// Thomas Nagy 2007-2019 GPLV3

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
 #include    <QLinearGradient> 
#include <QColor>
#include <QPen>
#include <QtDebug>

#include <cmath>

#include "sem_mediator.h" 
#include  "con.h"
#include "canvas_item.h"
#include  "canvas_link.h"
#include "canvas_view.h"

	canvas_link::canvas_link(canvas_view *i_oGraphWidget, canvas_item *i_oFrom, canvas_item *i_oTo)
: QGraphicsPathItem(), m_oGraph(i_oGraphWidget)
{
	setFlags(NULL);

	m_oFrom = i_oFrom;
	m_oTo = i_oTo;

	m_bSel = false;

	m_oFrom->add_link(this);
	m_oTo->add_link(this);

	setZValue(50);

	i_oGraphWidget->scene()->addItem(this);
	//setParentItem(m_oFrom);
}

void canvas_link::rm_link()
{
	m_oFrom->rm_link(this);
	m_oTo->rm_link(this);
}

void canvas_link::update_pos()
{
	#ifdef _DEBUG
		return;
	#endif
	prepareGeometryChange();
	if (m_oFrom->m_oGraph->m_oMediator->m_iConnType == 0)
	{
		update_triangle();
	}
	else
	{
		update_spline();
	}
	update();
}

void canvas_link::update_selected()
{
	bool l_b = (m_oFrom->isSelected() || m_oTo->isSelected());
	if (m_bSel != l_b)
	{
		m_bSel = l_b;
		if (m_bSel) setBrush(QBrush(Qt::yellow)); else setBrush(Qt::black);
	}
}

void canvas_link::update_triangle()
{
	QRectF l_oR1 = m_oFrom->boundingRect();
	QRectF l_oR2 = m_oTo->boundingRect();

	double _x1 = m_oFrom->x() + l_oR1.width()/2. - OFF;
	double _y1 = m_oFrom->y() + l_oR1.height()/2. - OFF;

	double _x2 = m_oTo->x() + l_oR2.width()/2. - OFF;
	double _y2 = m_oTo->y() + l_oR2.height()/2. - OFF;

	double _X1 = _x2 - _x1;
	double _Y1 = _y2 - _y1;

	double l_oDist = 1;
	double _coeff = 0;
	if (_Y1*_Y1>_X1*_X1) _coeff = _Y1*_Y1/(_Y1*_Y1+_X1*_X1);
	else _coeff = 1 - _X1*_X1/(_Y1*_Y1+_X1*_X1);

	if (std::isnan(_coeff))
	{
		QPolygonF _polygon;
		_polygon
			<<QPointF(1, 1)
			<<QPointF(1, 1)
			<<QPointF(1, 1)
			<<QPointF(1, 1);

		QPainterPath l_oPath;
		l_oPath.addPolygon(_polygon);

		setPath(l_oPath);
		return;
	}

	double _deltax = l_oDist * sqrt( _coeff );
	double _deltay = l_oDist * sqrt( 1 - _coeff );

	if (_X1 != 0) if (_Y1/_X1 < 0) _deltay *= -1;

	double _s = m_oFrom->m_oGraph->m_oMediator->m_dTriSize;

	QPolygonF _polygon;
	_polygon
		<<QPointF(_x1 + _s*_deltax, _y1 - _s*_deltay)
		<<QPointF(_x2 + _deltax, _y2 - _deltay)
		<<QPointF(_x2 - _deltax, _y2 + _deltay)
		<<QPointF(_x1 - _s*_deltax, _y1 + _s*_deltay);

	//qDebug()<<_polygon;

	QPainterPath l_oPath;
	l_oPath.addPolygon(_polygon);

	setPath(l_oPath);
}

#define _C_ 4.5

void canvas_link::update_spline()
{
	QRectF l_oR1 = m_oFrom->boundingRect();
	QRectF l_oR2 = m_oTo->boundingRect();

	double _x1 = m_oFrom->x() + l_oR1.width()/2. - OFF;
	double _y1 = m_oFrom->y() + l_oR1.height()/2. - OFF;

	double _x2 = m_oTo->x() + l_oR2.width()/2. - OFF;
	double _y2 = m_oTo->y() + l_oR2.height()/2. - OFF;

	double _X1 = _x2 - _x1;
	double _Y1 = _y2 - _y1;

	double l_oDist = 1;
	double _coeff = 0;
	if (_Y1*_Y1>_X1*_X1) _coeff = _Y1*_Y1/(_Y1*_Y1+_X1*_X1);
	else _coeff = 1 - _X1*_X1/(_Y1*_Y1+_X1*_X1);

	if (std::isnan(_coeff))
	{
		QPolygonF _polygon;
		_polygon
			<<QPointF(1, 1)
			<<QPointF(1, 1)
			<<QPointF(1, 1)
			<<QPointF(1, 1);

		QPainterPath l_oPath;
		l_oPath.addPolygon(_polygon);

		setPath(l_oPath);
		return;
	}

	double _deltax = l_oDist * sqrt( _coeff );
	double _deltay = l_oDist * sqrt( 1 - _coeff );

	if (_X1 != 0) if (_Y1/_X1 < 0) _deltay *= -1;


	QPainterPath l_oPath;
	l_oPath.moveTo(_x1 + _C_ * _deltax, _y1 - _C_ * _deltay);

	if (_x2 < _x1)
	{
		l_oPath.quadTo(_x2 + l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x2 + l_oR2.width()/2. - 3, _y2 + l_oR2.height()/2.);
		l_oPath.quadTo(_x2 + l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x1 - _C_ * _deltax, _y1 + _C_ * _deltay);
	}
	else
	{
		l_oPath.quadTo(_x2 - l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x2 - l_oR2.width()/2. + 3, _y2 + l_oR2.height()/2.);
		l_oPath.quadTo(_x2 - l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x1 - _C_ * _deltax, _y1 + _C_ * _deltay);
	}

	setPath(l_oPath);
}

void canvas_link::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *i_oStyle, QWidget *i_oWidget=NULL)
{
	QColor l_o = get_color();
	setPen(l_o);
	QColor l_oBack;
	if (m_oTo != NULL)
	{
		data_item& l_oItem = m_oGraph->m_oMediator->m_oItems[m_oTo->m_iId];
		color_scheme l_oColorScheme = l_oItem.get_color_scheme(m_oGraph->m_oMediator);
		l_oBack = l_oColorScheme.m_oInnerColor;
	}
	else
	{
		l_oBack = QColor(Qt::green);
	}

	double l_oA, l_oB;
	if (m_oFrom != NULL)
	{
		int l_oHeight = m_oGraph->m_oMediator->height_of(m_oFrom->m_iId);
		l_oA = qMin(0.2 * l_oHeight, 1.0);
		l_oB = qMin(0.2 * (l_oHeight + 1), 1.0);
	}

	double l_oNa = 1 - l_oA;
	int l_oRed1 = l_o.red() * l_oNa + (l_oBack.red()) * l_oA;
	int l_oGreen1 = l_o.green() * l_oNa + (l_oBack.green()) * l_oA;
	int l_oBlue1 = l_o.blue() * l_oNa + (l_oBack.blue()) * l_oA;
	QColor l_oV1 = QColor::fromRgb(l_oRed1, l_oGreen1, l_oBlue1);

        if (m_oGraph->m_bDisableGradient)
        {
                setBrush(l_oV1);
        }
        else
	{
		double l_oNb = 1 - l_oB;
		int l_oRed2 = l_o.red() * l_oNb + (l_oBack.red()) * l_oB;
		int l_oGreen2 = l_o.green() * l_oNb + (l_oBack.green()) * l_oB;
		int l_oBlue2 = l_o.blue() * l_oNb + (l_oBack.blue()) * l_oB;
		QColor l_oV2 = QColor::fromRgb(l_oRed2, l_oGreen2, l_oBlue2);

		const QRectF l_oR1 = m_oFrom->boundingRect();
		const QRectF l_oR2 = m_oTo->boundingRect();

		double _x1 = m_oFrom->x() + l_oR1.width()/2. - OFF;
		double _y1 = m_oFrom->y() + l_oR1.height()/2. - OFF;
		double _x2 = m_oTo->x() + l_oR2.width()/2. - OFF;
		double _y2 = m_oTo->y() + l_oR2.height()/2. - OFF;

		QLinearGradient l_oGradient(_x1, _y1, _x2, _y2);
		l_oGradient.setColorAt(0.0, l_oV1);
		l_oGradient.setColorAt(1.0, l_oV2);
		setBrush(l_oGradient);
	}

	QGraphicsPathItem::paint(i_oPainter, i_oStyle, i_oWidget);
}

QColor canvas_link::get_color()
{
return m_oGraph->m_oMediator->m_oArrowColor.lighter(250);
}

