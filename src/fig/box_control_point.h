// Thomas Nagy 2007-2019 GPLV3

#ifndef BOX_CTRL_H
#define BOX_CTRL_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "CON.h"
#include "con.h"

class box_view;
class data_item;
class data_box;
class box_link;
class box_control_point : public QGraphicsRectItem
{
	public:
		box_control_point(box_view*);
		bool m_bIsSegment;
		bool m_bMoveX;

		int m_iOffset;
		box_link *m_oLink;
		box_view *m_oView;

		void init_pos();
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		QPoint m_oRealPosition;
		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

		void force_position(const QPoint&);
		int h_length();

	private:
		bool m_bChanged;
		bool m_bForced;
};

#endif // BOX_CTRL_H

