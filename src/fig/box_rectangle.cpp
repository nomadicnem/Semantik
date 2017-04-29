// Thomas Nagy 2017-2017 GPLV3

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

#define PAD 2
#define MIN_FORK_SIZE 30

box_rectangle::box_rectangle(box_view* view, int id) : box_item(view, id)
{
	setZValue(95);
}

void box_rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	doc.setDefaultFont(scene()->font());
	//painter->setFont(scene()->font());

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
	painter->drawRect(l_oRect);

	if (isSelected())
	{
		l_oPen.setStyle(Qt::SolidLine);
		painter->setPen(l_oPen);
		painter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(l_oRect.bottomRight() - QPointF(6, 6), l_oRect.bottomRight());
		painter->drawRect(l_oR2);
	}

	painter->translate(OFF, OFF + 15);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes
	doc.documentLayout()->draw(painter, ctx);

	painter->restore();
}

