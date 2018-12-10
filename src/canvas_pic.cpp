// Thomas Nagy 2007-2018 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QClipboard>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include <QTextCursor>
#include <QPen>
#include <QtDebug>
#include <QKeyEvent>
#include <QTextList>
#include <QTextBlock>
#include <QSvgRenderer>
#include <QTextDocumentFragment>

#include "canvas_chain.h"
#include "canvas_flag.h"
#include "color_scheme.h"
#include "sem_mediator.h"
 #include "con.h" 
#include "data_item.h"
#include "canvas_pic.h"
#include "canvas_view.h"

#define pad 2

canvas_pic::canvas_pic(canvas_view *i_oGraphWidget, int i_iId) : QGraphicsRectItem(), m_oGraph(i_oGraphWidget)
{
	m_iId = i_iId;
	setZValue(99);
	setVisible(false);
	i_oGraphWidget->scene()->addItem(this);
}

void canvas_pic::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	data_item& l_oData = m_oGraph->m_oMediator->m_oItems[m_iId];
	if (l_oData.m_iDataType == VIEW_IMG)
	{
		const QPixmap& l_oPix = l_oData.getPix(m_oGraph->m_oMediator);
		if (!l_oPix.isNull())
		{
			QRectF l_o = boundingRect().adjusted(pad, pad, -pad, -pad);
			QRect l_oPixRect = l_oPix.rect();
			if (l_oPixRect.width() > l_oPixRect.height())
			{
				l_o.setHeight(l_o.height() * (1. * l_oPixRect.height() / l_oPixRect.width()));
			}
			else
			{
				l_o.setWidth(l_o.width() * (1. * l_oPixRect.width() / l_oPixRect.height()));
			}

			i_oPainter->drawPixmap(l_o, l_oPix, l_oPixRect);
		}
	}





}

