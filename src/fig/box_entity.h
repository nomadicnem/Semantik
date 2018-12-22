// Thomas Nagy 2018 GPLV3

#ifndef BOX_ENTITY_H
#define BOX_ENTITY_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_entity : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		box_entity(box_view*, int i_iId);
		QSizeF size();
		void force_size();

		void properties();
};

#endif // BOX_CLASS_H

