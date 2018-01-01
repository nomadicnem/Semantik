// Thomas Nagy 2007-2018 GPLV3

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
#include <QPainterPath>
#include "box_database.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_database::~box_database()
{
	delete m_oCaption;
}

box_database::box_database(box_view* view, int id) : box_item(view, id)
{
	m_oCaption = new QGraphicsTextItem();
	m_oCaption->setParentItem(this);
	m_oCaption->setPos(0, 0);
}

void box_database::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setPen(l_oPen);
	if (isSelected()) {
		l_oPen.setStyle(Qt::DotLine);
	}
	painter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);

	QRectF l_oRect2(l_oRect);
	qreal l_fRat = l_oRect2.width()/2.2;
	if (l_fRat > l_oRect.height() - 5)
	{
		l_fRat = l_oRect.height() - 5;
	}
	l_oRect2.setHeight(l_fRat);
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

	QRectF l_oRect3 = l_oRect2.translated(0, l_oRect.height() - l_oRect2.height());

	qreal l_fY1 = l_oRect2.topLeft().y() + l_oRect2.height()/2.;
	//qreal l_fY2 = l_oRect3.topLeft().y() + l_oRect3.height()/2.;

	qreal l_fX1 = l_oRect.topLeft().x();
	//qreal l_fX2 = l_oRect.topRight().x();

	QPainterPath p;
	p.addRect(l_fX1, l_fY1, l_oRect.width(), l_oRect3.topLeft().y() - l_oRect2.topLeft().y() + l_oRect2.height()/2. - l_oRect3.height()/2.);

	QPainterPath p2;
	p2.addEllipse(l_oRect3);
	p |= p2;
	painter->drawPath(p);


	painter->setBrush(bc);
	painter->drawEllipse(l_oRect2);
}

void box_database::update_links() {
	QRectF r = boundingRect();
	m_oCaption->setFont(scene()->font());
	m_oCaption->setPlainText(m_oBox->m_sText);
	m_oCaption->setPos((r.width() - m_oCaption->boundingRect().width()) / 2., r.height());
	box_item::update_links();
}

