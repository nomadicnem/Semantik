// Thomas Nagy 2007-2021 GPLV3

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
#include "box_class.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"
#include "box_class_properties.h"

#include <QFont>

#define PAD 0.5
#define MIN_FORK_SIZE 30

//#define DEBUG

box_class::box_class(box_view* view, int id) : box_item(view, id)
{
        setZValue(80);
	doc.setDefaultFont(scene()->font());
	update_size();
}

void box_class::force_size()
{
	QSizeF l_o = size_min();
	setRect(0, 0, l_o.width(), l_o.height());

	m_iWW = m_oBox->m_iWW = l_o.width();
	m_iHH = m_oBox->m_iHH = l_o.height();
}

void box_class::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen;
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	else l_oPen.setStyle(Qt::SolidLine);
	i_oPainter->setPen(l_oPen);

	qreal pad = l_oPen.widthF() / 2.;
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
	if (!m_oBox->m_sStereotype.isEmpty())
	{
		QFont l_oFont(scene()->font());
		QString l_sText = QChar(0xAB) + m_oBox->m_sStereotype + QChar(0xBB);
		i_oPainter->setFont(l_oFont);
		QFontMetricsF l_oFm(l_oFont);

		QRectF l_oR = l_oFm.boundingRect(l_sText);
		l_oR.setWidth(l_oInnerRect.width());
		l_oR.moveTo(l_oInnerRect.topLeft() + QPointF(0, l_fHpos));
		l_oR.adjust(-1, 0, 1, 0);

		#ifdef DEBUG
		i_oPainter->save();
		QColor bc("#0000ff");
		i_oPainter->setBrush(bc);
		i_oPainter->drawRect(l_oR);
		i_oPainter->restore();
		#endif

		i_oPainter->drawText(l_oR, Qt::AlignCenter | Qt::TextSingleLine | Qt::AlignVCenter, l_sText);
		l_fHpos += l_oR.height();
	}

	{
		QFont l_oBoldFont(scene()->font());
		l_oBoldFont.setBold(true);
		l_oBoldFont.setItalic(m_oBox->m_bAbstract);
		l_oBoldFont.setUnderline(m_oBox->m_bStatic);
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
	QFontMetricsF l_oNormalFm(l_oNormalFont);
	qreal l_iHVisibility = minVisibility(l_oNormalFm);

	QFont l_oItalicFont(scene()->font());
	l_oItalicFont.setItalic(true);
	QFontMetricsF l_oItalicFm(l_oItalicFont);

	QFont l_oUnderlineFont(scene()->font());
	l_oUnderlineFont.setUnderline(true);
	QFontMetricsF l_oUnderlineFm(l_oUnderlineFont);

	QFont l_oUnderlineItalicFont(l_oItalicFont);
	l_oUnderlineItalicFont.setUnderline(true);
	QFontMetricsF l_oUnderlineItalicFm(l_oUnderlineItalicFont);


	if (m_oBox->m_oAttributes.size() > 0)
	{
		l_fHpos += PAD;
		i_oPainter->drawLine(l_oRect.topLeft() + QPointF(0, l_fHpos +1 + PAD), l_oRect.topRight() + QPointF(0, l_fHpos + 1 + PAD));
		l_fHpos += 1 + PAD;
	}
	foreach (const data_box_attribute &l_o, m_oBox->m_oAttributes) {
		QRectF l_oR;
		if (l_o.m_bStatic) {
			l_oR = l_oUnderlineFm.boundingRect(l_o.m_sText);
			i_oPainter->setFont(l_oUnderlineFont);
		} else {
			l_oR = l_oNormalFm.boundingRect(l_o.m_sText);
			i_oPainter->setFont(l_oNormalFont);
		}

		l_oR.setWidth(l_oInnerRect.width());
		qreal l_fOff = PAD + l_iHVisibility;
		l_oR.setWidth(l_oR.width() - l_fOff);
		l_oR.moveTo(l_oInnerRect.topLeft() + QPointF(l_fOff, l_fHpos));
		l_oR.adjust(-1, 0, 1, 0);

		#ifdef DEBUG
		i_oPainter->save();
		QColor bc("#ff0000");
		i_oPainter->setBrush(bc);
		i_oPainter->drawRect(l_oR);
		i_oPainter->restore();
		#endif

		i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o.m_sText);

		QString l_sVis;
		if (l_o.m_oVisibility == visibility::PUBLIC) {
			l_sVis = "+";
		} else if (l_o.m_oVisibility == visibility::PROTECTED) {
			l_sVis = "#";
		} else if (l_o.m_oVisibility == visibility::PRIVATE) {
			l_sVis = QChar(0x2212);
		} else if (l_o.m_oVisibility == visibility::PACKAGE) {
			l_sVis = "~";
		} else if (l_o.m_oVisibility == visibility::DERIVED) {
			l_sVis = "/";
		} else {
			Q_ASSERT(QString("Invalid visibility %1").arg(l_o.m_oVisibility) != "0");
		}

		l_oR.translate(QPointF(- l_fOff, 0));
		l_oR.setWidth(l_iHVisibility + 2);
		i_oPainter->setFont(l_oNormalFont);

		#ifdef DEBUG
		i_oPainter->save();
                QColor bic("#00ff00");
                i_oPainter->setBrush(bic);
                i_oPainter->drawRect(l_oR);
                i_oPainter->restore();
		#endif

		i_oPainter->drawText(l_oR, Qt::AlignCenter | Qt::TextSingleLine | Qt::AlignTop, l_sVis);

		l_fHpos += l_oR.height();
	}

	if (m_oBox->m_oMethods.size() > 0)
	{
		l_fHpos += PAD;
		i_oPainter->drawLine(l_oRect.topLeft() + QPointF(0, l_fHpos + 1 + PAD), l_oRect.topRight() + QPointF(0, l_fHpos + 1 + PAD));
		l_fHpos += 1 + PAD;
	}

	foreach (const data_box_method &l_o, m_oBox->m_oMethods)
	{
		QRectF l_oR;
		if (l_o.m_bAbstract && l_o.m_bStatic)
		{
			l_oR = l_oUnderlineItalicFm.boundingRect(l_o.m_sText);
			i_oPainter->setFont(l_oUnderlineItalicFont);
		}
		else if (l_o.m_bAbstract)
		{
			l_oR = l_oItalicFm.boundingRect(l_o.m_sText);
			i_oPainter->setFont(l_oItalicFont);
		}
		else if (l_o.m_bStatic)
		{
			l_oR = l_oUnderlineFm.boundingRect(l_o.m_sText);
			i_oPainter->setFont(l_oUnderlineFont);
		}
		else
		{
			l_oR = l_oNormalFm.boundingRect(l_o.m_sText);
			i_oPainter->setFont(l_oNormalFont);
		}

		l_oR.setWidth(l_oInnerRect.width());
		qreal l_fOff = PAD + l_iHVisibility;
		l_oR.setWidth(l_oR.width() - l_fOff);
		l_oR.moveTo(l_oInnerRect.topLeft() + QPointF(l_fOff, l_fHpos));
		l_oR.adjust(-1, 0, 1, 0);

		#ifdef DEBUG
		i_oPainter->save();
		QColor bc("#ff0000");
		i_oPainter->setBrush(bc);
		i_oPainter->drawRect(l_oR);
		i_oPainter->restore();
		#endif

		i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o.m_sText);

		QString l_sVis;
		if (l_o.m_oVisibility == visibility::PUBLIC) {
			l_sVis = "+";
		} else if (l_o.m_oVisibility == visibility::PROTECTED) {
			l_sVis = "#";
		} else if (l_o.m_oVisibility == visibility::PRIVATE) {
			l_sVis = QChar(0x2212);
		} else if (l_o.m_oVisibility == visibility::PACKAGE) {
			l_sVis = "~";
		} else if (l_o.m_oVisibility == visibility::DERIVED) {
			l_sVis = "/";
		} else {
			Q_ASSERT(QString("Invalid visibility %1").arg(l_o.m_oVisibility) != "0");
		}

		l_oR.translate(QPointF(- l_fOff, 0));
		l_oR.setWidth(l_iHVisibility + 2);
		i_oPainter->setFont(l_oNormalFont);

		#ifdef DEBUG
		i_oPainter->save();
                QColor bic("#00ff00");
                i_oPainter->setBrush(bic);
                i_oPainter->drawRect(l_oR);
                i_oPainter->restore();
		#endif

		i_oPainter->drawText(l_oR, Qt::AlignCenter | Qt::TextSingleLine | Qt::AlignTop, l_sVis);
		l_fHpos += l_oR.height();
	}
}

