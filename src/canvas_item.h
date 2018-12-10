// Thomas Nagy 2007-2018 GPLV3



#ifndef CANVAS_ITEM_H
#define CANVAS_ITEM_H

#include "con.h" 

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
class canvas_pic;
class canvas_sort_toggle;

class canvas_item: public QGraphicsTextItem
{
	public:
		canvas_item(canvas_view *i_oGraphWidget, int i_iId);
		~canvas_item();

		int type() const { return CANVAS_ITEM_T; }
		QRectF boundingRect() const;

		void adjustSize();
		void update_flags();

		QList<canvas_link*> m_oLinks;
		void add_link(canvas_link*);
		void rm_link(canvas_link*);

		/*int get_id() const {return m_iID;}
		  QString get_label() {return m_sLabel;}

		  void set_ID(int i) {m_iID=i;}
		 */

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);
		QPainterPath opaqueArea() const;

		QColor get_color() {return m_oColor;}
		void set_color(QColor c) {m_oColor=c;}

		void set_parent(canvas_item *);
		//void adjustSize();

		int Id() const {return m_iId;};

		bool m_bEdit;
		int m_iNum;

		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
		void keyPressEvent(QKeyEvent*);
		void keyReleaseEvent(QKeyEvent*);
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

		static QColor s_oColorRect;
		static QColor s_oColorRoot;
		static QColor s_oColorLeaf;
		static QColor s_oColorSelect;
		static QColor s_oColorRectEd;

		void update_data();
		void update_links();
		void update_color();

		QList<canvas_flag*> m_oFlags;

		inline qreal const x2() {return x() + boundingRect().width();}
		inline qreal const y2() {return y() + boundingRect().height();}

		qreal reorgHeight();

		canvas_view *m_oGraph;
		canvas_item *m_oParent;
		canvas_sort *m_oSort;
		canvas_chain *m_oChain;
		canvas_sort_toggle *m_oSortToggle;
		 canvas_pic*  m_oPic;

		int m_iId;
		QColor m_oColor;
		QColor m_oColorBackup;
		QString m_sLabel;

		//QSvgRenderer *m_oRenderer;
};

#endif

