// Thomas Nagy 2007-2019 GPLV3

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
	m_oCaption = new QGraphicsTextItem();
	m_oCaption->setParentItem(this);
	m_oCaption->setPos(0, 0);

	setZValue(80);
}

void box_frame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setWidthF(0.01 + 1);
	painter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);

	painter->drawRect(l_oRect);
}

void box_frame::update_links()
{
	m_oCaption->setFont(scene()->font());
	m_oCaption->setPlainText(m_oBox->m_sText);

	QRectF l_oR = boundingRect();
	int l_iVpos;
	if (m_oBox->m_iLabelPosition == Qt::TopEdge)
	{
		l_iVpos = 0;
	}
	else
	{
		l_iVpos = l_oR.height() - m_oCaption->boundingRect().height();
	}

	int l_iHpos;
	if (m_oBox->m_iAlign == Qt::AlignLeft)
	{
		l_iHpos = l_oR.left() + PAD;
	}
	else if (m_oBox->m_iAlign == Qt::AlignRight)
	{
		l_iHpos = l_oR.right() - m_oCaption->boundingRect().width() - PAD;
	}
	else
	{
		l_iHpos = (l_oR.width() - m_oCaption->boundingRect().width()) / 2.;
	}

	m_oCaption->setPos(l_iHpos, l_iVpos);
	box_item::update_links();
}

void box_frame::update_data()
{
	box_item::update_data();
	update_links();
}

void box_frame::properties()
{
	box_frame_properties props(m_oView, this);
	props.exec();
}

