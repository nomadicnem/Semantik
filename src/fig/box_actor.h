// Thomas Nagy 2007-2017 GPLV3

#ifndef BOX_ACTOR_H
#define BOX_ACTOR_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class QGraphicsTextItem;
class box_actor : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_actor(box_view*, int i_iId);
		~box_actor();
		QGraphicsTextItem *m_oCaption;
		void properties();
		void update_size();
		void update_links();
};

#endif // BOX_ACTOR_H

