// Thomas Nagy 2007-2018 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QLinearGradient>
#include <QTextDocument>
#include "box_component.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#define MIN_FORK_SIZE 30

box_component::box_component(box_view* view, int id) : box_item(view, id)
{
	y_text_off = 2 * OFF + 20;
	x_text_off = 2 * OFF + 10;
	setZValue(95);
}

void box_component::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	doc.setDefaultFont(scene()->font());

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);

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

	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}

	painter->setPen(l_oPen);
	painter->drawRect(l_oRect);

	int w = rect().width();
	l_oPen.setStyle(Qt::SolidLine);
	painter->setPen(l_oPen);
	painter->setBrush(bc);
	QRectF l_o(w - 20, 6, 12, 14);
	painter->drawRect(l_o);
	QRectF l_o2(w - 24, 9, 8, 3);
	painter->drawRect(l_o2);
	QRectF l_o3(w - 24, 14, 8, 3);
	painter->drawRect(l_o3);


	painter->translate(OFF, OFF + 15);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes
	doc.documentLayout()->draw(painter, ctx);
}

