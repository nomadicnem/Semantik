// Thomas Nagy 2017-2017 GPLV3

#ifndef BOX_RECTANGLE_H
#define BOX_RECTANGLE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_rectangle : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_rectangle(box_view*, int i_iId);

};

#endif // BOX_RECTANGLE_H

