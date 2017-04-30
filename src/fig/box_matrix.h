// Thomas Nagy 2007-2017 GPLV3

#ifndef BOX_MATRIX_H
#define BOX_MATRIX_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include <box_resize_point.h>
#include "CON.h"
#include "con.h"

#define OFF 3

class box_matrix : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_matrix(box_view*, int i_iId);
		void update_size();
		void properties();

		int m_iLastSize;
		int m_iMovingRow;
		int m_iMovingCol;

		void update_selection();

		void fix_sizers_visibility();
		void allocate_sizers();
		void update_sizers();
		QPointF validate_point(box_resize_point *p, const QPointF & orig);
		void freeze(bool b);
		void commit_size(box_resize_point *p);
		QList<box_resize_point*> m_oRowSizers;
		QList<box_resize_point*> m_oColSizers;
};

#endif // BOX_MATRIX_H

