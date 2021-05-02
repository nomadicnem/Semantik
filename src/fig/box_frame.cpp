// Thomas Nagy 2007-2021 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
 #include <box_frame_properties.h>
   #include <QComboBox>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include "box_frame.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_frame::box_frame(box_view* view, int id) : box_item(view, id)
{
	setZValue(80);
}

void box_frame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	doc.setDefaultFont(scene()->font());
	QTextOption l_oOption = doc.defaultTextOption();
	l_oOption.setAlignment(m_oBox->m_iAlign);
	doc.setDefaultTextOption(l_oOption);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setWidthF(0.01 + 1);
	painter->setPen(l_oPen);


	qreal pad = l_oPen.widthF() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);
	painter->setClipRect(rect());

	painter->drawRect(l_oRect);

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes
	QAbstractTextDocumentLayout * lay = doc.documentLayout();

	qreal l_fYpos;
	if (m_oBox->m_iLabelPosition == Qt::TopEdge)
	{
		l_fYpos = OFF;
	}
	else
	{
		l_fYpos = l_oRect.height() - OFF - lay->documentSize().height();;
	}

	painter->translate(OFF, l_fYpos);
	lay->draw(painter, ctx);
}

void box_frame::properties()
{
	box_frame_properties props(m_oView, this);
	props.exec();
}

