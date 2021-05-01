// Thomas Nagy 2007-2021 GPLV3

#include <QApplication>
#include <QPainter>
#include <QInputDialog>
#include <QLineEdit>
#include <QtDebug>
#include <QSpinBox>
#include <QComboBox>
#include "CON.h"
#include "box_link.h"
#include "box_view.h"
#include "data_item.h"
#include "mem_box.h"
#include <cmath>
#include <box_control_point.h>
#include "box_link_properties.h"

#define pad 25
#define MAX 2000000000
#define DAMP 1000000
#define RADIUS 3


box_link::box_link(box_view* i_oParent) : QGraphicsRectItem(), editable()
{
	m_oInnerLink.m_iParentPos = 0;
	m_oInnerLink.m_iChildPos = 0;

	m_oView = i_oParent;

	m_oLink = NULL;

	i_oParent->scene()->addItem(this);
	setZValue(98);

	setFlags(ItemIsSelectable);
	m_bReentrantLock = false;

	m_oStartPoint = new box_control_point(m_oView);
	m_oStartPoint->hide();
	m_oStartPoint->m_bIsSegment = false;
	m_oStartPoint->m_oLink = this;
	m_oStartPoint->setZValue(117);

	m_oEndPoint = new box_control_point(m_oView);
	m_oEndPoint->hide();
	m_oEndPoint->m_bIsSegment = false;
	m_oEndPoint->m_oLink = this;
	m_oEndPoint->setZValue(117);

	m_oStartCaption = new QGraphicsTextItem();
	m_oStartCaption->setFlags(0);
	m_oStartCaption->setZValue(116);
	i_oParent->scene()->addItem(m_oStartCaption);

	m_oEndCaption = new QGraphicsTextItem();
	m_oEndCaption->setFlags(0);
	m_oEndCaption->setZValue(116);
	i_oParent->scene()->addItem(m_oEndCaption);

	m_oMidCaption = new QGraphicsTextItem();
	m_oMidCaption->setFlags(0);
	m_oMidCaption->setZValue(116);
	i_oParent->scene()->addItem(m_oMidCaption);

	//m_oStartCaption->setPlainText("1..*");
	//m_oEndCaption->setPlainText("1..*");

	for (int i=0; i < 7; ++i)
	{
		box_control_point* b = new box_control_point(m_oView);
		b->hide();
		b->m_oLink = this;
		b->m_iOffset = i;
		m_oControlPoints.append(b);
	}
}

box_link::~box_link()
{
	foreach (box_control_point *b, m_oControlPoints) {
		delete b;
	}
	delete m_oStartCaption;
	delete m_oEndCaption;
	delete m_oMidCaption;
	delete m_oStartPoint;
	delete m_oEndPoint;
}

#define xw 5.
#define yw 12.


void compute_angles(bool i_bParent, data_link::LineType i_iType, data_link::Direction i_iDir, qreal *cosphi, qreal *sinphi, qreal ddx, qreal ddy)
{
	if (i_iType)
	{
		if (i_bParent) {
			*sinphi = ddy;
			*cosphi = ddx;
		} else {
			*sinphi = -ddy;
			*cosphi = -ddx;
		}
	}
	else
	{
		switch (i_iDir) //m_oInnerLink.m_iParentPos & data_link::COORD)
		{
			case data_link::NORTH:
				{
					*sinphi = -1.;
					*cosphi = 0.;
				}
				break;
			case data_link::WEST:
				{
					*sinphi = 0.;
					*cosphi = -1.;
				}
				break;
			case data_link::SOUTH:
				{
					*sinphi = 1.;
					*cosphi = 0.;
				}
				break;
			case data_link::EAST:
				{
					*sinphi = 0.;
					*cosphi = 1.;
				}
				break;
			default:
				Q_ASSERT(false);
		}
	}
}


