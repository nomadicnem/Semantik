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
#include <QTextEdit>
#include <QStringList>
#include <QTextDocument>
#include "box_entity.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"
#include "box_entity_properties.h"

#include "math.h"
#include <QFont>

#define PAD 0.5
#define MIN_FORK_SIZE 30

//#define DEBUG


box_entity::box_entity(box_view* view, int id) : box_item(view, id)
{
        setZValue(80);
	update_size();
}

void box_entity::force_size()
{
	QSizeF l_o = size();

	int l_iWW = fceil(l_o.width(), GRID);
	int l_iHH = fceil(l_o.height(), GRID);

	setRect(0, 0, l_iWW, l_iHH);

	m_iWW = m_oBox->m_iWW = l_iWW;
	m_iHH = m_oBox->m_iHH = l_iHH;
}

void box_entity::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen;
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	else l_oPen.setStyle(Qt::SolidLine);
	i_oPainter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);


	QColor bc = m_oBox->getColor(m_oView->m_oMediator);
	i_oPainter->setBrush(bc);
	i_oPainter->drawRect(l_oRect);

	l_oPen.setStyle(Qt::SolidLine);

	QRectF l_oInnerRect = l_oRect.adjusted(1 + PAD, 1 + PAD, -1 - PAD, -1 - PAD);

	#ifdef DEBUG
	i_oPainter->save();
	QColor bic("#00ffff");
	i_oPainter->setBrush(bic);
	i_oPainter->drawRect(l_oInnerRect);
	i_oPainter->restore();
	#endif

	qreal l_fHpos = 0;
	{
		QFont l_oBoldFont(scene()->font());
		l_oBoldFont.setBold(true);
		i_oPainter->setFont(l_oBoldFont);
		QFontMetricsF l_oFm(l_oBoldFont);

		QString l_sText = m_oBox->m_sText;
		if (l_sText.isEmpty()) {
			l_sText = notr(" ");
		}

		QRectF l_oR = l_oFm.boundingRect(l_sText);

		l_oR.setWidth(l_oInnerRect.width());
		l_oR.moveTo(l_oInnerRect.topLeft() + QPointF(0, l_fHpos));
		l_oR.adjust(-1, 0, 1, 0);

		#ifdef DEBUG
		i_oPainter->save();
		QColor bc("#ffff00");
		i_oPainter->setBrush(bc);
		i_oPainter->drawRect(l_oR);
		i_oPainter->restore();
		#endif

		i_oPainter->drawText(l_oR, Qt::AlignCenter | Qt::TextSingleLine | Qt::AlignVCenter, l_sText);

		l_fHpos += l_oR.height();
	}

	QFont l_oNormalFont(scene()->font());
	i_oPainter->setFont(l_oNormalFont);
	QFontMetricsF l_oNormalFm(l_oNormalFont);

	if (m_oBox->m_oEntityValues.size() > 0)
	{
		l_fHpos += PAD;
		i_oPainter->drawLine(l_oRect.topLeft() + QPointF(0, l_fHpos +1 + PAD), l_oRect.topRight() + QPointF(0, l_fHpos + 1 + PAD));
		l_fHpos += 1 + PAD;
	}

	qreal l_fSizeAttr = 0;
	qreal l_fSizeType = 0;
	qreal l_fSizeKey = 0;
	foreach (const data_box_entity_value &l_o, m_oBox->m_oEntityValues)
	{
		QRectF l_oR = l_oNormalFm.boundingRect(l_o.m_sName);
		l_fSizeAttr = qMax(l_fSizeAttr, l_oR.width() + 2);

		if (!l_o.m_sType.isEmpty())
		{
			l_oR = l_oNormalFm.boundingRect(l_o.m_sType);
			l_fSizeType = qMax(l_fSizeType, l_oR.width() + 2);
		}
		if (!l_o.m_sKey.isEmpty())
		{
			l_oR = l_oNormalFm.boundingRect(l_o.m_sKey);
			l_fSizeKey = qMax(l_fSizeKey, l_oR.width() + 2);
		}
	}

	qreal l_fSpacer = l_oNormalFm.boundingRect(notr("  ")).width() / 2.;

	qreal l_fLeftPad = PAD;
	if (l_oInnerRect.width() > l_fSpacer * 2 + l_fSizeAttr + l_fSizeType + l_fSizeKey)
	{
		l_fLeftPad = (l_oInnerRect.width() - l_fSpacer * 2 - l_fSizeAttr - l_fSizeType - l_fSizeKey) / 2.;
	}

	foreach (const data_box_entity_value &l_o, m_oBox->m_oEntityValues)
	{
		QRectF l_oR = l_oNormalFm.boundingRect(l_o.m_sName + l_o.m_sType + l_o.m_sKey);

		l_oR.moveTo(l_oInnerRect.topLeft() + QPointF(l_fLeftPad, l_fHpos));

		l_oR.setWidth(l_fSizeAttr);

		#ifdef DEBUG
		i_oPainter->save();
		QColor bc("#ff0000");
		i_oPainter->setBrush(bc);
		i_oPainter->drawRect(l_oR);
		i_oPainter->restore();
		#endif

		i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o.m_sName);

		if (!l_o.m_sType.isEmpty())
		{
			l_oR.setWidth(l_fSizeType);
			l_oR.translate(l_fSizeAttr + l_fSpacer, 0);

			#ifdef DEBUG
			i_oPainter->save();
			QColor bc("#ff0000");
			i_oPainter->setBrush(bc);
			i_oPainter->drawRect(l_oR);
			i_oPainter->restore();
			#endif

			i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o.m_sType);

		}
		if (!l_o.m_sType.isEmpty())
		{
			l_oR.translate(l_fSizeType + l_fSpacer, 0);
			l_oR.setWidth(l_fSizeKey);

			#ifdef DEBUG
			i_oPainter->save();
			QColor bc("#ff0000");
			i_oPainter->setBrush(bc);
			i_oPainter->drawRect(l_oR);
			i_oPainter->restore();
			#endif

			i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o.m_sKey);
		}

		l_fHpos += l_oR.height();
	}
}

