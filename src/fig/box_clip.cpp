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
#include "box_clip.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define MIN_FORK_SIZE 30

box_clip::box_clip(box_view* view, int id) : box_item(view, id)
{
	m_oCaption = new QGraphicsTextItem();
	m_oCaption->setParentItem(this);
	m_oCaption->setPos(0, 0);

	QString l_sImg = QString(SEMANTIK_DIR)+"/flags/" + notr("flag_lunch.svg");
	m_oRenderer.load(l_sImg);
}

box_clip::~box_clip() {
	delete m_oCaption;
}

void box_clip::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	i_oPainter->setPen(l_oPen);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);

	m_oRenderer.render(i_oPainter, l_oRect);
}

void box_clip::properties()
{
	bool ok = false;
	QString text = QInputDialog::getText(m_oView, i18n("Shape properties"),
			i18n("Caption:"), QLineEdit::Normal, m_oBox->m_sText, &ok);
	if (ok && text != m_oBox->m_sText)
	{
		mem_edit_box *ed = new mem_edit_box(m_oView->m_oMediator, m_oView->m_iId, m_iId);
		ed->newText = text;
		ed->apply();
	}
}

void box_clip::update_size() {
	m_iWW = m_oBox->m_iWW;
	m_iHH = m_oBox->m_iHH;

	prepareGeometryChange();
	setRect(0, 0, m_iWW, m_iHH);
	m_oChain->setPos(m_iWW + 3, 0);

	update_links();
	update_sizers();
}

void box_clip::update_links() {
	QRectF r = boundingRect();
	m_oCaption->setFont(scene()->font());
	m_oCaption->setPlainText(m_oBox->m_sText);
	m_oCaption->setPos((r.width() - m_oCaption->boundingRect().width()) / 2., (r.height() - m_oCaption->boundingRect().height()) / 2.);
	box_item::update_links();
}