void box_link::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
	QList<QGraphicsItem*> l_oColliding = collidingItems();
	QList<box_link*> l_oLinks;
	foreach (QGraphicsItem* l_oItem, l_oColliding)
	{
		box_link *l_oLink = dynamic_cast<box_link*>(l_oItem);
		if (l_oLink != NULL && !l_oLink->m_oInnerLink.m_iLineType)
		{
			if (m_oInnerLink.m_iId > l_oLink->m_oInnerLink.m_iId)
			{
				l_oLinks.push_back(l_oLink);
			}
		}
	}

	i_oPainter->setBrush(m_oInnerLink.getColor(m_oView->m_oMediator));

	QPen l_oPen(m_oInnerLink.pen_style);
	l_oPen.setWidthF(0.01 + m_oInnerLink.border_width);
	l_oPen.setColor(m_oInnerLink.getColor(m_oView->m_oMediator));
	l_oPen.setJoinStyle(Qt::RoundJoin);
	l_oPen.setCosmetic(false);
	i_oPainter->setPen(l_oPen);

	if (m_oInnerLink.m_iLineType)
	{
		int last = m_oGood.size() - 1;
		QLineF l_oLine(m_oGood[0].x(), m_oGood[0].y(), m_oGood[last].x(), m_oGood[last].y());
		i_oPainter->drawLine(l_oLine);
	}
	else
	{
		for (int i=0; i<m_oGood.size() - 1; ++i)
		{
			QLineF l_oLine(m_oGood[i].x(), m_oGood[i].y(), m_oGood[i+1].x(), m_oGood[i+1].y());
			if (l_oLinks.size() > 0 && l_oLine.length() > 10)
			{
				QList<int> l_oSub;
				bool l_bIsVertical = m_oGood[i].x() == m_oGood[i+1].x();
				int l_iYMin = qMin(m_oGood[i].y(), m_oGood[i + 1].y());
				int l_iYMax = qMax(m_oGood[i].y(), m_oGood[i + 1].y());
				int l_iXMin = qMin(m_oGood[i].x(), m_oGood[i + 1].x());
				int l_iXMax = qMax(m_oGood[i].x(), m_oGood[i + 1].x());

				foreach (box_link *l_oLink, l_oLinks)
				{
					for (int j=0; j<l_oLink->m_oGood.size() - 1; ++j)
					{
						if (l_bIsVertical)
						{
							int l_iXMin2 = qMin(l_oLink->m_oGood[j].x(), l_oLink->m_oGood[j + 1].x());
							int l_iXMax2 = qMax(l_oLink->m_oGood[j].x(), l_oLink->m_oGood[j + 1].x());

							if (l_oLink->m_oGood[j].y() == l_oLink->m_oGood[j + 1].y()
								&& l_oLink->m_oGood[j].y() > l_iYMin
								&& l_oLink->m_oGood[j].y() < l_iYMax
								&& l_iXMin2 < l_iXMin
								&& l_iXMax2 > l_iXMin)
							{
								l_oSub.append(l_oLink->m_oGood[j].y());
							}
						}
						else
						{
							int l_iYMin2 = qMin(l_oLink->m_oGood[j].y(), l_oLink->m_oGood[j + 1].y());
							int l_iYMax2 = qMax(l_oLink->m_oGood[j].y(), l_oLink->m_oGood[j + 1].y());

							if (l_oLink->m_oGood[j].x() == l_oLink->m_oGood[j + 1].x()
								&& l_oLink->m_oGood[j].x() > l_iXMin
								&& l_oLink->m_oGood[j].x() < l_iXMax
								&& l_iYMin2 < l_iYMin
								&& l_iYMax2 > l_iYMax)
							{
								l_oSub.append(l_oLink->m_oGood[j].x());
							}
						}
					}
				}
				if (l_oSub.size() > 0)
				{
					int l_iX = m_oGood[i].x();
					int l_iY = m_oGood[i].y();
					if (l_bIsVertical)
					{
						l_oSub.append(l_iYMin - RADIUS);
						l_oSub.append(l_iYMax + RADIUS);
					}
					else
					{
						l_oSub.append(l_iXMin - RADIUS);
						l_oSub.append(l_iXMax + RADIUS);
					}
					std::sort(l_oSub.begin(), l_oSub.end());
					for (int k = 0; k < l_oSub.size() - 1; ++k)
					{
						int l_i1 = l_oSub.at(k) + RADIUS;
						int l_i2 = l_oSub.at(k + 1) - RADIUS;

						if (l_bIsVertical)
						{
							i_oPainter->drawLine(l_iX, l_i1, l_iX, l_i2);
							if (k > 0)
							{
								i_oPainter->drawArc(l_iX - RADIUS, l_i1 - 2 * RADIUS, 2 * RADIUS, 2 * RADIUS, 90*16, 180*16);
							}
						}
						else
						{
							i_oPainter->drawLine(l_i1, l_iY, l_i2, l_iY);
							if (k > 0)
							{
								i_oPainter->drawArc(l_i1 - 2 * RADIUS, l_iY - RADIUS, 2 * RADIUS, 2 * RADIUS, 0, 180*16);
							}
						}
					}
				}
				else
				{
					i_oPainter->drawLine(l_oLine);
				}
			}
			else
			{
				i_oPainter->drawLine(l_oLine);
			}
		}
	}

	int last = m_oGood.size() - 1;
	qreal dx = m_oGood[last].x() - m_oGood[0].x();
	qreal dy = m_oGood[last].y() - m_oGood[0].y();
	qreal tot = sqrt(dx * dx + dy * dy);
	if (tot > 0.01)
	{
		l_oPen.setStyle(Qt::SolidLine);
		i_oPainter->setPen(l_oPen);

		qreal ddx = dx / tot;
		qreal ddy = dy / tot;

		qreal cosphi = 0;
		qreal sinphi = 1;

		compute_angles(true, m_oInnerLink.m_iLineType, (data_link::Direction) (m_oInnerLink.m_iParentPos & data_link::COORD), &cosphi, &sinphi, ddx, ddy);
		if (m_oInnerLink.m_iLeftArrow)
		{
			QPolygonF l_oPol(3);
			l_oPol[1] = m_oGood[0];
			l_oPol[0] = m_oGood[0] + QPointF(yw * cosphi - xw * sinphi, yw * sinphi + xw * cosphi);
			l_oPol[2] = m_oGood[0] + QPointF(yw * cosphi + xw * sinphi, yw * sinphi - xw * cosphi);

			if (m_oInnerLink.m_iLeftArrow == data_link::INHERITANCE) {
				i_oPainter->setBrush(Qt::white);
			}

			if (m_oInnerLink.m_iLeftArrow == data_link::AGGREGATION) {
				i_oPainter->setBrush(Qt::white);
				l_oPol << m_oGood[0] + QPointF(2 * yw * cosphi, 2 * yw * sinphi);
			}

			if (m_oInnerLink.m_iLeftArrow == data_link::ASSOCIATION) {
				i_oPainter->drawPolyline(l_oPol);
			} else {
				i_oPainter->drawPolygon(l_oPol);
			}
		}

		compute_angles(false, m_oInnerLink.m_iLineType, (data_link::Direction) (m_oInnerLink.m_iChildPos & data_link::COORD), &cosphi, &sinphi, ddx, ddy);
		if (m_oInnerLink.m_iRightArrow)
		{
			QPolygonF l_oPol(3);
			l_oPol[1] = m_oGood[last];
			l_oPol[0] = m_oGood[last] + QPointF(yw * cosphi - xw * sinphi, yw * sinphi + xw * cosphi);
			l_oPol[2] = m_oGood[last] + QPointF(yw * cosphi + xw * sinphi, yw * sinphi - xw * cosphi);
			if (m_oInnerLink.m_iRightArrow == data_link::INHERITANCE) {
				i_oPainter->setBrush(Qt::white);
			}

			if (m_oInnerLink.m_iRightArrow == data_link::AGGREGATION) {
				i_oPainter->setBrush(Qt::white);
				l_oPol << m_oGood[last] + QPointF(2 * yw * cosphi, 2 * yw * sinphi);
			}

			if (m_oInnerLink.m_iRightArrow == data_link::ASSOCIATION) {
				i_oPainter->drawPolyline(l_oPol);
			} else {
				i_oPainter->drawPolygon(l_oPol);
			}
		}
	}
}

