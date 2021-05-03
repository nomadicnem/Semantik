// Thomas Nagy 2007-2021 GPLV3

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
#include <QLinearGradient>
#include <QTextDocument>
#include "box_component.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#define MIN_FORK_SIZE 30

box_component::box_component(box_view* view, int id) : box_item(view, id)
{
	y_text_off = 2 * OFF + 20;
	x_text_off = 2 * OFF;
	setZValue(95);
}

QSize box_component::best_size(const QPointF &dims)
{
	int x = dims.x();
	x = GRID * (x / GRID);
	if (x < 3 * GRID) x = 3 * GRID;

	int y = dims.y();
	y = GRID * (y / GRID);
	if (y < 3 * GRID) y = 3 * GRID;

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

		qreal l_fMaxHeight = l_fHeight - 2 * OFF;
		int l_iMaxLines = l_fMaxHeight / l_fLineHeight;
		if (l_iMaxLines < 1)
		{
			x += GRID;
			y += GRID;
			continue;
		}

		int l_iNumLines = linesCount(l_sText, l_iMaxLines, l_fWidth, l_fHeight);

		if (l_iNumLines < 1)
		{
			x += GRID;
			continue;
		}

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

void box_component::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);

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

	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}

	painter->setPen(l_oPen);
	painter->drawRect(l_oRect);

	int w = rect().width();
	l_oPen.setStyle(Qt::SolidLine);
	painter->setPen(l_oPen);
	painter->setBrush(bc);
	QRectF l_o(w - 20, 6, 12, 14);
	painter->drawRect(l_o);
	QRectF l_o2(w - 24, 9, 8, 3);
	painter->drawRect(l_o2);
	QRectF l_o3(w - 24, 14, 8, 3);
	painter->drawRect(l_o3);

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

	qreal l_fMaxHeight = l_fHeight - 2 * OFF;
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
	while (true)
	{
		QTextLine l_oLine = l_oTextLayout.createLine();
		if (!l_oLine.isValid())
		{
			break;
		}

		qreal l_iLineWidth = l_fWidth - 2 * OFF;
		if (l_iY < 20)
		{
			l_iLineWidth -= 24;
		}

		l_oLine.setLineWidth(l_iLineWidth);
		l_oLine.setPosition(QPointF(OFF, l_iY));
		l_iY += l_oLine.height();
	}
	l_oTextLayout.endLayout();
	l_oTextLayout.draw(painter, QPoint(0, 0));
}

int box_component::linesCount(const QString& i_sInput, int i_iNumLines, qreal i_fWidth, qreal i_fHeight)
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
	while (true)
	{
		QTextLine l_oLine = l_oTextLayout.createLine();
		if (!l_oLine.isValid())
		{
			break;
		}

		qreal l_iLineWidth = i_fWidth - 2 * OFF;
		if (l_iY < 20)
		{
			l_iLineWidth -= 24;
		}

		l_oLine.setLineWidth(l_iLineWidth);
		l_oLine.setPosition(QPointF(OFF, l_iY));
		l_iY += l_oLine.height();
	}
	l_oTextLayout.endLayout();
	return l_oTextLayout.lineCount();
}

QSize box_component::best_size_for(const QString &i_sText)
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

		qreal l_fMaxHeight = l_fHeight - 2 * OFF;
		int l_iMaxLines = l_fMaxHeight / l_fLineHeight;
		if (l_iMaxLines < 1)
		{
			x += GRID;
			continue;
		}

		int l_iNumLines = linesCount(i_sText, l_iMaxLines, l_fWidth, l_fHeight);

		if (l_iNumLines > l_iMaxLines)
		{
			if (2 * y > x)
			{
				x += GRID;
			}
			else
			{
				y += GRID;
			}
		}
		else
		{
			break;
		}
	}
	while (true);

	return QSize(x, y);
}


