// Thomas Nagy 2007-2019 GPLV3

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
#include "box_view.h"
#include "canvas_pic.h"
#include "canvas_view.h"

#define pad 2

canvas_pic::canvas_pic(canvas_view *i_oGraphWidget, int i_iId) : QGraphicsRectItem(), m_oGraph(i_oGraphWidget)
{
	m_oBoxView = NULL;
	m_iId = i_iId;
	setZValue(98);
	setFlags(QGraphicsItem::ItemStacksBehindParent);
	i_oGraphWidget->scene()->addItem(this);
	setVisible(false);
	m_iOffset = pad;
}

canvas_pic::~canvas_pic()
{
	delete m_oBoxView;
	//m_oBoxView = NULL;
}

void canvas_pic::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QRectF l_o = boundingRect().adjusted(pad, pad, -pad, -pad);
	data_item& l_oData = m_oGraph->m_oMediator->m_oItems[m_iId];

	if (l_oData.m_iDataType == VIEW_IMG)
	{
		const QPixmap& l_oPix = l_oData.getPix(m_oGraph->m_oMediator);
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
		m_iOffset = l_o.height() + pad;
	}
	else if (l_oData.m_iDataType == VIEW_DIAG)
	{
		if (m_oBoxView == NULL)
		{
			m_oBoxView = new box_view(m_oGraph, m_oGraph->m_oMediator);
		}
		QRectF l_oDrawn = m_oBoxView->drawThumb(i_oPainter, l_o, m_iId);
		m_iOffset = l_oDrawn.height() + pad;
	}
}