void box_link::update_pos()
{
	QRectF l_oR1, l_oR2;
	QPoint apos, bpos;

	connectable *l_oStart;
	if (( l_oStart = m_oView->m_oItems.value(m_oInnerLink.m_iParent)	))
	{
		m_oInnerLink.m_oStartPoint = apos = l_oStart->get_point(m_oInnerLink.m_iParentPos);
		l_oR1 = l_oStart->rectPos();
	}
	else
	{
		QPoint l_oP = m_oInnerLink.m_oStartPoint;
		l_oR1 = QRectF(l_oP - QPointF(1, 1), l_oP + QPointF(1, 1));
		switch (m_oInnerLink.m_iParentPos & data_link::COORD) {
			case data_link::NORTH:
				apos = QPoint(l_oP.x(), l_oP.y() - 1);
				break;
			case data_link::WEST:
				apos = QPoint(l_oP.x() - 1, l_oP.y());
				break;
			case data_link::SOUTH:
				apos = QPoint(l_oP.x(), l_oP.y() + 1);
				break;
			case data_link::EAST:
				apos = QPoint(l_oP.x() + 1, l_oP.y());
				break;
			default:
				Q_ASSERT(false);
		}
	}

	m_oStartPoint->force_position(m_oInnerLink.m_oStartPoint);

	connectable *l_oEnd;
	if ((l_oEnd = m_oView->m_oItems.value(m_oInnerLink.m_iChild))	)
	{
		m_oInnerLink.m_oEndPoint = bpos = l_oEnd->get_point(m_oInnerLink.m_iChildPos);
		l_oR2 = l_oEnd-> rectPos();
	}
	else
	{
		QPoint l_oP = m_oInnerLink.m_oEndPoint;
		l_oR2 = QRectF(l_oP - QPointF(1, 1), l_oP + QPointF(1, 1));
		switch (m_oInnerLink.m_iChildPos & data_link::COORD) {
			case data_link::NORTH:
				bpos = QPoint(l_oP.x(), l_oP.y() - 1);
				break;
			case data_link::WEST:
				bpos = QPoint(l_oP.x() - 1, l_oP.y());
				break;
			case data_link::SOUTH:
				bpos = QPoint(l_oP.x(), l_oP.y() + 1);
				break;
			case data_link::EAST:
				bpos = QPoint(l_oP.x() + 1, l_oP.y());
				break;
			default:
				Q_ASSERT(false);
		}

	}
	m_oEndPoint->force_position(m_oInnerLink.m_oEndPoint);

	int ax1 = (int) l_oR1.left();
	int ax2 = (int) l_oR1.right();
	int ay1 = (int) l_oR1.top();
	int ay2 = (int) l_oR1.bottom();

	int bx1 = (int) l_oR2.left();
	int bx2 = (int) l_oR2.right();
	int by1 = (int) l_oR2.top();
	int by2 = (int) l_oR2.bottom();

	hor.clear();
	ver.clear();
	graph.clear();
	prev.clear();
	dist.clear();
	m_oLst.clear();

	hor.append(apos.x());
	ver.append(apos.y());
	QPair<int,int> init_p(apos.x(), apos.y());

	hor.append(bpos.x());
	ver.append(bpos.y());
	QPair<int, int> end_p(bpos.x(), bpos.y());

	int aap = m_oInnerLink.m_iParentPos & data_link::COORD;
	int bbp = m_oInnerLink.m_iChildPos & data_link::COORD;

	if (aap == data_link::WEST && bbp == data_link::EAST && bx2 >= ax1 - pad)
	{
		hor.append(ax1 - pad);
		hor.append(bx2 + pad);
	}
	else if (aap == data_link::EAST && bbp == data_link::WEST && ax2 >= bx1 - pad)
	{
		hor.append(bx1 - pad);
		hor.append(ax2 + pad);
	}
	else if (aap == data_link::NORTH && bbp == data_link::SOUTH && by2 >= ay1 - pad)
	{
		ver.append(ay1 - pad);
		ver.append(by2 + pad);
	}
	else if (aap == data_link::SOUTH && bbp == data_link::NORTH && ay2 >= by1 - pad)
	{
		ver.append(by1 - pad);
		ver.append(ay2 + pad);
	}

	hor.append(qMin(ax1, bx1) - pad);
	hor.append(qMax(ax2, bx2) + pad);
	ver.append(qMin(ay1, by1) - pad);
	ver.append(qMax(ay2, by2) + pad);

	if (ax1 > bx2 + pad)
	{
		hor.append((ax1 + bx2) / 2);
	}
	if (bx1 > ax2 + pad)
	{
		hor.append((ax2 + bx1) / 2);
	}
	if (ay1 > by2 + pad)
	{
		ver.append((ay1 + by2) / 2);
	}
	if (by1 > ay2 + pad)
	{
		ver.append((ay2 + by1) / 2);
	}

	foreach (int x, hor) {
		foreach (int y, ver) {
			QPair<int, int> p(x, y);

			graph << p;
			dist[p] = MAX;
			prev[p] = init_p;
		}
	}
	dist[init_p] = 0;

	while (graph.size())
	{
		int mi = MAX;
		QPair<int, int> cand;
		QPair<int, int> p;
		bool ok = false;
		foreach (p, graph) {
			if (dist[p] < mi) {
				mi = dist[p];
				cand = p;
				ok = true;
			}
		}

		if (!ok) {
			m_oLst.clear();
			m_oLst.append(QPoint(init_p.first, init_p.second));
			m_oLst.append(QPoint(end_p.first, end_p.second));
			break;
		}

		graph.remove(cand);
		if (cand == end_p) {
			QPair<int, int> cand = end_p;
			while (cand != init_p) {
				m_oLst.prepend(QPoint(cand.first, cand.second));
				cand = prev[cand];
			}
			m_oLst.prepend(QPoint(cand.first, cand.second));
			break;
		}

		foreach (p, graph) {
			if (p.first == cand.first || p.second == cand.second)
			{
				if (may_use(cand, p, l_oStart, l_oR1, m_oInnerLink.m_iParentPos, apos) && may_use(cand, p, l_oEnd, l_oR2, m_oInnerLink.m_iChildPos, bpos))
				{
					int newdist = dist[cand] + qAbs(p.first - cand.first) + qAbs(p.second - cand.second) + DAMP;
					if (newdist < dist[p])
					{
						dist[p] = newdist;
						prev[p] = cand;
					}
				}
			}
		}
	}
	update_ratio();
	update();
	update_text_pos();
}

