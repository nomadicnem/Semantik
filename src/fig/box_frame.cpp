// Thomas Nagy 2007-2017 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
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

#define MIN_FORK_SIZE 30

box_frame::box_frame(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setPointSize(font.pointSize() - 2);
	doc.setDefaultFont(font);

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
	l_oPen.setWidth(1);
	painter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);

	painter->drawRect(l_oRect);
}

void box_frame::update_links() {
	m_oCaption->setPlainText(m_oBox->m_sText);
	m_oCaption->setPos((m_iWW - m_oCaption->boundingRect().width()) / 2., 0);
	//m_oCaption->setPos((m_iWW - m_oCaption->boundingRect().width()) / 2., m_iHH - m_oCaption->boundingRect().height());
	box_item::update_links();
}

