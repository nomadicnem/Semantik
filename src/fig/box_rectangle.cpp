// Thomas Nagy 2017-2018 GPLV3

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
#include "box_rectangle.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#define MIN_FORK_SIZE 30

box_rectangle::box_rectangle(box_view* view, int id) : box_item(view, id)
{
	setZValue(95);
}

void box_rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	doc.setDefaultFont(scene()->font());
	//painter->setFont(scene()->font());

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);


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

	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}

	painter->setPen(l_oPen);
	painter->drawRect(l_oRect);

	painter->translate(OFF, OFF + 15);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes
	doc.documentLayout()->draw(painter, ctx);
}

