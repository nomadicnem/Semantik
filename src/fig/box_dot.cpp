// Thomas Nagy 2007-2018 GPLV3

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
#include <QTextDocument>
#include "box_dot.h"
#include "box_view.h"
 #include "box_link.h"
#include "data_item.h"
#include "sem_mediator.h"

#define PAD 0.5

box_dot::box_dot(box_view* i_oParent, int i_iId) : QGraphicsRectItem(), connectable(), m_oView(i_oParent)
{
	m_iId = i_iId;
	m_oItem = m_oView->m_oMediator->m_oItems[m_oView->m_iId];
	m_oBox = m_oItem->m_oBoxes[m_iId];
	Q_ASSERT(m_oBox);

	i_oParent->scene()->addItem(this);
	//setRect(QRectF(QPointF(0, 0), QSizeF(20, 20)));

	m_oChain = new box_chain(i_oParent);
	m_oChain->setParentItem(this);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(100);
	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);

	update_size();
}

box_dot::~box_dot()
{
	delete m_oChain;
}

void box_dot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setCosmetic(false);
	l_oPen.setWidthF(0.01 + 1);

	qreal pad = l_oPen.width() / 2.;
	QRectF l_oRect = rect().adjusted(pad, pad, -pad, -pad);


	painter->setPen(l_oPen);
	if (m_oBox->m_bIsEnd) {
		painter->drawEllipse(l_oRect);
		painter->setBrush(m_oBox->color);
		QRectF in = l_oRect.adjusted(PAD+1, PAD+1, -PAD-1, -PAD-1);
		painter->drawEllipse(in);
	} else {
		painter->setBrush(m_oBox->color);
		painter->drawEllipse(l_oRect);
	}
}

void box_dot::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(100);
	QGraphicsRectItem::mousePressEvent(e);
}

void box_dot::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(99);
	QGraphicsRectItem::mouseReleaseEvent(e);
}

/*QRectF box_dot::boundingRect() const {
	return rect().adjusted(-OFF, -OFF, OFF, OFF);
}*/

void box_dot::update_data() {
	setPos(QPointF(m_oBox->m_iXX, m_oBox->m_iYY));
	update_size();
	update();
}

void box_dot::update_size()
{
	setRect(0, 0, m_oBox->m_iWW, m_oBox->m_iHH);
	m_oChain->setPos(m_oBox->m_iWW + 3, 0);
}

QVariant box_dot::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionChange)
		{
			QPointF np = i_oValue.toPointF();
			np.setX(((int) np.x() / GRID) * GRID);
			np.setY(((int) np.y() / GRID) * GRID);
			return np;
		}
		else if (i_oChange == ItemPositionHasChanged)
		{
			update_links();
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			m_oChain->setVisible(isSelected());
			if (isSelected())
				setZValue(101);
			else
				setZValue(100);
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

void box_dot::update_links()
{
	foreach (box_link* l_oLink, m_oView->m_oLinks)
	{
		if (l_oLink->m_oInnerLink.m_iParent == m_iId || l_oLink->m_oInnerLink.m_iChild == m_iId)
		{
			l_oLink->update_pos();
		}
	}
}

int box_dot::choose_position(const QPointF& i_oP, box_link*, box_control_point*)
{
	QRectF r = rectPos();
	QPointF l_o = pos() - i_oP + QPointF(r.width()/2, r.height()/2);
	double c_x = l_o.x() * r.height();
	double c_y = l_o.y() * r.width();
	if (qAbs(c_x) > qAbs(c_y))
	{
		return (c_x > 0) ? data_link::WEST : data_link::EAST;
	}
	else
	{
		return (c_y > 0) ? data_link::NORTH : data_link::SOUTH;
	}
	return data_link::NORTH;
}

QPoint box_dot::get_point(int i_oP)
{
	QRectF r = rectPos();
	switch (i_oP & data_link::COORD) {
		case data_link::NORTH:
			return QPoint(r.x() + r.width() / 2., r.y());
		case data_link::WEST:
			return QPoint(r.x(), r.y() + r.height() / 2);
		case data_link::SOUTH:
			return QPoint(r.x() + r.width()/2., r.y() + r.height());
		case data_link::EAST:
			return QPoint(r.x() + r.width(), r.y() + r.height()/2.);
	}
	Q_ASSERT(false);
	return QPoint(0, 0);
}

