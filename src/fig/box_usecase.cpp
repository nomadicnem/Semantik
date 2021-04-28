// Thomas Nagy 2007-2020 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include "QTextLayout"
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include "box_usecase.h"
#include "data_item.h"
 #include "res:zable.h"
#include <QRegion>
#include "box_view.h"
#include "math.h"
#include "math.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_usecase::box_usecase(box_view* view, int id) : box_item(view, id)
{
}

void box_usecase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	doc.setDefaultFont(scene()->font());
	QTextOption l_oOption = doc.defaultTextOption();
	l_oOption.setAlignment(m_oBox->m_iAlign);
	doc.setDefaultTextOption(l_oOption);


	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	painter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);


	QColor bc = m_oBox->getColor(m_oView->m_oMediator);
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

	painter->drawEllipse(l_oRect);

	QRegion l_oRegion(l_oRect.toAlignedRect(), QRegion::Ellipse);
	painter->setClipRegion(l_oRegion);

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes


	int l_iWidth = l_oRect.width() - 2 * OFF;
	int l_iHeight = l_oRect.height();
	qreal l_iY = 2 * OFF;

	qreal l_fW2 = pow(l_iWidth, 2);
	qreal l_fH2 = pow(l_iHeight, 2);

	QTextLayout l_oTextLayout(doc.toPlainText(), scene()->font());
	l_oTextLayout.setTextOption(l_oOption);
	l_oTextLayout.beginLayout();

	qreal l_iLineHeight = QFontMetrics(scene()->font()).height();

	while (true)
	{
		QTextLine l_oLine = l_oTextLayout.createLine();
		if (!l_oLine.isValid())
		{
			break;
		}

		qreal l_iX1 = qMax(0.0, pow((1 - 4 * pow(l_iY - l_iHeight/2, 2) / l_fH2) * (l_fW2 / 4), .5));
		qreal l_iX2 = qMax(0.0, pow((1 - 4 * pow(l_iY + l_iLineHeight - l_iHeight/2, 2) / l_fH2) * (l_fW2 / 4), .5));

		qreal l_iLineWidth = qMin(l_iX1, l_iX2) * 2;
		qreal l_iX = l_iWidth/2. - l_iLineWidth / 2.;

		l_oLine.setLineWidth(l_iLineWidth);
		l_oLine.setPosition(QPointF(l_iX + OFF, l_iY));
		l_iY += l_oLine.height();
	}
	l_oTextLayout.endLayout();
	l_oTextLayout.draw(painter, QPoint(0, 0));
}