qreal box_class::minVisibility(const QFontMetricsF i_oFm)
{
	qreal l_iHVisibility = 0;
	qreal l_fW;
	l_fW = i_oFm.width("+");
	if (l_fW > l_iHVisibility) l_iHVisibility = l_fW;
	l_fW = i_oFm.width(QChar(0x2212));
	if (l_fW > l_iHVisibility) l_iHVisibility = l_fW;
	l_fW = i_oFm.width("#");
	if (l_fW > l_iHVisibility) l_iHVisibility = l_fW;
	l_fW = i_oFm.width("~");
	if (l_fW > l_iHVisibility) l_iHVisibility = l_fW;
	l_fW = i_oFm.width("/");
	if (l_fW > l_iHVisibility) l_iHVisibility = l_fW;
	return l_iHVisibility;
}

QSize box_class::best_size(const QPointF &dims)
{
	QSizeF size_min_val = size_min();

	int x = dims.x();
	x = GRID * (x / GRID);
	if (x < GRID) x = GRID;

	while (x < size_min_val.width()) {
		x += GRID;
	}

	int y = dims.y();
	y = GRID * (y / GRID);
	if (y < GRID) y = GRID;
	while (y < size_min_val.height())
	{
		y += GRID;
	}
	return QSize(x, y);
}

