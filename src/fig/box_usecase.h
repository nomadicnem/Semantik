// Thomas Nagy 2007-2021 GPLV3

#ifndef BOX_USECASE_H
#define BOX_USECASE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_usecase : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_usecase(box_view*, int i_iId);
		int linesCount(const QString &, int, qreal, qreal);
		QString m_sLastText;
		qreal m_fLastLineHeight;
		qreal m_fLastWidth;
		qreal m_fLastHeight;
		 int m_iLastLineCount;
		QSize best_size(const QPointF & orig);
		QSize best_size_for(const QString &);
};

#endif // BOX_USECASE_H

