// Thomas Nagy 2017-2018 GPLV3

#ifndef BOX_SEQUENCE_H
#define BOX_SEQUENCE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
 #include 	<QPainterPath>
#include "box_item.h"
#include <box_resize_point.h>
#include "CON.h"
#include "con.h"

class box_sequence : public box_item
{
	public:
		void paint(QPainter *, const QStyleOptionGraphicsItem*, QWidget*);
		box_sequence(box_view*, int);
		void update_size();

		box_resize_point *m_oResizeStick;

		QPainterPath shape() const;
		QPainterPath inner_shape;

		int choose_position(const QPointF&, box_link*);
		QPoint get_point(int);
		void update_data();
		void update_selection();
		void fix_sizers_visibility();
		void allocate_connectors();

		void update_sizers();
		QPointF validate_point(box_resize_point *, const QPointF &);
		void freeze(bool);
		void commit_size(box_resize_point *);
		int optimize_position(const QPointF& i_o);

		int m_iBoxHeight;
};

#endif // BOX_SEQUENCE_H