QSizeF box_class::size_min()
{
	QFont l_oNormalFont(scene()->font());
	QFontMetricsF l_oNormalFm(l_oNormalFont);
	qreal l_iHVisibility = minVisibility(l_oNormalFm);

	QFont l_oItalicFont(scene()->font());
	l_oItalicFont.setItalic(true);
	QFontMetricsF l_oItalicFm(l_oItalicFont);

	QFont l_oUnderlineFont(scene()->font());
	l_oUnderlineFont.setUnderline(true);
	QFontMetricsF l_oUnderlineFm(l_oUnderlineFont);

	QFont l_oUnderlineItalicFont(l_oUnderlineFont);
	l_oUnderlineFont.setItalic(true);
	QFontMetricsF l_oUnderlineItalicFm(l_oUnderlineItalicFont);

	qreal l_iWW = 0, l_iHH = 2 * PAD;
	if (m_oBox->m_oMethods.size() > 0)
	{
		l_iHH += 2 * PAD +  1; // 1 for the line
	}
	foreach (const data_box_method &l_o, m_oBox->m_oMethods)
	{
		QRectF l_oR;

		if (l_o.m_bAbstract && l_o.m_bStatic)
		{
			l_oR = l_oUnderlineItalicFm.boundingRect(l_o.m_sText);
		}
		else if (l_o.m_bAbstract)
		{
			l_oR = l_oItalicFm.boundingRect(l_o.m_sText);
		}
		else if (l_o.m_bStatic)
		{
			l_oR = l_oUnderlineFm.boundingRect(l_o.m_sText);
		}
		else
		{
			l_oR = l_oNormalFm.boundingRect(l_o.m_sText);
		}
		l_iWW = qMax(l_oR.width(), l_iWW);
		l_iHH += l_oR.height();
	}

	if (m_oBox->m_oAttributes.size() > 0) {
		l_iHH += 2 * PAD + 1; // 1 for the line
	}
	foreach (const data_box_attribute &l_o, m_oBox->m_oAttributes) {
		QRectF l_oR;
		if (l_o.m_bStatic) {
			l_oR = l_oUnderlineFm.boundingRect(l_o.m_sText);
		} else {
			l_oR = l_oNormalFm.boundingRect(l_o.m_sText);
		}

		l_iWW = qMax(l_oR.width(), l_iWW);
		l_iHH += l_oR.height();
	}

	l_iWW += PAD + l_iHVisibility;
	if (!m_oBox->m_sStereotype.isEmpty())
	{
		QString l_sText = QChar(0xAB) + m_oBox->m_sStereotype + QChar(0xBB);
		QFontMetricsF l_oFm(l_oNormalFont);

		QRectF l_oR = l_oFm.boundingRect(l_sText);
		l_iWW = qMax(l_oR.width(), l_iWW);
		l_iHH += l_oR.height();
	}

	l_oNormalFont.setBold(true);
	if (m_oBox->m_bAbstract)
	{
		l_oNormalFont.setItalic(true);
	}
	if (m_oBox->m_bStatic)
	{
		l_oNormalFont.setUnderline(true);
	}

	{
		QFontMetricsF l_oFm(l_oNormalFont);
		QString l_sText = m_oBox->m_sText;
		if (l_sText.isEmpty()) {
			l_sText = notr(" ");
		}

		QRectF l_oR = l_oFm.boundingRect(l_sText);
		l_iWW = qMax(l_oR.width(), l_iWW);
		l_iHH += l_oR.height();
	}

	// +2 for the border width
	int l_iWWN = (int) (l_iWW + 2 + 2 * PAD);
	int l_iHHN = (int) (l_iHH + 2 * PAD);

	//int RG = GRID/2;
	//if (l_iWWN % RG) l_iWWN = RG * (1 + l_iWWN / RG);
	//if (l_iHHN % RG) l_iHHN = RG * (1 + l_iHHN / RG);

	return QSizeF(l_iWWN, l_iHHN); // adjusted
}

