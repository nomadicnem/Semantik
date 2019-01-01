// Thomas Nagy 2007-2019 GPLV3

#include <QtDebug>
#include <QString>
#include <QCoreApplication>
#include <QtDebug>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>
#include <QAction>
#include <QStandardItemModel>
#include "con.h"

 #include "sembind.h"
#include "sem_mediator.h"
#include "data_item.h"


node::node() {

}

node::~node()
{
	while (!m_oChildren.isEmpty()) {
		m_oChildren.takeFirst();
	}
}

node& node::make_node(const QString& i_sTag, const QXmlAttributes& i_oAttrs) {
	node l_oNode;
	m_oChildren.push_back(l_oNode);
	node &ret = m_oChildren.last();
	ret.read_data(i_sTag, i_oAttrs);
	return ret;
}

void node::read_data(const QString&, const QXmlAttributes&)
{

}

void node::dump_xml(QStringList & other)
{
	foreach (node n, m_oChildren) {
		n.dump_xml(other);
	}
}

void data_box_entity_value::read_data(const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	Q_ASSERT(i_sName == QString("box_entity_val"));
	m_sName = i_oAttrs.value(notr("name"));
	m_sType = i_oAttrs.value(notr("type"));
	m_sKey  = i_oAttrs.value(notr("key"));
}

void data_box_entity_value::dump_xml(QStringList& i_oS)
{
	i_oS<<notr("          <box_entity_val name=\"%1\" type=\"%2\" key=\"%3\"/>\n").arg(
		bind_node::protectXML(m_sName),
		bind_node::protectXML(m_sType),
		bind_node::protectXML(m_sKey)
	);
}

data_item::data_item(int i_iId)
{
	m_iDataType = VIEW_TEXT;

	m_iId = i_iId;
	m_iColor = 0;
	m_sSummary = i18n("Empty");
	m_sText = "";
	m_sComment = "";
	m_sDiag = "";

	m_sPicLocation = "";
	m_sPicCaption = "";
	m_sPicComment = "";

	m_iDown = NO_ITEM;

	m_iNumRows = 3;
	m_iNumCols = 3;
	m_iPicId = NO_ITEM;

	m_iObjectWidthHint = 0;
	m_iObjectHeightHint = 0;

	m_iYY = m_iXX = -200000.0;
	m_bSelected = false;

	m_oDiagramFont.fromString("Monospace,10,-1,5,50,0,0,0,0,0");
}

const QPixmap data_item::getPix(sem_mediator* i_oMediator) const
{
	if (m_iDataType == VIEW_IMG)
	{
		return i_oMediator->getPix(m_iPicId);
	}
	else if (m_iDataType == VIEW_DIAG)
	{
		return i_oMediator->getPix(m_iId);
	}
	else
	{
		return QPixmap();
	}
}

const QPixmap data_item::getThumb(sem_mediator* i_oMediator) const
{
	return i_oMediator->getThumb(m_iPicId);
}


const color_scheme& data_item::get_color_scheme(sem_mediator* i_oMediator) const
{
	if (i_oMediator->parent_of(m_iId) <= 0)
	{
		return i_oMediator->m_oColorSchemes[0];
	}
	if (m_iColor < 0)
	{
		return m_oCustom;
	}
	if (m_iColor == i_oMediator->m_oColorSchemes.size())
	{
		return m_oCustom;
	}
	if (m_iColor > i_oMediator->m_oColorSchemes.size())
	{
		qDebug()<<"invalid index color scheme "<<m_iColor;
		return m_oCustom;
	}
	if (m_iColor == 0)
	{
		return i_oMediator->m_oColorSchemes[1];
	}
	return i_oMediator->m_oColorSchemes[m_iColor];
}

const color_scheme& data_item::get_color_scheme_raw(sem_mediator* i_oMediator) const
{
	if (m_iColor < 0)
	{
		return m_oCustom;
	}
	if (m_iColor == i_oMediator->m_oColorSchemes.size())
	{
		return m_oCustom;
	}
	if (m_iColor > i_oMediator->m_oColorSchemes.size())
	{
		qDebug()<<"invalid index color scheme "<<m_iColor;
		return m_oCustom;
	}
	return i_oMediator->m_oColorSchemes[m_iColor];
}


data_ref::data_ref(int i_iParent, int i_iChild)
{
	m_iParent = i_iParent;
	m_iChild = i_iChild;
}

