// Thomas Nagy 2007-2018 GPLV3

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
#include "box_actor.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_actor::box_actor(box_view* view, int id) : box_item(view, id)
{
	m_oCaption = new QGraphicsTextItem();
	m_oCaption->setParentItem(this);
	m_oCaption->setPos(0, 0);
}

box_actor::~box_actor() {
	delete m_oCaption;
}

void box_actor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

	painter->drawLine(QLineF(xcoord, ytop + 2 * ycoord, xcoord, ytop + 4 * ycoord));
	painter->drawLine(QLineF(xtop, ytop + 3 * ycoord, xtop + l_oRect.width(), ytop + 3 * ycoord));

	painter->drawLine(QLineF(xtop, ytop + l_oRect.height(), xcoord, ytop + 4 * ycoord));
	painter->drawLine(QLineF(xtop + l_oRect.width(), ytop + l_oRect.height(), xcoord, ytop + 4 * ycoord));

	double cir = 4 * l_oRect.width() / 10.;
	if (cir > ycoord) {
		cir = ycoord;
	}
	painter->drawEllipse(QRectF(xcoord - cir, ytop + 2 * ycoord - 2 * cir, 2 * cir, 2 * cir));
}

void box_actor::properties()
{
	bool ok = false;
	QString text = QInputDialog::getText(m_oView, i18n("Actor properties"),
			i18n("Caption:"), QLineEdit::Normal, m_oBox->m_sText, &ok);
	if (ok && text != m_oBox->m_sText)
	{
		mem_edit_box *ed = new mem_edit_box(m_oView->m_oMediator, m_oView->m_iId, m_iId);
		ed->newText = text;
		ed->apply();
	}
}

void box_actor::update_size() {
	m_iWW = m_oBox->m_iWW;
	m_iHH = m_oBox->m_iHH;

	prepareGeometryChange();
	setRect(0, 0, m_iWW, m_iHH);
	m_oChain->setPos(m_iWW + 3, 0);

	update_links();
	update_sizers();
}

void box_actor::update_links() {
	QRectF r = boundingRect();
	m_oCaption->setFont(scene()->font());
	m_oCaption->setPlainText(m_oBox->m_sText);
	m_oCaption->setPos((r.width() - m_oCaption->boundingRect().width()) / 2., r.height());
	box_item::update_links();
}