int box_link::may_use(const QPair<int, int> i_oA, const QPair<int, int> i_oB, const connectable *con, const QRectF l_oR, const int i_iPos, const QPoint i_oP)
{
	if (con) {
		int ret =  con->may_use(i_oA, i_oB, i_iPos, i_oP);
		return ret;
	}
	else
	{
		int cx1 = qMin(i_oA.first, i_oB.first);
		int cx2 = qMax(i_oA.first, i_oB.first);
		int cy1 = qMin(i_oA.second, i_oB.second);
		int cy2 = qMax(i_oA.second, i_oB.second);

		int ax1 = (int) l_oR.left();
		int ax2 = (int) l_oR.right();
		int ay1 = (int) l_oR.top();
		int ay2 = (int) l_oR.bottom();
		return \
			(cx1 >= ax2 && cx2 > ax2) ||
			(cx2 <= ax1 && cx1 < ax1) ||
			(cy1 >= ay2 && cy2 > ay2) ||
			(cy2 <= ay1 && cy1 < ay1);
	}
}

void box_link::update_offset(const QPointF& i_oP, int i_iIdx)
{
	if (m_bReentrantLock) return;
	m_bReentrantLock = true;

	update_ratio();
	for (int i = 0; i < m_oLst.size() - 3; ++i)
	{
		box_control_point * b = m_oControlPoints.at(i);
		if (i != i_iIdx) {
			b->init_pos();
		}
	}
	m_bReentrantLock = false;
	update();
}

