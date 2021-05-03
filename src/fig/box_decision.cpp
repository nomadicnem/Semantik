// Thomas Nagy 2007-2021 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include <QLinearGradient>
#include "box_decision.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_decision::box_decision(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setBold(true);
	doc.setDefaultFont(font);

	setZValue(90);
}

void box_decision::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}
	painter->setPen(l_oPen);

	qreal pad = l_oPen.widthF() / 2.;
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

	QPointF pts[4]; // NESW
	QPointF topLeft = l_oRect.topLeft();
	pts[0] = topLeft + QPointF(l_oRect.width()/2, 0);
	pts[1] = topLeft + QPointF(l_oRect.width(),   l_oRect.height()/2);
	pts[2] = topLeft + QPointF(l_oRect.width()/2, l_oRect.height());
	pts[3] = topLeft + QPointF(0,                 l_oRect.height()/2);

	painter->drawPolygon(pts, 4);
	painter->setClipRect(l_oRect);

	qreal l_fWidth = l_oRect.width();
	qreal l_fHeight = l_oRect.height();

	QTextLayout l_oTextLayout(doc.toPlainText(), scene()->font());
	QTextOption l_oOption = doc.defaultTextOption();
	l_oOption.setAlignment(m_oBox->m_iAlign);
	l_oTextLayout.setTextOption(l_oOption);
	l_oTextLayout.beginLayout();

	QFontMetricsF l_oFontMetrics(scene()->font());
	qreal l_fLineHeight = l_oFontMetrics.height();

	qreal l_fMinWidth = l_oFontMetrics.maxWidth()/2. + OFF;
	qreal l_fMinHeight = l_fMinWidth * l_fHeight / l_fWidth;

	qreal l_fMaxHeight = l_fHeight - 2 * l_fMinHeight;
	int l_iMaxLines = l_fMaxHeight / l_fLineHeight;

	int l_iNumLines;
	QString l_sText = doc.toPlainText();

	if (l_sText == m_sLastText && m_fLastLineHeight == l_fLineHeight && l_fWidth == m_fLastWidth && l_fHeight == m_fLastHeight)
	{
                l_iNumLines = m_iLastLineCount;
	}
	else
	{
		int l_iBestCount = linesCount(l_sText, l_iMaxLines, l_fWidth, l_fHeight);
		int l_iBestIndex = l_iMaxLines;

		for (int i=l_iMaxLines; i > 0; i--)
		{
			int l_iNumLineTry = linesCount(l_sText, i, l_fWidth, l_fHeight);
			if (l_iNumLineTry <= l_iBestCount && l_iNumLineTry <= i)
			{
				l_iBestCount = l_iNumLineTry;
				l_iBestIndex = i;
			}
			else if (l_iNumLineTry > i)
			{
				break;
			}
		}

		m_sLastText = l_sText;
		m_fLastLineHeight = l_fLineHeight;
		m_fLastHeight = l_fHeight;
		m_fLastWidth = l_fWidth;
		m_iLastLineCount = l_iNumLines = l_iBestIndex;
	}

	qreal l_iY = (l_fHeight - l_iNumLines * l_fLineHeight) / 2.;

	painter->setPen(QApplication::palette("QTextControl").color(QPalette::Text));
	qreal l_fRatio = l_fWidth / l_fHeight;
	while (true)
	{
		QTextLine l_oLine = l_oTextLayout.createLine();
		if (!l_oLine.isValid())
		{
			break;
		}

		qreal l_iLineWidth;
		if (l_iY + l_fLineHeight >= l_fHeight)
		{
			l_iLineWidth = 1.;
		}
		else
		{
			qreal l_iA = qMax(0.0, l_iY * l_fRatio);
			qreal l_iB = qMax(0.0, (l_iY + l_fLineHeight) * l_fRatio);

			qreal l_iC = qMax(0.0, (l_fHeight - l_iY) * l_fRatio);
			qreal l_iD = qMax(0.0, (l_fHeight - l_iY - l_fLineHeight) * l_fRatio);

			qreal l_iX1 = qMin(l_iA, l_iB);
			qreal l_iX2 = qMin(l_iC, l_iD);

			l_iLineWidth = qMax(qMin(l_iX1, l_iX2) * 2 - 2 * OFF, 1.);
		}
		qreal l_iX = l_fWidth/2. - l_iLineWidth / 2.;

		l_oLine.setLineWidth(l_iLineWidth);
		l_oLine.setPosition(QPointF(l_iX, l_iY));

		l_iY += l_oLine.height();

	}
	l_oTextLayout.endLayout();
	l_oTextLayout.draw(painter, QPoint(0, 0));
}

