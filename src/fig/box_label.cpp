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
#include <QTextDocument>
#include "box_label.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_label::box_label(box_view* view, int id) : box_item(view, id)
{
	doc.setDefaultFont(scene()->font());
}

void box_label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	doc.setDefaultFont(scene()->font());

	qreal pad = 0.5;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);

	if (isSelected())
	{
		QPen l_oPen = QPen(Qt::DotLine);
		l_oPen.setColor(Qt::black);
		l_oPen.setCosmetic(false);
		l_oPen.setWidth(1);
		painter->setPen(l_oPen);
		painter->drawRoundRect(l_oRect, 20, 20);
	}

	painter->translate(OFF, OFF);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, m_oBox->color);
	doc.documentLayout()->draw(painter, ctx);
}

/*QVariant box_label::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionHasChanged)
		{
			update_links();
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			m_oChain->setVisible(isSelected());
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}*/


