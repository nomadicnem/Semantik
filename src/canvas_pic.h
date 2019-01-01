// Thomas Nagy 2007-2019 GPLV3

 #ifndef CANVAS_PIC_H
#define CANVAS_PIC_H

#include "con.h" 

#include <QPixmap>
#include <QGraphicsItem>
#include <QList>

#define OFF 3
#define PAD 1

class QTextDocument;
class QColor;
class QString;
class QSvgRenderer;
 class QGraphicsSceneMouseEvent;
class canvas_view;
class canvas_link;
class canvas_sort;
class canvas_flag;
class canvas_chain;
class box_view;
class canvas_sort_toggle;

class canvas_pic: public QGraphicsRectItem
{
	public:
		canvas_pic(canvas_view *i_oGraphWidget, int i_iId);
		~canvas_pic();

		int type() const { return CANVAS_PIC_T; }
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

		canvas_view * m_oGraph;
		QPixmap m_oPixmap;
		box_view *m_oBoxView;
		int m_iId;
		int m_iOffset;





































};

#endif

