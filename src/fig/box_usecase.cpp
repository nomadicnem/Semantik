// Thomas Nagy 2007-2021 GPLV3

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

int box_usecase::linesCount(const QString& i_sInput, int i_iNumLines, qreal i_fWidth, qreal i_fHeight)
{
	doc.setDefaultFont(scene()->font());
	QTextOption l_oOption = doc.defaultTextOption();
	l_oOption.setAlignment(m_oBox->m_iAlign);
	doc.setDefaultTextOption(l_oOption);

	QFontMetricsF l_oFontMetrics(scene()->font());
	qreal l_fLineHeight = l_oFontMetrics.height();

	qreal l_fW2 = pow(i_fWidth, 2);
	qreal l_fH2 = pow(i_fHeight, 2);

	QTextLayout l_oTextLayout(doc.toPlainText(), scene()->font());
	l_oTextLayout.setTextOption(l_oOption);
	l_oTextLayout.beginLayout();

	qreal l_iY = (i_fHeight - i_iNumLines * l_fLineHeight) / 2.;

	while (true)
	{
		QTextLine l_oLine = l_oTextLayout.createLine();
		if (!l_oLine.isValid())
		{
			break;
		}

		qreal l_iX1 = qMax(0.0, pow((1 - 4 * pow(l_iY - i_fHeight/2, 2) / l_fH2) * (l_fW2 / 4), .5));
		qreal l_iX2 = qMax(0.0, pow((1 - 4 * pow(l_iY + l_fLineHeight - i_fHeight/2, 2) / l_fH2) * (l_fW2 / 4), .5));

		qreal l_iLineWidth = qMin(l_iX1, l_iX2) * 2 - 2 * OFF;
		qreal l_iX = i_fWidth/2. - l_iLineWidth / 2.;

		l_oLine.setLineWidth(l_iLineWidth);
		l_oLine.setPosition(QPointF(l_iX, l_iY));
		l_iY += l_oLine.height();
	}
	l_oTextLayout.endLayout();
	return l_oTextLayout.lineCount();
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


	qreal l_fWidth = l_oRect.width();
	qreal l_fHeight = l_oRect.height();

	qreal l_fW2 = pow(l_fWidth, 2);
	qreal l_fH2 = pow(l_fHeight, 2);

	QTextLayout l_oTextLayout(doc.toPlainText(), scene()->font());
	l_oTextLayout.setTextOption(l_oOption);
	l_oTextLayout.beginLayout();

	QFontMetricsF l_oFontMetrics(scene()->font());
	qreal l_fLineHeight = l_oFontMetrics.height();

	qreal l_fMinWidth = l_oFontMetrics.maxWidth() + 2 * OFF;
	qreal l_fMinHeight = (l_fHeight/2.) * (1 - pow(1 - (l_fMinWidth * l_fMinWidth) / l_fW2, 0.5));

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
		l_iNumLines = linesCount(l_sText, l_iMaxLines, l_fWidth, l_fHeight);
		while (true)
		{
			int l_iNumLineTry = linesCount(l_sText, l_iNumLines - 1, l_fWidth, l_fHeight);
			if (l_iNumLineTry < l_iNumLines)
			{
				l_iNumLines -= 1;
			}
			else
			{
				break;
			}
		}
		m_sLastText = l_sText;
		m_fLastLineHeight = l_fLineHeight;
		m_fLastHeight = l_fHeight;
		m_fLastWidth = l_fWidth;
		m_iLastLineCount = l_iNumLines;
	}


	qreal l_iY = (l_fHeight - l_iNumLines * l_fLineHeight) / 2.;

	painter->setPen(QApplication::palette("QTextControl").color(QPalette::Text));
	while (true)
	{
		QTextLine l_oLine = l_oTextLayout.createLine();
		if (!l_oLine.isValid())
		{
			break;
		}

		qreal l_iX1 = qMax(0.0, pow((1 - 4 * pow(l_iY - l_fHeight/2, 2) / l_fH2) * (l_fW2 / 4), .5));
		qreal l_iX2 = qMax(0.0, pow((1 - 4 * pow(l_iY + l_fLineHeight - l_fHeight/2, 2) / l_fH2) * (l_fW2 / 4), .5));

		qreal l_iLineWidth = qMin(l_iX1, l_iX2) * 2 - 2 * OFF;
		qreal l_iX = l_fWidth/2. - l_iLineWidth / 2.;

		l_oLine.setLineWidth(l_iLineWidth);
		l_oLine.setPosition(QPointF(l_iX, l_iY));

		l_iY += l_oLine.height();

	}
	l_oTextLayout.endLayout();
	l_oTextLayout.draw(painter, QPoint(0, 0));
}

QSize box_usecase::best_size(const QPointF &dims)
{
	if (doc.toPlainText().isEmpty())
	{
		return QSize(qMax(fceil(dims.x(), GRID), GRID), qMax(fceil(dims.y(), GRID), GRID));
	}

	int x = dims.x();
	x = GRID * (x / GRID);
	if (x < 3 * GRID) x = 3 * GRID;

	int y = dims.y();
	y = GRID * (y / GRID);
	if (y < GRID) y = GRID;

	QString l_sText = doc.toPlainText();
	do {

		qreal l_fWidth = x - (1 + 0.01);
		qreal l_fHeight = y - (1 + 0.01);

		qreal l_fW2 = pow(l_fWidth, 2);

		QFontMetricsF l_oFontMetrics(scene()->font());
		qreal l_fLineHeight = l_oFontMetrics.height();

		qreal l_fMinWidth = l_oFontMetrics.maxWidth() + 2 * OFF;
		qreal l_fMinHeight = (l_fHeight/2.) * (1 - pow(1 - (l_fMinWidth * l_fMinWidth) / l_fW2, 0.5));

		qreal l_fMaxHeight = l_fHeight - 2 * l_fMinHeight;
		int l_iMaxLines = l_fMaxHeight / l_fLineHeight;

		int l_iNumLines = linesCount(l_sText, l_iMaxLines, l_fWidth, l_fHeight);

		if (l_iNumLines > l_iMaxLines)
		{
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