bool data_ref::operator==(const data_ref& i_oA) const
{
	return i_oA.m_iParent == m_iParent && i_oA.m_iChild == m_iChild;
}

diagram_item::diagram_item()
{
	pen_style = Qt::SolidLine;
	border_width = 1;
	m_iColor = -1;
}

data_box::data_box(int id) : diagram_item(), node()
{
	m_iId = id;
	m_bIsEnd = false;
	m_bIsVertical = false;
	m_iType = data_box::ACTIVITY;
	m_iWW = 100;
	m_iHH = 40;
	m_iAlign = Qt::AlignCenter;
	m_oCustom.m_oInnerColor = QColor("#a7e89b");
	m_iLabelPosition = Qt::TopEdge;

	m_bAbstract = false;
	m_bStatic = false;
}

data_box& data_box::operator=(const data_box & i_o)
{
	m_iType = i_o.m_iType;
	m_iId = i_o.m_iId;
	m_sText = i_o.m_sText;
	m_iAlign = i_o.m_iAlign;
	m_iLabelPosition = i_o.m_iLabelPosition;

	m_bIsEnd = i_o.m_bIsEnd;
	m_bIsEnd = i_o.m_bIsEnd;
	m_bIsVertical = i_o.m_bIsVertical;
	m_iType = i_o.m_iType;
	m_iWW = i_o.m_iWW;
	m_iHH = i_o.m_iHH;
	m_oCustom.m_oInnerColor = i_o.m_oCustom.m_oInnerColor;

	m_oRowSizes     = i_o.m_oRowSizes;
	m_oColSizes     = i_o.m_oColSizes;

	m_bStatic       = i_o.m_bStatic;
	m_bAbstract     = i_o.m_bAbstract;
	m_sStereotype   = i_o.m_sStereotype;
	m_oMethods      = i_o.m_oMethods;
	m_oAttributes   = i_o.m_oAttributes;
	m_oEntityValues = i_o.m_oEntityValues;
	return *this;
}

data_box::~data_box()
{
	while (!m_oMethods.isEmpty()) {
		m_oMethods.takeFirst();
	}
	while (!m_oAttributes.isEmpty()) {
		m_oAttributes.takeFirst();
	}
}

void data_box::dump_xml(QStringList & i_oS)
{
	i_oS<<notr("<itembox id=\"%1\" text=\"%2\" x=\"%3\" y=\"%4\" w=\"%5\" h=\"%6\" color=\"%7\" t=\"%8\" %9>\n").arg(
		QString::number(m_iId),
		bind_node::protectXML(m_sText),
		QString::number(m_iXX),
		QString::number(m_iYY),
		QString::number(m_iWW),
		QString::number(m_iHH),
		m_oCustom.m_oInnerColor.name(),
		QString::number((int) m_iType),
		QString(" v=\"%1\" e=\"%2\" seq=\"%3\" version=\"3\" color_id=\"%4\" align=\"%5\" label_position=\"%6\"").arg(
			QString::number((int) m_bIsVertical),
			QString::number((int) m_bIsEnd),
			QString::number((int) m_iBoxHeight),
			QString::number((int) m_iColor),
			QString::number((int) m_iAlign),
			QString::number((int) m_iLabelPosition)
		)
	);

	foreach (int l_i, m_oRowSizes) {
		i_oS<<notr("      <box_row_size val=\"%1\"/>\n").arg(QString::number(l_i));
	}
	foreach (int l_i, m_oColSizes) {
		i_oS<<notr("      <box_col_size val=\"%1\"/>\n").arg(QString::number(l_i));
	}

	if (m_iType == data_box::CLASS)
	{
		i_oS<<notr("      <box_class stereotype=\"%1\" static=\"%2\" abstract=\"%3\">\n").arg(
			bind_node::protectXML(m_sStereotype),
			QString::number((int) m_bStatic),
			QString::number((int) m_bAbstract));

		foreach (data_box_method l_o, m_oMethods) {
			l_o.dump_xml(i_oS);
		}
		foreach (data_box_attribute l_o, m_oAttributes) {
			l_o.dump_xml(i_oS);
		}
		i_oS<<notr("      </box_class>\n");
	}
	else if (m_iType == data_box::ENTITY)
	{
		i_oS<<notr("<box_entity>\n");
		foreach (data_box_entity_value l_oValue, m_oEntityValues)
		{
			l_oValue.dump_xml(i_oS);
		}
		i_oS<<notr("</box_entity>\n");
	}

	//node::dump_xml(i_oS);
	i_oS<<notr("</itembox>\n");
}