void box_link::update_ratio()
{
	// here we reset the offsets if necessary
	int ret = m_oLst.size();

	if (m_oGood.size() != ret)
	{
		m_oGood.clear();
		for (int i=0; i<ret; ++i) m_oGood.append(m_oLst[i]);
	}
	else
	{
		for (int i=0; i<ret; ++i) m_oGood[i] = m_oLst[i];
	}

	if (m_oInnerLink.m_oOffsets.size() != ret - 3)
	{
		m_oInnerLink.m_oOffsets.clear();
		for (int i=1; i < ret - 2; ++i)
		{
			m_oInnerLink.m_oOffsets.append(QPoint(0, 0));
		}
	}

	for (int i=0; i < m_oInnerLink.m_oOffsets.size(); ++i)
	{
		if (m_oLst[i+1].x() == m_oLst[i+2].x())
		{
			int v = m_oInnerLink.m_oOffsets[i].x() + m_oLst[i+1].x();
			v = int_val2(v);
			m_oGood[i+1].setX(v);
			m_oGood[i+2].setX(v);
		}
		else if (m_oLst[i+1].y() == m_oLst[i+2].y())
		{
			int v = m_oInnerLink.m_oOffsets[i].y() + m_oLst[i+1].y();
			v = int_val2(v);
			m_oGood[i+1].setY(v);
			m_oGood[i+2].setY(v);
		}
	}

	int mx1, mx2, my1, my2;
	mx1 = mx2 = m_oGood[0].x();
	my1 = my2 = m_oGood[0].y();

	for (int i = 1; i < ret; ++i)
	{
		int nx = m_oGood[i].x(), ny = m_oGood[i].y();
		mx1 = qMin(mx1, nx);
		mx2 = qMax(mx2, nx);
		my1 = qMin(my1, ny);
		my2 = qMax(my2, ny);
	}

	// add a padding for the borders
	mx1 -= min_offset;
	mx2 += min_offset;
	my1 -= min_offset;
	my2 += min_offset;

	// now we have the size
	setRect(QRectF(mx1, my1, qAbs(mx2 - mx1), qAbs(my2 - my1)));

	for (int i = 0; i < m_oControlPoints.size(); ++i)
	{
		box_control_point * b = m_oControlPoints.at(i);
		if (i < m_oGood.size() - 3)
		{
			b->init_pos();
			if (isSelected() && !m_oInnerLink.m_iLineType)
			{
				b->show();
			}
			else
			{
				b->hide();
			}
		}
		else
		{
			b->hide();
		}
	}

	QPainterPath p;
	if (m_oInnerLink.m_iLineType)
	{
		if (m_oGood.size() > 0)
		{
			double ax = m_oGood[0].x();
			double ay = m_oGood[0].y();
			double bx = m_oGood[m_oGood.size() - 1].x();
			double by = m_oGood[m_oGood.size() - 1].y();
			double coeff = 5. / sqrt((by - ay) * (by - ay) + (bx - ax) * (bx - ax));

			QPolygonF l_oPol(4);
			l_oPol[0].setX(ax - coeff * (by - ay));
			l_oPol[0].setY(ay - coeff * (ax - bx));
			l_oPol[1].setX(ax + coeff * (by - ay));
			l_oPol[1].setY(ay + coeff * (ax - bx));
			l_oPol[2].setX(bx + coeff * (by - ay));
			l_oPol[2].setY(by + coeff * (ax - bx));
			l_oPol[3].setX(bx - coeff * (by - ay));
			l_oPol[3].setY(by - coeff * (ax - bx));
			p.addPolygon(l_oPol);
		}
	}
	else
	{
		for (int i = 0; i < m_oGood.size() - 1; ++i)
		{
			int x1 = qMin(m_oGood[i].x(), m_oGood[i+1].x());
			int x2 = qMax(m_oGood[i].x(), m_oGood[i+1].x());
			int y1 = qMin(m_oGood[i].y(), m_oGood[i+1].y());
			int y2 = qMax(m_oGood[i].y(), m_oGood[i+1].y());

			if (x1 == x2)
			{
				p.addRect(x1 - 5, y1, 10, y2 - y1);
			}
			else
			{
				p.addRect(x1, y1 - 5, x2 - x1, 10);
			}
		}
	}

	inner_shape = p;
}

