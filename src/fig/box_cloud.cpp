// Thomas Nagy 2020-2020 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QGraphicsTextItem>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include <QInputDialog>
#include "box_cloud.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_cloud::box_cloud(box_view* view, int id) : box_item(view, id)
{
	m_oCaption = new QGraphicsTextItem();
	m_oCaption->setParentItem(this);
	m_oCaption->setPos(0, 0);
}

box_cloud::~box_cloud() {
	delete m_oCaption;
}

void box_cloud::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	painter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);

	qreal xtop = l_oRect.x();
	qreal ytop = l_oRect.y();
	qreal xcoord = xtop + l_oRect.width() / 2.0;
	qreal ycoord = l_oRect.height() / 5.;

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

	qreal w = l_oRect.width();
	qreal h = l_oRect.height();

	int hdiv = 4;
	int vdiv = 2;

	qreal min = h;
	qreal prev_min = min;
	for (int x=2; x < 100; ++x) {
		qreal new_min = qAbs(h - (x * w) / hdiv);
		if (new_min < min)
		{
			min = new_min;
			vdiv = x;
		}

		if (new_min > prev_min) {
			break;
		}
		prev_min = new_min;
	}

	qreal CONST = 1.2;
	qreal dx = (CONST * l_oRect.width()) / hdiv;
	qreal dy = (CONST * l_oRect.height()) / vdiv;

	qreal xstep = (w - dx) / (hdiv - 1);
	qreal ystep = (h - dy) / (vdiv - 1);

	for (int i=0; i<hdiv; ++i)
	{
		painter->drawEllipse(QRectF(xtop + i * xstep, ytop, dx, dy));
	}

	for (int i=0; i<vdiv; ++i)
	{
		painter->drawEllipse(QRectF(xtop + w - dx, ytop + i * ystep, dx, dy));
	}

	for (int i=hdiv-1; i >= 0; --i)
	{
		painter->drawEllipse(QRectF(xtop + i * xstep, ytop + h - dy, dx, dy));
	}

	for (int i=vdiv-1; i >= 1; --i)
	{
		painter->drawEllipse(QRectF(xtop, ytop + i * ystep, dx, dy));
	}

	painter->setPen(Qt::NoPen);
	painter->drawRect(l_oRect.adjusted(dx/4., dy/4., -dx/4., -dy/4.));
}

void box_cloud::properties()
{
	bool ok = false;
	QString text = QInputDialog::getText(m_oView, i18n("Cloud properties"),
			i18n("Caption:"), QLineEdit::Normal, m_oBox->m_sText, &ok);
	if (ok && text != m_oBox->m_sText)
	{
		mem_edit_box *ed = new mem_edit_box(m_oView->m_oMediator, m_oView->m_iId, m_iId);
		ed->newText = text;
		ed->apply();
	}
}

void box_cloud::update_size() {
	m_iWW = m_oBox->m_iWW;
	m_iHH = m_oBox->m_iHH;

	prepareGeometryChange();
	setRect(0, 0, m_iWW, m_iHH);
	m_oChain->setPos(m_iWW + 3, 0);

	update_links();
	update_sizers();
}

void box_cloud::update_links() {
	QRectF r = boundingRect();
	m_oCaption->setFont(scene()->font());
	m_oCaption->setPlainText(m_oBox->m_sText);
	m_oCaption->setPos((r.width() - m_oCaption->boundingRect().width()) / 2., (r.height() - m_oCaption->boundingRect().height()) / 2.);
	box_item::update_links();
}