void data_box::read_data(const QString& i_sTag, const QXmlAttributes& i_oAttrs)
{
	m_sText = i_oAttrs.value(notr("text"));
	m_iXX = i_oAttrs.value(notr("x")).toFloat();
	m_iYY = i_oAttrs.value(notr("y")).toFloat();
	m_iWW = i_oAttrs.value(notr("w")).toFloat();
	m_iHH = i_oAttrs.value(notr("h")).toFloat();
	m_iType = (data_box::IType) i_oAttrs.value(notr("t")).toInt();
	m_iLabelPosition = (Qt::Edge) i_oAttrs.value(notr("label_position")).toInt();
	if (m_iLabelPosition == 0)
	{
		m_iLabelPosition = Qt::TopEdge;
	}
	m_bIsVertical = i_oAttrs.value(notr("v")).toInt();
	m_bIsEnd = i_oAttrs.value(notr("e")).toInt();
	m_iBoxHeight = i_oAttrs.value(notr("seq")).toInt();
	QString l_oAlignVal = i_oAttrs.value(notr("align"));
	if (l_oAlignVal.isEmpty())
	{
		m_iAlign = Qt::AlignHCenter;
	}
	else
	{
		switch (l_oAlignVal.toInt())
		{
			case Qt::AlignLeft:
				m_iAlign = Qt::AlignLeft;
				break;
			case Qt::AlignRight:
				m_iAlign = Qt::AlignRight;
				break;
			case Qt::AlignJustify:
				m_iAlign = Qt::AlignJustify;
				break;
			case Qt::AlignHCenter:
			default:
				m_iAlign = Qt::AlignHCenter;
		}
	}
	if (m_iBoxHeight < 5)
	{
		m_iBoxHeight = 20;
	}
	m_oCustom.m_oInnerColor = QColor(i_oAttrs.value(notr("color")));
	m_iColor = i_oAttrs.value(notr("color_id")).toInt();
	int l_iVersion = i_oAttrs.value(notr("version")).toInt();
	if (l_iVersion < 1 && m_iType == data_box::LABEL)
	{
		m_oCustom.m_oInnerColor = Qt::black;
	}

	// TODO remove in the future...
	if (m_iType == data_box::ACTIVITY_START)
	{
		if (m_iWW > 20 + m_iHH) m_iWW = m_iHH = 20;
	}
}

node& data_box::make_node(const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	if (i_sName == notr("box_row_size"))
	{
		int l_iVal = i_oAttrs.value(notr("val")).toInt();
		m_oRowSizes.push_back(l_iVal);
	}
	else if (i_sName == notr("box_col_size"))
	{
		int l_iVal = i_oAttrs.value(notr("val")).toInt();
		m_oColSizes.push_back(l_iVal);
	}
	else if (i_sName == notr("box_class_method"))
	{
		data_box_method l_o;
		l_o.read_data(i_sName, i_oAttrs);
		m_oMethods.push_back(l_o);
		return m_oMethods.last();
	}
	else if (i_sName == notr("box_class_attribute"))
	{
		data_box_attribute l_o;
		l_o.read_data(i_sName, i_oAttrs);
		m_oAttributes.push_back(l_o);
		return m_oAttributes.last();
	}
	else if (i_sName == notr("box_class"))
	{
		m_bStatic = i_oAttrs.value(notr("static")).toInt();
		m_bAbstract = i_oAttrs.value(notr("abstract")).toInt();
		m_sStereotype = i_oAttrs.value(notr("stereotype"));
	}
	else if (i_sName == notr("box_entity_val"))
	{
		data_box_entity_value l_o;
		l_o.read_data(i_sName, i_oAttrs);
		m_oEntityValues.push_back(l_o);
		return m_oEntityValues.last();
	}

	return *this;
	// return node::make_node(i_sName, i_oAttrs);
}

void diagram_item::setBorderWidth(int bw)
{
	border_width = bw;
}

void diagram_item::setPenStyle(Qt::PenStyle st)
{
	pen_style = st;
}

void diagram_item::setColor(QColor co)
{
	m_oCustom.m_oInnerColor = co;
}

