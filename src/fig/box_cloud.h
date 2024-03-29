// Thomas Nagy 2020-2021 GPLV3

#ifndef BOX_CLOUD_H
#define BOX_CLOUD_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class QGraphicsTextItem;
class box_cloud : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_cloud(box_view*, int i_iId);
		~box_cloud();
		void properties();
};

#endif // BOX_CLOUD_H