QSizeF box_entity::size()
{
	QFont l_oNormalFont(scene()->font());
	QFontMetricsF l_oNormalFm(l_oNormalFont);

	qreal l_iWW = 0, l_iHH = 2 * PAD;
	if (m_oBox->m_oEntityValues.size() > 0) {
		l_iHH += 2 * PAD + 1; // 1 for the line
	}

	qreal l_fSizeAttr = 0;
	qreal l_fSizeType = 0;
	qreal l_fSizeKey = 0;
	foreach (const data_box_entity_value &l_o, m_oBox->m_oEntityValues)
	{
		QRectF l_oR = l_oNormalFm.boundingRect(l_o.m_sName);
		l_fSizeAttr = qMax(l_fSizeAttr, l_oR.width());

		if (!l_o.m_sType.isEmpty())
		{
			l_oR = l_oNormalFm.boundingRect(l_o.m_sType);
			l_fSizeType = qMax(l_fSizeType, l_oR.width());
		}
		if (!l_o.m_sKey.isEmpty())
		{
			l_oR = l_oNormalFm.boundingRect(l_o.m_sKey);
			l_fSizeKey = qMax(l_fSizeKey, l_oR.width());
		}

		QRectF l_oVer = l_oNormalFm.boundingRect(l_o.m_sName + l_o.m_sType + l_o.m_sKey);
		l_iHH += l_oVer.height();
	}
	qreal l_fSpacer = l_oNormalFm.boundingRect(notr("  ")).width() / 2.;

	l_iWW = qMax(l_iWW, l_fSizeAttr + l_fSizeType + l_fSizeKey + 4 * l_fSpacer + 2 * PAD);

	l_oNormalFont.setBold(true);

	{
		QFontMetricsF l_oFm(l_oNormalFont);
		QString l_sText = m_oBox->m_sText;
		if (l_sText.isEmpty()) {
			l_sText = notr(" ");
		}

		QRectF l_oR = l_oFm.boundingRect(l_sText);

		l_iWW = qMax(l_oR.width() + 2 * l_fSpacer, l_iWW);
		l_iHH += l_oR.height();
	}

	// +2 for the border width
	int l_iWWN = (int) (l_iWW + 2 + 2 * PAD);
	int l_iHHN = (int) (l_iHH + 2 * PAD);

	return QSizeF(l_iWWN, l_iHHN); // adjusted
}

void box_entity::properties()
{
	box_entity_properties props(m_oView, this);
	QStringList l_oS;

	l_oS<<notr("entity ")<<props.m_oEntity->m_oBox->m_sText<<"\n";

	int l_oSizeName = 0;
	int l_oSizeType = 0;
	foreach (const data_box_entity_value &l_o, m_oBox->m_oEntityValues)
	{
		l_oSizeName = qMax(l_o.m_sName.size(), l_oSizeName);
		l_oSizeType = qMax(l_o.m_sType.size(), l_oSizeType);
	}

	foreach (const data_box_entity_value &l_o, m_oBox->m_oEntityValues)
	{
		l_oS<<l_o.m_sName.leftJustified(l_oSizeName);
		if (!l_o.m_sType.isEmpty())
		{
			l_oS<<" "<<l_o.m_sType.leftJustified(l_oSizeType);
		}
		if (!l_o.m_sKey.isEmpty())
		{
			l_oS<<" ("<<l_o.m_sKey<<")";
		}
		l_oS<<notr("\n");
	}

	l_oS<<notr("\n");
	props.m_oEntityDefinition->setText(l_oS.join(notr("")));
	props.m_oEntityDefinition->selectAll();
	props.exec();
}