data_link::data_link() : diagram_item()
{
	m_oCustom.m_oInnerColor = QColor(Qt::black);
	m_iParentPos = NORTH;
	m_iChildPos = NORTH;
	m_iLeftArrow = NONE;
	m_iRightArrow = ASSOCIATION;
	m_iLineType = ZIGZAG;
}

void data_link::copy_from(const data_link& i_oLink)
{
	m_iId         = i_oLink.m_iId;
	m_sLink       = i_oLink.m_sLink;
	m_sCaption    = i_oLink.m_sCaption;
	m_iParent     = i_oLink.m_iParent;
	m_iChild      = i_oLink.m_iChild;
	m_sParentCaption = i_oLink.m_sParentCaption;
	m_sChildCaption  = i_oLink.m_sChildCaption;
	m_iParentPos  = i_oLink.m_iParentPos;
	m_iChildPos   = i_oLink.m_iChildPos;
	m_iLineType   = i_oLink.m_iLineType;
	m_oStartPoint = i_oLink.m_oStartPoint;
	m_oEndPoint   = i_oLink.m_oEndPoint;
	m_oOffsets    = i_oLink.m_oOffsets;

	m_iLeftArrow  = i_oLink.m_iLeftArrow;
	m_iRightArrow = i_oLink.m_iRightArrow;

	border_width  = i_oLink.border_width;
	pen_style     = i_oLink.pen_style;
	m_oCustom.m_oInnerColor = i_oLink.m_oCustom.m_oInnerColor;
}

bool data_link::equals(const data_link& i_oLink)
{
	if (m_iParent != NO_ITEM)
	{
		if (m_iParent != i_oLink.m_iParent || m_iParentPos != i_oLink.m_iParentPos)
			return false;
	}
	else
	{
		if (m_oStartPoint != i_oLink.m_oStartPoint)
			return false;
	}

	if (m_iChild != NO_ITEM)
	{
		if (m_iChild != i_oLink.m_iChild || m_iChildPos != i_oLink.m_iChildPos)
			return false;
	}
	else
	{
		if (m_oEndPoint != i_oLink.m_oEndPoint)
			return false;
	}

	if (m_oOffsets.size() != i_oLink.m_oOffsets.size())
		return false;

	for (int i = 0; i < m_oOffsets.size(); ++i)
	{
		if (m_oOffsets.at(i) != i_oLink.m_oOffsets.at(i))
			return false;
	}

	return true;
}

void data_box_method::read_data(const QString&i_sName, const QXmlAttributes& i_oAttrs)
{
	Q_ASSERT(i_sName == QString("box_class_method"));
	m_sText = i_oAttrs.value(notr("text"));
	m_oVisibility = (visibility::VisibilityType) i_oAttrs.value(notr("visibility")).toInt();
	m_bStatic = i_oAttrs.value(notr("static")).toInt();
        m_bAbstract = i_oAttrs.value(notr("abstract")).toInt();
}

void data_box_method::dump_xml(QStringList &i_oS)
{
	i_oS<<notr("          <box_class_method text=\"%1\" visibility=\"%2\" static=\"%3\" abstract=\"%4\" />\n").arg(
		bind_node::protectXML(m_sText),
		QString::number((int) m_oVisibility),
		QString::number((int) m_bStatic),
		QString::number((int) m_bAbstract)
	);
}

void data_box_attribute::read_data(const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	Q_ASSERT(i_sName == QString("box_class_attribute"));
	m_sText = i_oAttrs.value(notr("text"));
	m_oVisibility = (visibility::VisibilityType) i_oAttrs.value(notr("visibility")).toInt();
	m_bStatic = i_oAttrs.value(notr("static")).toInt();
}

void data_box_attribute::dump_xml(QStringList &i_oS)
{
	i_oS<<notr("          <box_class_attribute text=\"%1\" visibility=\"%2\" static=\"%3\" />\n").arg(
		bind_node::protectXML(m_sText),
		QString::number((int) m_oVisibility),
		QString::number((int) m_bStatic)
	);
}

const QColor & diagram_item::getColor(sem_mediator* i_oMediator) const
{
	if (m_iColor <= 0 || m_iColor >= i_oMediator->m_oColorSchemes.size())
	{
		return m_oCustom.m_oInnerColor;
	}
	else
	{
		return i_oMediator->m_oColorSchemes[m_iColor].m_oInnerColor;
	}
}

data_item::data_item()
{
}


