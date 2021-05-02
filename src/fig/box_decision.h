// Thomas Nagy 2007-2021 GPLV3

#ifndef BOX_DECISION_H
#define BOX_DECISION_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_decision : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_decision(box_view*, int i_iId);
		QSize best_size(const QPointF &dims);
		int linesCount(const QString& i_sInput, int i_iNumLines, qreal i_fWidth, qreal i_fHeight);
		QString m_sLastText;
		qreal m_fLastLineHeight;
		qreal m_fLastWidth;
		qreal m_fLastHeight;
		 int m_iLastLineCount;
		QSize best_size_for(const QString &);
};

#endif // BOX_DECISION_H

