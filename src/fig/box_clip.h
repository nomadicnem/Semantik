// Thomas Nagy 2020-2020 GPLV3

#ifndef BOX_CLIP_H
#define BOX_CLIP_H

#include <QGraphicsRectItem>
#include <QSvgRenderer>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class QGraphicsTextItem;
class box_clip : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_clip(box_view*, int i_iId);
		~box_clip();
		QGraphicsTextItem *m_oCaption;
		QSvgRenderer m_oRenderer;
		void properties();
		void update_size();
		void update_links();
};

#endif // BOX_CLIP_H