QPainterPath box_link::shape() const
{
	return inner_shape;
}

QVariant box_link::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemSelectedHasChanged)
		{
			if (isSelected())
			{
				setZValue(102);
				m_oStartPoint->show();
				m_oEndPoint->show();
			}
			else
			{
				setZValue(98);
				m_oStartPoint->hide();
				m_oEndPoint->hide();
			}
			update_pos();
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

void box_link::update_text_pos() {
	const QPointF l_oStartPos = m_oStartPoint->pos();
	const QPointF l_oEndPos = m_oEndPoint->pos();

	const QRectF m_oStartRect = m_oStartCaption->boundingRect();
	const QRectF m_oEndRect = m_oEndCaption->boundingRect();

	const int l_iStartDir = m_oInnerLink.m_iParentPos & data_link::COORD;
	const int l_iEndDir = m_oInnerLink.m_iChildPos & data_link::COORD;
	switch (l_iStartDir) {
		case data_link::NORTH:
			if (l_oStartPos.x() < l_oEndPos.x()) {
				m_oStartCaption->setPos(l_oStartPos + QPointF(- m_oStartRect.width() - 5, - m_oStartRect.height()));
			} else {
				m_oStartCaption->setPos(l_oStartPos + QPointF(5, - m_oStartRect.height()));
			}
			break;
		case data_link::WEST:
			if (l_oStartPos.y() < l_oEndPos.y()) {
				m_oStartCaption->setPos(l_oStartPos + QPointF(- m_oStartRect.width() - 2, - m_oStartRect.height() - 3));
			} else {
				m_oStartCaption->setPos(l_oStartPos + QPointF(- m_oStartRect.width() - 2, 2));
			}
			break;
		case data_link::SOUTH:
			if (l_oStartPos.x() < l_oEndPos.x()) {
				m_oStartCaption->setPos(l_oStartPos + QPointF(- m_oStartRect.width() - 5, 0));
			} else {
				m_oStartCaption->setPos(l_oStartPos + QPointF(5, 0));
			}
			break;
		case data_link::EAST:
			if (l_oStartPos.y() < l_oEndPos.y()) {
				m_oStartCaption->setPos(l_oStartPos + QPointF(2, - m_oStartRect.height() - 3));
			} else {
				m_oStartCaption->setPos(l_oStartPos + QPointF(2, 2));
			}
			break;
		default:
			Q_ASSERT(false);
	}

	switch (l_iEndDir) {
		case data_link::NORTH:
			if (l_oStartPos.x() > l_oEndPos.x()) {
				m_oEndCaption->setPos(l_oEndPos + QPointF(- m_oEndRect.width() - 5, - m_oEndRect.height()));
			} else {
				m_oEndCaption->setPos(l_oEndPos + QPointF(5, - m_oEndRect.height()));
			}
			break;
		case data_link::WEST:
			if (l_oStartPos.y() > l_oEndPos.y()) {
				m_oEndCaption->setPos(l_oEndPos + QPointF(- m_oEndRect.width() - 2, - m_oEndRect.height() - 3));
			} else {
				m_oEndCaption->setPos(l_oEndPos + QPointF(- m_oEndRect.width() - 2, 2));
			}
			break;
		case data_link::SOUTH:
			if (l_oStartPos.x() > l_oEndPos.x()) {
				m_oEndCaption->setPos(l_oEndPos + QPointF(- m_oEndRect.width() - 5, 0));
			} else {
				m_oEndCaption->setPos(l_oEndPos + QPointF(5, 0));
			}
			break;
		case data_link::EAST:
			if (l_oStartPos.y() > l_oEndPos.y()) {
				m_oEndCaption->setPos(l_oEndPos + QPointF(2, - m_oEndRect.height() - 3));
			} else {
				m_oEndCaption->setPos(l_oEndPos + QPointF(2, 2));
			}
			break;
		default:
			Q_ASSERT(false);
	}
	update_text_label_pos();
}

void box_link::update_text_label_pos()
{
	const QPointF l_oStartPos = m_oStartPoint->pos();
	const QPointF l_oEndPos = m_oEndPoint->pos();

	const int l_iStartDir = m_oInnerLink.m_iParentPos & data_link::COORD;
	const int l_iEndDir = m_oInnerLink.m_iChildPos & data_link::COORD;

	const QRectF l_oMidRect = m_oMidCaption->boundingRect();
	QPointF mid = (l_oStartPos + l_oEndPos)/2. + QPointF(5, 2);

	if (m_oInnerLink.m_iLineType)
	{
		double d_r_x = (10 + l_oMidRect.width()) / 2.;
		double d_r_y = (6 + l_oMidRect.height()) / 2.;
		double d_r = sqrt(d_r_x * d_r_x + d_r_y * d_r_y);

		double d_a_x = qAbs(l_oStartPos.x() - l_oEndPos.x());
		double d_a_y = qAbs(l_oStartPos.y() - l_oEndPos.y());
		double d_a = sqrt(d_a_x * d_a_x + d_a_y * d_a_y);

		double cos_a = d_a_x / d_a;
		double sin_a = d_a_y / d_a;

		double cos_r = d_r_x / d_r;
		double sin_r = d_r_y / d_r;

		double d = d_r * (sin_a * cos_r + cos_a * sin_r);
		double dx = d_r_x - d * sin_a;
		double dy = d * cos_a;

		double ddx = 5 - dx;
		double ddy = - d_r_y + 3 - d * cos_a;

		if ((l_oStartPos.x() < l_oEndPos.x() && l_oStartPos.y() > l_oEndPos.y()) || (l_oStartPos.x() > l_oEndPos.x() && l_oStartPos.y() < l_oEndPos.y()))
		{
			ddx = - l_oMidRect.width() - 5 + dx;
		}

		if (!std::isnan(dx) && !std::isnan(dy))
		{
			mid = (l_oStartPos + l_oEndPos)/2. + QPointF(ddx, ddy);
		}
	}
	else
	{
		if ((l_oStartPos.x() == l_oEndPos.x()) && (
			(l_iStartDir == data_link::NORTH && l_iEndDir == data_link::SOUTH) || (l_iStartDir == data_link::SOUTH && l_iEndDir == data_link::NORTH)))
		{
			mid = (l_oStartPos + l_oEndPos)/2. + QPointF(5, - l_oMidRect.height() / 2.);
		}
		else if ((l_oStartPos.y() == l_oEndPos.y()) && (
			(l_iStartDir == data_link::EAST && l_iEndDir == data_link::WEST) || (l_iStartDir == data_link::WEST && l_iEndDir == data_link::EAST)))
		{
			mid = (l_oStartPos + l_oEndPos)/2. + QPointF(- l_oMidRect.width() / 2., 2);
		}
		else if (m_oGood.size() == 3)
		{
			if (m_oControlPoints.at(0)->h_length() == 0)
			{
				if (l_oStartPos.y() > l_oEndPos.y())
				{
					mid = QPointF(mid.x(), l_oStartPos.y()) + QPointF(- l_oMidRect.width() / 2., 0);
				}
				else
				{
					mid = QPointF(mid.x(), l_oStartPos.y()) + QPointF(- l_oMidRect.width() / 2., - l_oMidRect.height() - 3);
				}
			}
			else
			{
				if (l_oStartPos.y() < l_oEndPos.y())
				{
					mid = QPointF(mid.x(), l_oEndPos.y()) + QPointF(- l_oMidRect.width() / 2., 0);
				}
				else
				{
					mid = QPointF(mid.x(), l_oEndPos.y()) + QPointF(- l_oMidRect.width() / 2., - l_oMidRect.height() - 3);
				}
			}
		}
		else if (m_oGood.size() == 4)
		{
			if (m_oControlPoints.at(0)->h_length() == 0)
			{
				if (l_iStartDir == data_link::WEST && l_iEndDir == data_link::WEST)
				{
					mid = m_oControlPoints.at(0)->pos() + QPointF(- l_oMidRect.width() - 2, - l_oMidRect.height() / 2.);
				}
				else if (l_iStartDir == data_link::EAST && l_iEndDir == data_link::EAST)
				{
					mid = m_oControlPoints.at(0)->pos() + QPointF(2, - l_oMidRect.height() / 2.);
				}
				else
				{
					mid = m_oControlPoints.at(0)->pos() + QPointF(2, - l_oMidRect.height() / 2.);
					if (l_oStartPos.y() > l_oEndPos.y())
					{

						if (l_oStartPos.x() < l_oEndPos.x())
							mid.setY(l_oEndPos.y() + 2);
						else
							mid.setY(l_oStartPos.y() - l_oMidRect.height() - 2.);
					}
					else if (l_oStartPos.y() < l_oEndPos.y())
					{
						if (l_oStartPos.x() < l_oEndPos.x())
							mid.setY(l_oEndPos.y() - l_oMidRect.height() - 2.);
						else
							mid.setY(l_oStartPos.y() + 2);
					}
				}
			}
			else
			{
				if (l_iStartDir == data_link::NORTH && l_iEndDir == data_link::NORTH)
				{
					mid = m_oControlPoints.at(0)->pos() + QPointF(- l_oMidRect.width()/2. - 2., - l_oMidRect.height() - 2.);
				}
				else if (l_iStartDir == data_link::SOUTH && l_iEndDir == data_link::SOUTH)
				{
					mid = m_oControlPoints.at(0)->pos() + QPointF(- l_oMidRect.width()/2. - 2., 2.);
				}
				else
				{
					mid = m_oControlPoints.at(0)->pos() + QPointF(- l_oMidRect.width()/2. - 2., 2.);

					if (qAbs(l_oStartPos.x() - l_oEndPos.x()) < l_oMidRect.width() + 2.)
					{
						if (l_oStartPos.x() > l_oEndPos.x())
						{
							if (l_oStartPos.y() < l_oEndPos.y())
							{
								mid.setX(l_oEndPos.x() + 2.);
							}
							else
							{
								mid.setX(l_oStartPos.x() - l_oMidRect.width() - 2.);
							}
						}
						else if (l_oStartPos.x() < l_oEndPos.x())
						{
							if (l_oStartPos.y() < l_oEndPos.y())
							{
								mid.setX(l_oEndPos.x() - l_oMidRect.width() - 2.);
							}
							else
							{
								mid.setX(l_oStartPos.x() + 2.);
							}
						}
					}
				}
			}
		}
		else
		{
			int l_iHlen = 0;
			box_control_point *l_oCandidate = NULL;

			foreach (box_control_point *b, m_oControlPoints)
			{
				if (b->m_iOffset >= m_oGood.size() - 3)
				{
					break;
				}

				const int l_iCurLen = b->h_length();
				if (l_iCurLen >= l_iHlen)
				{
					l_iHlen = l_iCurLen;
					l_oCandidate = b;
				}
			}
			if (l_oCandidate)
			{
				mid = l_oCandidate->pos() + QPointF(- l_oMidRect.width() / 2., 2);
			}
		}
	}
	m_oMidCaption->setPos(mid);
}

void box_link::update_text()
{
	m_oMidCaption->setFont(scene()->font());
	m_oMidCaption->setPlainText(m_oInnerLink.m_sCaption);
	m_oStartCaption->setFont(scene()->font());
	m_oStartCaption->setPlainText(m_oInnerLink.m_sParentCaption);
	m_oEndCaption->setPlainText(m_oInnerLink.m_sChildCaption);
	m_oEndCaption->setFont(scene()->font());
}

void box_link::properties()
{
	box_link_properties props(m_oView, this);
	props.m_oThickness->setValue(m_oInnerLink.border_width);
	props.m_oType->setCurrentIndex(m_oInnerLink.m_iLineType);
	props.m_oStyle->setCurrentIndex((int) m_oInnerLink.pen_style);
	props.m_oLeftArrow->setCurrentIndex(m_oInnerLink.m_iLeftArrow);
	props.m_oRightArrow->setCurrentIndex(m_oInnerLink.m_iRightArrow);
	props.m_oArrowEdit->setText(m_oInnerLink.m_sCaption);
	props.m_oLeftArrowEdit->setText(m_oInnerLink.m_sParentCaption);
	props.m_oRightArrowEdit->setText(m_oInnerLink.m_sChildCaption);
	props.exec();
}

