// Thomas Nagy 2017-2017 GPLV3

#ifndef BOX_PIPE_H
#define BOX_PIPE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_pipe : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_pipe(box_view*, int i_iId);
		~box_pipe();
		void update_links();
		QGraphicsTextItem *m_oCaption;
};

#endif // BOX_PIPE_H

