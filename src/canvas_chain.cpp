// Thomas Nagy 2007-2018 GPLV3

#include <QApplication>
#include <QPainter>
#include <QtDebug>
#include "CON.h"
#include "canvas_chain.h"
#include "box_view.h"
#include "data_item.h"
#include <box_control_point.h>

#define PAD 1

canvas_chain::canvas_chain(canvas_view* i_oParent) : QGraphicsRectItem()
{
	m_oParent = i_oParent;
	setVisible(false);
	setRect(0, 0, 10, 10);
}

void canvas_chain::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
        QPen l_oPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setWidthF(0.01 + 1);
	i_oPainter->setPen(l_oPen);

	i_oPainter->drawLine(QPoint(PAD, 5), QPoint(10-PAD, 5));
	i_oPainter->drawLine(QPoint(5, PAD), QPoint(5, 10-PAD));
}

