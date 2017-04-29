// Thomas Nagy 2017-2017 GPLV3

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
#include "box_pipe.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_pipe::box_pipe(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setPointSize(font.pointSize() - 2);
	doc.setDefaultFont(font);
}

void box_pipe::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setPen(l_oPen);
	if (isSelected()) {
		l_oPen.setStyle(Qt::DotLine);
	}
	painter->setPen(l_oPen);

	QRectF l_oRect2(l_oRect);
	qreal l_fRat = l_oRect2.height()/2.2;
	if (l_fRat > l_oRect.width() - 5)
	{
		l_fRat = l_oRect.width() - 5;
	}
	l_oRect2.setWidth(l_fRat);
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

	QRectF l_oRect3 = l_oRect2.translated(l_oRect.width() - l_oRect2.width(), 0);

	qreal l_fX1 = l_oRect2.topLeft().y() + l_oRect2.width()/2.;
	//qreal l_fY2 = l_oRect3.topLeft().y() + l_oRect3.height()/2.;

	qreal l_fY1 = l_oRect.topLeft().y();
	//qreal l_fX2 = l_oRect.topRight().x();

	QPainterPath p;
	p.addRect(l_fX1, l_fY1, l_oRect3.topLeft().x() - l_oRect2.topLeft().x() + l_oRect2.width()/2. - l_oRect3.width()/2., l_oRect.height());

	QPainterPath p2;
	p2.addEllipse(l_oRect3);
	p |= p2;
	painter->drawPath(p);


	painter->setBrush(bc);
	painter->drawEllipse(l_oRect2);


	if (isSelected())
	{
		l_oPen.setStyle(Qt::SolidLine);
		painter->setPen(l_oPen);
		painter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(l_oRect.bottomRight() - QPointF(6, 6), l_oRect.bottomRight());
		painter->drawRect(l_oR2);
	}

	painter->restore();
}

