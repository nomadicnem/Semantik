// Thomas Nagy 2007-2018 GPLV3


#ifndef BOX_ITEM_H
#define BOX_ITEM_H

#include <QGraphicsRectItem>
#include <QLineEdit>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_resize_point.h"
#include "box_chain.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class QTextDocument;
class box_view;
class data_item;
class data_box;
class box_item : public QGraphicsRectItem, public connectable, public editable, public resizable
{
	public:
		box_item(box_view*, int i_iId);
		~box_item();
		int y_text_off;
		int x_text_off;

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		QRectF rectPos() const { return QGraphicsRectItem::rect().translated(pos()); }
		//void setRect(int, int, int, int) { }

		QTextDocument doc;

		box_view *m_oView;
		data_item *m_oItem;

		virtual void properties();

		virtual void update_data();
		virtual void update_size();
		virtual void update_links();
		virtual void update_selection();

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		int choose_position(const QPointF&p, box_link*);
		QPoint get_point(int position);
		QPointF m_oLastPressPoint;

		int m_iWW;
		int m_iHH;

		box_resize_point *m_oResize;
		virtual QSize best_size(const QPointF & orig);

		virtual void update_sizers();
		int m_iLastStretchX;
		int m_iLastStretchY;
		QPointF validate_point(box_resize_point *p, const QPointF & orig);
                void freeze(bool b);
                void commit_size(box_resize_point *p);
};

#endif // BOX_ITEM_H

