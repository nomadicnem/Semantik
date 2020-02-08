// Thomas Nagy 2018-2020 GPLV3

#ifndef CANVAS_REF_H
#define CANVAS_REF_H

#include  <QGraphicsPathItem>
#include  <QList>
#include <canvas_link.h>
#include  "con.h"

class QColor;
class QString;
class QGraphicsSceneMouseEvent;
class canvas_view;
class canvas_item;

class canvas_ref: public canvas_link
{
	public:
		canvas_ref(canvas_view *i_oGraphWidget, canvas_item *i_oFrom, canvas_item * i_oTo);

		int type() const { return CANVAS_REF_T; }
		void paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *i_oStyle, QWidget *i_oWidget=NULL);
		QColor get_color();
};

#endif