QSize box_decision::best_size(const QPointF &dims)
{
	int x = dims.x();
	x = GRID * (x / GRID);
	if (x < GRID) x = GRID;

	int y = dims.y();
	y = GRID * (y / GRID);
	if (y < GRID) y = GRID;

	QString l_sText = doc.toPlainText();
	if (l_sText.isEmpty())
	{
		return QSize(x, y);
	}

	do {
		qreal l_fWidth = x - (1 + 0.01);
		qreal l_fHeight = y - (1 + 0.01);

		QFontMetricsF l_oFontMetrics(scene()->font());
		qreal l_fLineHeight = l_oFontMetrics.height();


		qreal l_fMinWidth = l_oFontMetrics.maxWidth()/2. + OFF;
		qreal l_fMinHeight = l_fMinWidth * l_fHeight / l_fWidth;

		qreal l_fMaxHeight = l_fHeight - 2 * l_fMinHeight;
		int l_iMaxLines = l_fMaxHeight / l_fLineHeight;
		if (l_iMaxLines < 1)
		{
			x += GRID;
			y += GRID;
			continue;
		}


		int l_iNumLines = linesCount(l_sText, l_iMaxLines, l_fWidth, l_fHeight);

		if (l_iNumLines > l_iMaxLines)
		{
			y += GRID;
			x += GRID;
		}
		else
		{
			break;
		}
	}
	while (true);

	return QSize(x, y);
}

int box_decision::linesCount(const QString& i_sInput, int i_iNumLines, qreal i_fWidth, qreal i_fHeight)
{
	if (i_iNumLines <= 0) return 0;
	doc.setDefaultFont(scene()->font());
	QTextOption l_oOption = doc.defaultTextOption();
	l_oOption.setAlignment(m_oBox->m_iAlign);
	doc.setDefaultTextOption(l_oOption);

	QFontMetricsF l_oFontMetrics(scene()->font());
	qreal l_fLineHeight = l_oFontMetrics.height();

	QTextLayout l_oTextLayout(i_sInput, scene()->font());
	l_oTextLayout.setTextOption(l_oOption);
	l_oTextLayout.beginLayout();

	qreal l_iY = (i_fHeight - i_iNumLines * l_fLineHeight) / 2.;
	qreal l_fRatio = i_fWidth / i_fHeight;
	while (true)
	{
		QTextLine l_oLine = l_oTextLayout.createLine();
		if (!l_oLine.isValid())
		{
			break;
		}

		qreal l_iLineWidth;

		if (l_iY + l_fLineHeight >= i_fHeight)
		{
			l_iLineWidth = 1.;
		}
		else
		{
			qreal l_iA = qMax(0.0, l_iY * l_fRatio);
			qreal l_iB = qMax(0.0, (l_iY + l_fLineHeight) * l_fRatio);

			qreal l_iC = qMax(0.0, (i_fHeight - l_iY) * l_fRatio);
			qreal l_iD = qMax(0.0, (i_fHeight - l_iY - l_fLineHeight) * l_fRatio);

			qreal l_iX1 = qMin(l_iA, l_iB);
			qreal l_iX2 = qMin(l_iC, l_iD);

			l_iLineWidth = qMax(qMin(l_iX1, l_iX2) * 2 - 2 * OFF, 1.);
		}
		qreal l_iX = i_fWidth/2. - l_iLineWidth / 2.;

		l_oLine.setLineWidth(l_iLineWidth);
		l_oLine.setPosition(QPointF(l_iX, l_iY));

		l_iY += l_oLine.height();
	}
	l_oTextLayout.endLayout();
	return l_oTextLayout.lineCount();
}

QSize box_decision::best_size_for(const QString &i_sText)
{
	if (i_sText.isEmpty())
	{
		return QSize(m_oBox->m_iWW, m_oBox->m_iHH);
	}

	int x = m_oBox->m_iWW;
	x = GRID * (x / GRID);
	if (x < GRID) x = GRID;

	int y = m_oBox->m_iHH;
	y = GRID * (y / GRID);
	if (y < GRID) y = GRID;

	do {
		qreal l_fWidth = x - (1 + 0.01);
		qreal l_fHeight = y - (1 + 0.01);

		QFontMetricsF l_oFontMetrics(scene()->font());
		qreal l_fLineHeight = l_oFontMetrics.height();

		qreal l_fMinWidth = l_oFontMetrics.maxWidth()/2. + OFF;
		qreal l_fMinHeight = l_fMinWidth * l_fHeight / l_fWidth;

		qreal l_fMaxHeight = l_fHeight - 2 * l_fMinHeight;
		int l_iMaxLines = l_fMaxHeight / l_fLineHeight;
		if (l_iMaxLines < 1)
		{
			x += GRID;
			continue;
		}

		int l_iNumLines = linesCount(i_sText, l_iMaxLines, l_fWidth, l_fHeight);

		if (l_iNumLines > l_iMaxLines)
		{
			x += GRID;
			y += GRID;
		}
		else
		{
			break;
		}
	}
	while (true);

	return QSize(x, y);
}