void box_class::properties()
{
	box_class_properties props(m_oView, this);
	QStringList l_oS;

	if (props.m_oClass->m_oBox->m_bStatic) {
		l_oS<<notr("static ");
	}
	if (props.m_oClass->m_oBox->m_bAbstract) {
		l_oS<<notr("abstract ");
	}

	l_oS<<notr("class ");
	l_oS<<props.m_oClass->m_oBox->m_sText<<notr("\n");
	if (!props.m_oClass->m_oBox->m_sStereotype.isEmpty()) {
		l_oS<<notr("stereotype ")<<props.m_oClass->m_oBox->m_sStereotype<<notr("\n");
	}

	l_oS<<notr("\n");

	foreach (const data_box_attribute &l_o, m_oBox->m_oAttributes) {
		if (l_o.m_oVisibility == visibility::PUBLIC) {
			l_oS<<notr("public\t");
		} else if (l_o.m_oVisibility == visibility::PROTECTED) {
			l_oS<<notr("protected\t");
		} else if (l_o.m_oVisibility == visibility::PRIVATE) {
			l_oS<<notr("private\t");
		} else if (l_o.m_oVisibility == visibility::PACKAGE) {
			l_oS<<notr("package\t");
		} else if (l_o.m_oVisibility == visibility::DERIVED) {
			l_oS<<notr("derived\t");
		} else {
			Q_ASSERT(QString("Invalid visibility %1").arg(l_o.m_oVisibility) != "0");
		}
		if (l_o.m_bStatic) {
			l_oS<<notr("static\t");
		}
		l_oS<<l_o.m_sText<<notr("\n");
	}

	l_oS<<notr("\n");

	foreach (const data_box_method &l_o, m_oBox->m_oMethods) {
		if (l_o.m_oVisibility == visibility::PUBLIC) {
			l_oS<<notr("public\t");
		} else if (l_o.m_oVisibility == visibility::PROTECTED) {
			l_oS<<notr("protected\t");
		} else if (l_o.m_oVisibility == visibility::PRIVATE) {
			l_oS<<notr("private\t");
		} else if (l_o.m_oVisibility == visibility::PACKAGE) {
			l_oS<<notr("package\t");
		} else if (l_o.m_oVisibility == visibility::DERIVED) {
			l_oS<<notr("derived\t");
		} else {
			Q_ASSERT(QString("Invalid visibility %1").arg(l_o.m_oVisibility) != "0");
		}
		if (l_o.m_bAbstract) {
			l_oS<<notr("abstract\t");
		}
		if (l_o.m_bStatic) {
			l_oS<<notr("static\t");
		}
		l_oS<<l_o.m_sText<<notr("\n");
	}

	props.m_oClassDefinition->setText(l_oS.join(notr("")));
	props.m_oClassDefinition->selectAll();
	props.exec();
}

