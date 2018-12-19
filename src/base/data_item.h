// Thomas Nagy 2007-2018 GPLV3

#ifndef DATA_ITEM_H
#define DATA_ITEM_H

#include <QPair>
#include <QPixmap>
#include <QList>
#include <QPoint>
#include <QMap>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QXmlDefaultHandler>

#include "color_scheme.h"
#include "con.h"

class sem_mediator;
class node { // so we can read/write a tree of objects
	public:
	node();
	virtual ~node();
	virtual node& make_node(const QString&, const QXmlAttributes&);
	virtual void read_data(const QString&, const QXmlAttributes&);
	virtual void dump_xml(QStringList & other);

	QList<node> m_oChildren;
};

class diagram_item
{
	public:
	diagram_item();

	void setBorderWidth(int);
	void setPenStyle(Qt::PenStyle);
	void setColor(QColor);
	const QColor& getColor(sem_mediator*) const;

	int border_width;
	Qt::PenStyle pen_style;

	color_scheme m_oCustom;
	//QColor m_oCustomColor;
	int m_iColor;
};

class data_link : public diagram_item
{
	public:
	enum Direction { NORTH=1, WEST=2, SOUTH=4, EAST=8, COORD=15 };
	enum Arrow { NONE=0, TRIANGLE=1, ASSOCIATION=2, INHERITANCE=3, AGGREGATION=4};
	enum LineType {ZIGZAG=0, LINE=1};

	data_link();
	QString m_sLink;
	QString m_sCaption;
	QString m_sParentCaption;
	QString m_sChildCaption;

	int m_iId;
	int m_iParent;
	int m_iChild;
	int m_iParentPos;
	int m_iChildPos;

	Arrow m_iLeftArrow;
	Arrow m_iRightArrow;
	LineType m_iLineType;

	QPoint m_oStartPoint;
	QPoint m_oEndPoint;
	QList<QPoint> m_oOffsets;

	void copy_from(const data_link&);
	bool equals(const data_link&);
};

class visibility
{
	public:
		enum VisibilityType {PRIVATE=1, PUBLIC=2, PROTECTED=3, PACKAGE=4, DERIVED=5};
		static VisibilityType toVisibility(const QString& i_s)
		{
			if (i_s == notr("public"))
			{
				return PUBLIC;
			}
			if (i_s == notr("protected"))
			{
				return PROTECTED;
			}
			if (i_s == notr("private"))
			{
				return PRIVATE;
			}
			if (i_s == notr("derived"))
			{
				return DERIVED;
			}
			return PACKAGE;
		}
};

class data_box_method : public node
{
	public:
		QString m_sText;
		visibility::VisibilityType m_oVisibility;
		bool m_bStatic;
		bool m_bAbstract;

		virtual void read_data(const QString&, const QXmlAttributes&);
		virtual void dump_xml(QStringList & other);
};

class data_box_attribute : public node
{
	public:
		QString m_sText;
		visibility::VisibilityType m_oVisibility;
		bool m_bStatic;

		virtual void read_data(const QString&, const QXmlAttributes&);
		virtual void dump_xml(QStringList & other);
};

class data_box : public diagram_item, public node
{
	public:
	data_box(int id);
	~data_box();

	data_box& operator=(const data_box &);

	int m_iId;
	Qt::AlignmentFlag m_iAlign;
	QString m_sText;
	int m_iXX;
	int m_iYY;
	int m_iWW;
	int m_iHH;
	enum IType { ACTIVITY=1, ACTIVITY_START=2, ACTIVITY_PARALLEL=3, LABEL=4, COMPONENT=5, NODE=6, \
		ACTOR=7, USECASE=8, DECISION=9, MATRIX=10, FRAME=11, CLASS=12, CAPTION=13, DATABASE=14,
                RECTANGLE=15, PIPE=16, SEQUENCE=17};
	data_box::IType m_iType;

	node& make_node(const QString&, const QXmlAttributes&);
	void read_data(const QString&, const QXmlAttributes&);
	void dump_xml(QStringList & other);

	// activity
	bool m_bIsEnd;

	// fork
	bool m_bIsVertical;

	// matrix
	QList<int> m_oRowSizes;
	QList<int> m_oColSizes;

	int m_iBoxHeight;

	// frame
	Qt::Edge m_iLabelPosition;

	// class
	bool m_bStatic;
	bool m_bAbstract;
	QString m_sStereotype;
	QList<data_box_method> m_oMethods;
	QList<data_box_attribute> m_oAttributes;
};

class data_pic
{
	public:
	QPixmap m_oPix;
	QPixmap m_oThumb;
	//QString m_sPicLocation; // keep them on the item?
	//QString m_sPicCaption;
	//QString m_sPicComment;
};

class data_item
{
	public:
		data_item();
		data_item(int i_iId);

		QString m_sSummary;
		QString m_sText;
		// length of the plain text code
		int m_iTextLength;
		QString m_sComment;

		QString m_sPicLocation;
		QString m_sPicCaption;
		QString m_sPicComment;
		//QPixmap m_oPix;
		//QPixmap m_oThumbnail;

		int m_iPicId; // references a data_pic

		const QPixmap getPix(sem_mediator* i_oMediator) const;
		const QPixmap getThumb(sem_mediator* i_oMediator) const;
		const color_scheme& get_color_scheme(sem_mediator* i_oMediator) const;
		const color_scheme& get_color_scheme_raw(sem_mediator* i_oMediator) const;

		int m_iId;
		bool m_bSelected;
		double m_iXX;
		double m_iYY;
		double m_iWW;
		double m_iHH;

		int m_iDataType;

		QList<QString> m_oFlags;

		int m_iColor;
		int m_iDown; // item to use when going down (keyboard navigation)
		color_scheme m_oCustom;

		QString m_sDiag;

		int m_iNumRows;
		int m_iNumCols;
		QHash<QPair<int, int>, QString> m_oTableData;
		QString m_sHints;

		//bool load_from_path(const QString &);

		QFont m_oDiagramFont;
		QHash<int, data_box*> m_oBoxes;
		QList<data_link*> m_oLinks;

		// transient
		int m_iObjectWidthHint;
		int m_iObjectHeightHint;
};

class data_ref
{
	public:
		data_ref(int, int);
		int m_iParent;
		int m_iChild;
};

#endif

