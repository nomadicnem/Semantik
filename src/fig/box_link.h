// Thomas Nagy 2007-2019 GPLV3


#ifndef BOX_LINK_H
#define BOX_LINK_H

#include <QGraphicsRectItem>
#include <QRectF>
#include <QPair>
#include <QSet>
#include <QPainterPath>
#include "data_item.h"
#include "ed:table.h"
#include "con.h"
#include "CON.h"

class QGraphicsTextItem;
class box_view;
class box_item;
class box_control_point;
class box_link : public QGraphicsRectItem, public editable
{
	public:
		box_link(box_view*);
		~box_link();

		QPointF m_oMediatorPoint;

		data_link *m_oLink;
		data_link m_oInnerLink;

		box_view *m_oView;

		void update_pos();
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		QPainterPath shape() const;
		QPainterPath inner_shape;

		QList<box_control_point*> m_oControlPoints;
		box_control_point* m_oStartPoint;
		box_control_point* m_oEndPoint;
		QGraphicsTextItem *m_oStartCaption;
		QGraphicsTextItem *m_oEndCaption;
		QGraphicsTextItem *m_oMidCaption;

		QList<QPoint> m_oGood;
		QList<QPoint> m_oLst;

		void update_text();
		void update_text_pos();
		void update_text_label_pos();
		void update_offset(const QPointF& i_oP, int i_iIdx);
		void update_ratio();

		void properties();

		bool m_bReentrantLock;

		// This shows how to abuse the qt containers :-)
		QList<int> ver;
		QList<int> hor;
		QSet<QPair<int, int> > graph;
		QHash<QPair<int, int>, QPair<int, int> > prev;
		QHash<QPair<int, int>, int> dist;
		int may_use(const QPair<int, int>, const QPair<int, int>, const connectable*, const QRectF, const int, const QPoint);
};

#endif // BOX_LINK_H

