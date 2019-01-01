// Thomas Nagy 2007-2019 GPLV3

#include <QFileDialog>
#include <KMessageBox>
#include <QMenuBar>
#include <KDE/KApplication>
#include <KStandardAction>
#include <KRecentFilesAction>
#include <KDE/KActionCollection>
#include <windef.h>
#include <QClipboard>
#include <QMouseEvent>
#include <QAction>
#include <QGraphicsScene>
#include <QApplication>
#include<QList>
#include<QPrintDialog>
#include <QSet>
#include <QRegExp>
#include <QScrollBar>
#include <QMenu>
#include<QTextDocument>
#include <QColorDialog>
#include <QtDebug>
#include <QtDebug>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>
#include <QAction>
#include <QStandardItemModel>
#include <QPrinter>
#include <QSvgGenerator>
#include <QDesktopWidget>
#include <QStandardPaths>
#include "aux.h"
#include <QRadioButton>
#include <QSpinBox>
#include "con.h"
#include <math.h>
#include "kurlrequester.h"
#include "sem_mediator.h"
#include "box_item.h"
#include "box_dot.h"
#include "box_label.h"
#include "box_database.h"
#include "box_fork.h"
#include "box_chain.h"
#include "box_link.h"
#include "box_component.h"
#include "box_rectangle.h"
#include "box_pipe.h"
#include "box_node.h"
#include "box_decision.h"
#include "box_actor.h"
#include "box_matrix.h"
#include "box_frame.h"
#include "box_class.h"
#include "box_entity.h"
#include "data_item.h"
#include "box_usecase.h"
#include "box_view.h"
 #include"box_sequence.h"
#include "sembind.h"
 #include "mem_box.h"
	#include "export_fig_dialog.h"
#include "box_document_properties.h"

#define ALIGN_LEFT 22
#define ALIGN_CENTER 33
#define ALIGN_RIGHT 44
#define ALIGN_TOP 55
#define ALIGN_MIDDLE 66
#define ALIGN_BOTTOM 77

#define SAME_WIDTH 101
#define SAME_HEIGHT 102
#define SAME_WIDTH_HEIGHT 103

#define PIPAD 20

class box_reader : public QXmlDefaultHandler
{
    public:
	box_reader(box_view*);

	QString m_sBuf;
	int m_iVersion;
	box_view *m_oMediator;
	data_link *m_oCurrent;
	int m_iId;

	bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
	bool endElement(const QString&, const QString&, const QString&);
	bool characters(const QString &i_sStr);
};

box_reader::box_reader(box_view *i_oControl)
{
	m_oMediator = i_oControl;
	m_iId = m_oMediator->m_iId;
	m_oCurrent = NULL;
}

bool box_reader::startElement(const QString&, const QString&, const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	#ifdef _DEBUG
		return true;
	#endif
	if (i_sName == notr("box_item"))
	{
		int id = i_oAttrs.value(notr("id")).toInt();

		data_box *box = new data_box(id);
		m_oMediator->m_oMediator->m_oItems[m_iId].m_oBoxes[id] = box;
		box->m_iXX = i_oAttrs.value(notr("c1")).toFloat();
		box->m_iYY = i_oAttrs.value(notr("c2")).toFloat();
		box->m_sText = i_oAttrs.value(notr("text"));
		box->m_oCustom.m_oInnerColor = i_oAttrs.value(notr("col"));
		//l_o->setRect(QRectF(0., 0., i_oAttrs.value(notr("c3")).toDouble(), i_oAttrs.value(notr("c4")).toDouble()));
	}
	else if (i_sName == notr("box_link"))
	{
		data_link *link = new data_link();
		link->m_iParentPos = i_oAttrs.value(notr("p1")).toInt();
		link->m_iParent    = i_oAttrs.value(notr("c1")).toInt();
		link->m_iChildPos  = i_oAttrs.value(notr("p2")).toInt();
		link->m_iChild     = i_oAttrs.value(notr("c2")).toInt();

		if (link->m_iChildPos == 0) link->m_iChildPos = data_link::NORTH;
		if (link->m_iChildPos == 1) link->m_iChildPos = data_link::WEST;
		if (link->m_iChildPos == 2) link->m_iChildPos = data_link::SOUTH;
		if (link->m_iChildPos == 3) link->m_iChildPos = data_link::EAST;

		if (link->m_iParentPos == 0) link->m_iParentPos = data_link::NORTH;
		if (link->m_iParentPos == 1) link->m_iParentPos = data_link::WEST;
		if (link->m_iParentPos == 2) link->m_iParentPos = data_link::SOUTH;
		if (link->m_iParentPos == 3) link->m_iParentPos = data_link::EAST;

		link->m_sCaption = i_oAttrs.value(notr("caption"));
		link->m_sParentCaption = i_oAttrs.value(notr("parent_caption"));
		link->m_sChildCaption = i_oAttrs.value(notr("child_caption"));

		link->pen_style = (Qt::PenStyle) i_oAttrs.value(notr("pen_style")).toInt();
		link->border_width = i_oAttrs.value(notr("border_width")).toInt();
		m_oCurrent = link;
		Q_ASSERT(m_oMediator->m_oMediator->m_oItems.contains(m_iId));
		m_oMediator->m_oMediator->m_oItems[m_iId].m_oLinks.append(link);
	}
	else if (i_sName == notr("box_link_offset"))
	{
		if (m_oCurrent)
		{
			m_oCurrent->m_oOffsets.append(QPoint(i_oAttrs.value(notr("x")).toInt(), i_oAttrs.value(notr("y")).toInt()));
		}
	}

	return true;
}

bool box_reader::endElement(const QString&, const QString&, const QString& i_sName)
{
	if (i_sName == notr("box_link"))
	{
		m_oCurrent = NULL;
	}
	return true;
}

bool box_reader::characters(const QString &i_s)
{
	m_sBuf += i_s;
	return true;
}

int fceil(qreal l_fInput, int l_iBase)
{
	int l_iCal = ceil(l_fInput);
	if (l_iCal % l_iBase == 0)
	{
		return l_iCal;
	}
	return l_iBase * (1 + l_iCal / l_iBase);
}

int grid_int(int x) {
	return GRID * (x / GRID);
}

box_view::box_view(QWidget *i_oWidget, sem_mediator *i_oControl) : QGraphicsView(i_oWidget)
{
	m_oMediator = i_oControl;
	m_iId = NO_ITEM;
	m_bDisableGradient = false;
	m_bShowFileMenu = false;

	num_seq = 1111;

	QGraphicsScene *l_oScene = new QGraphicsScene(this);
	setScene(l_oScene);

	setDragMode(QGraphicsView::RubberBandDrag);

	//setCacheMode(CacheBackground);
	setRenderHint(QPainter::Antialiasing);

	m_oPropertiesAction = new QAction(i18n("Properties..."), this);
	//m_oPropertiesAction->setShortcut(i18n("Return"));
	connect(m_oPropertiesAction, SIGNAL(triggered()), this, SLOT(slot_edit_properties()));
	addAction(m_oPropertiesAction);

	m_oAddItemAction = new QAction(i18n("Activity"), this);
	m_oAddItemAction->setShortcut(i18n("Ctrl+Return"));
	connect(m_oAddItemAction, SIGNAL(triggered()), this, SLOT(slot_add_item()));
	addAction(m_oAddItemAction);

	m_oDeleteAction = new QAction(i18n("Delete selection"), this);
	m_oDeleteAction->setShortcut(i18n("Delete"));
	connect(m_oDeleteAction, SIGNAL(triggered()), this, SLOT(slot_delete()));
	addAction(m_oDeleteAction);

	m_oMoveUpAction = new QAction(i18n("Raise"), this);
	m_oMoveUpAction->setShortcut(i18n("PgUp"));
	connect(m_oMoveUpAction, SIGNAL(triggered()), this, SLOT(slot_move_up()));
	addAction(m_oMoveUpAction);

	m_oMoveDownAction = new QAction(i18n("Send back"), this);
	m_oMoveDownAction->setShortcut(i18n("PgDown"));
	connect(m_oMoveDownAction, SIGNAL(triggered()), this, SLOT(slot_move_down()));
	addAction(m_oMoveDownAction);

	m_oAddLabel = new QAction(i18n("Floating text"), this);
	connect(m_oAddLabel, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddComponent = new QAction(i18n("Component"), this);
	connect(m_oAddComponent, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddRectangle = new QAction(i18n("Rectangle"), this);
	connect(m_oAddRectangle, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddNode = new QAction(i18n("Node"), this);
	connect(m_oAddNode, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddDecision = new QAction(i18n("Decision"), this);
	connect(m_oAddDecision, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddDatabase = new QAction(i18n("Database"), this);
	connect(m_oAddDatabase, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddPipe = new QAction(i18n("Pipe"), this);
	connect(m_oAddPipe, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddSequence = new QAction(i18n("Sequence"), this);
	connect(m_oAddSequence, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddDotStart = new QAction(i18n("Activity start"), this);
	connect(m_oAddDotStart, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddDotEnd = new QAction(i18n("Activity end"), this);
	connect(m_oAddDotEnd, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddParallelHorizontal = new QAction(i18n("Horizontal fork/join"), this);
	connect(m_oAddParallelHorizontal, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddParallelVertical = new QAction(i18n("Vertical fork/join"), this);
	connect(m_oAddParallelVertical, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddActor = new QAction(i18n("Actor"), this);
	connect(m_oAddActor, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddUsecase = new QAction(i18n("Usecase"), this);
	connect(m_oAddUsecase, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddMatrix = new QAction(i18n("Matrix"), this);
	connect(m_oAddMatrix, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddFrame = new QAction(i18n("Boundary"), this);
	connect(m_oAddFrame, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddClass = new QAction(i18n("Class"), this);
	connect(m_oAddClass, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddEntity = new QAction(i18n("Entity"), this);
	connect(m_oAddEntity, SIGNAL(triggered()), this, SLOT(slot_add_element()));


	m_oFileImport = new QAction(i18n("Import from file..."), this);
	connect(m_oFileImport, SIGNAL(triggered()), this, SLOT(slot_import_from_file()));
	m_oFileExport = new QAction(i18n("Export to file..."), this);
	connect(m_oFileExport, SIGNAL(triggered()), this, SLOT(slot_export_to_file()));
	m_oCopyToClipboard = new QAction(i18n("Copy image to clipboard"), this);
	connect(m_oCopyToClipboard, SIGNAL(triggered()), this, SLOT(slot_copy_picture()));

	m_oLastMovePoint = QPointF(-100, -100);

	m_oCurrent = NULL;
	m_bPressed = false;
	m_bScroll = false;

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	m_oMenu = NULL;
	setMouseTracking(true);
}

void box_view::init_menu()
{
	m_oMenu = new QMenu(this);

	if (m_bShowFileMenu) {
		m_oFileMenu = m_oMenu->addMenu(i18n("File operations"));
		m_oFileMenu->addAction(m_oFileImport);
		m_oFileMenu->addAction(m_oFileExport);
		m_oFileMenu->addAction(m_oCopyToClipboard);
		m_oMenu->addSeparator();
	}

	m_oMenu->addAction(m_oAddLabel);
	m_oMenu->addAction(m_oAddItemAction);
	m_oMenu->addAction(m_oAddComponent);
	m_oMenu->addAction(m_oAddNode);

	m_oAddBoxMenu = m_oMenu->addMenu(i18n("More elements"));
	m_oAddBoxMenu->addAction(m_oAddDotStart);
	m_oAddBoxMenu->addAction(m_oAddDotEnd);
	m_oAddBoxMenu->addAction(m_oAddParallelHorizontal);
	m_oAddBoxMenu->addAction(m_oAddParallelVertical);
	m_oAddBoxMenu->addAction(m_oAddDecision);
	m_oAddBoxMenu->addAction(m_oAddActor);
	m_oAddBoxMenu->addAction(m_oAddUsecase);
	m_oAddBoxMenu->addAction(m_oAddMatrix);
	m_oAddBoxMenu->addAction(m_oAddFrame);
	m_oAddBoxMenu->addAction(m_oAddRectangle);
	m_oAddBoxMenu->addAction(m_oAddClass);
	m_oAddBoxMenu->addAction(m_oAddEntity);
	m_oAddBoxMenu->addAction(m_oAddPipe);
	m_oAddBoxMenu->addAction(m_oAddDatabase);
	m_oAddBoxMenu->addAction(m_oAddSequence);

	//m_oMenu->addAction(m_oEditAction);
	m_oMenu->addAction(m_oDeleteAction);

	m_oMenu->addAction(m_oMoveUpAction);
	m_oMenu->addAction(m_oMoveDownAction);

	QAction *l_o;

	/*
	m_oWidthMenu = m_oMenu->addMenu(i18n("Connection width"));
	m_oWidthGroup = new QActionGroup(this);
	l_o = m_oWidthMenu->addAction(i18n("1px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(1)); m_oWidthGroup->addAction(l_o);
	l_o = m_oWidthMenu->addAction(i18n("2px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(2)); m_oWidthGroup->addAction(l_o);
	l_o = m_oWidthMenu->addAction(i18n("3px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(3)); m_oWidthGroup->addAction(l_o);

	m_oStyleMenu = m_oMenu->addMenu(i18n("Connection style"));
	m_oStyleGroup = new QActionGroup(this);
	l_o = m_oStyleMenu->addAction(i18n("solid line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::SolidLine)); m_oStyleGroup->addAction(l_o);
	l_o = m_oStyleMenu->addAction(i18n("dot line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::DotLine)); m_oStyleGroup->addAction(l_o);
	l_o = m_oStyleMenu->addAction(i18n("dash line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::DashLine)); m_oStyleGroup->addAction(l_o);

	*/

	m_oMenu->addSeparator();

	m_oTextAlignMenu = m_oMenu->addMenu(i18n("Text Alignment"));
	m_oTextAlignGroup = new QActionGroup(this);
	l_o = m_oTextAlignMenu->addAction(i18n("Align left")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_text_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_LEFT)); m_oTextAlignGroup->addAction(l_o);
	l_o = m_oTextAlignMenu->addAction(i18n("Align center")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_text_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_CENTER)); m_oTextAlignGroup->addAction(l_o);
	l_o = m_oTextAlignMenu->addAction(i18n("Align right")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_text_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_RIGHT)); m_oTextAlignGroup->addAction(l_o);

	m_oAlignMenu = m_oMenu->addMenu(i18n("Alignment"));
	m_oAlignGroup = new QActionGroup(this);
	l_o = m_oAlignMenu->addAction(i18n("Align left")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_LEFT)); m_oAlignGroup->addAction(l_o);
	l_o = m_oAlignMenu->addAction(i18n("Align center")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_CENTER)); m_oAlignGroup->addAction(l_o);
	l_o = m_oAlignMenu->addAction(i18n("Align right")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_RIGHT)); m_oAlignGroup->addAction(l_o);

	l_o = m_oAlignMenu->addAction(i18n("Align top")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_TOP)); m_oAlignGroup->addAction(l_o);
	l_o = m_oAlignMenu->addAction(i18n("Align middle")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_MIDDLE)); m_oAlignGroup->addAction(l_o);
	l_o = m_oAlignMenu->addAction(i18n("Align bottom")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_align())); addAction(l_o); l_o->setData(QVariant(ALIGN_BOTTOM)); m_oAlignGroup->addAction(l_o);


	m_oSizeMenu = m_oMenu->addMenu(i18n("Size"));
	m_oSizeGroup = new QActionGroup(this);
	l_o = m_oSizeMenu->addAction(i18n("Same width")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_size())); addAction(l_o); l_o->setData(QVariant(SAME_WIDTH)); m_oSizeGroup->addAction(l_o);
	l_o = m_oSizeMenu->addAction(i18n("Same height")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_size())); addAction(l_o); l_o->setData(QVariant(SAME_HEIGHT)); m_oSizeGroup->addAction(l_o);
	l_o = m_oSizeMenu->addAction(i18n("Same width and height")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_size())); addAction(l_o); l_o->setData(QVariant(SAME_WIDTH_HEIGHT)); m_oSizeGroup->addAction(l_o);


	m_oAddItemAction->setEnabled(false);
	m_oDeleteAction->setEnabled(false);

	//m_oEditAction->setEnabled(false);
	//m_oMoveUpAction->setEnabled(false);
	//m_oMoveDownAction->setEnabled(false);
	m_oMenu->addSeparator();
	m_oMenu->addMenu(m_oColorMenu);
	m_oMenu->addAction(m_oPropertiesAction);
}

box_view::~box_view()
{

}

void box_view::resizeEvent(QResizeEvent* e)
{
	QGraphicsView::resizeEvent(e);

	QRect l_oRect = viewport()->rect();
	QRectF ar = QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight()));
	QRectF br = scene()->itemsBoundingRect();
	br = br.united(ar);

	scene()->setSceneRect(br);
}

void box_view::notify_add_item(int id)
{
	m_iId = id;
	setEnabled(true);
}

void box_view::notify_edit_box(int id, int bid)
{
	Q_ASSERT(id == m_iId);
	box_item *item = (box_item*) m_oItems.value(bid); // TODO
	Q_ASSERT(item != NULL);
	item->update_data();
}

void box_view::notify_select(const QList<int>& unsel, const QList<int>& sel)
{
	clear_diagram();

	if (sel.size() != 1)
	{
		m_iId = NO_ITEM;
		setEnabled(false);
		//m_oEditAction->setEnabled(false);
		//m_oCancelEditAction->setEnabled(false);
	}
	else
	{
		m_iId = sel.at(0);
		Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
		data_item& l_oData = m_oMediator->m_oItems[m_iId];
		if (l_oData.m_iDataType == VIEW_DIAG)
		{
			if (!l_oData.m_sDiag.isEmpty())
			{
				from_string(l_oData.m_sDiag);
				l_oData.m_sDiag = notr("");
			}
			sync_view();
			check_canvas_size();
		}
		setEnabled(true);
	}
}

void box_view::sync_view()
{
	if (!m_iId) return;

	Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
	data_item& item = m_oMediator->m_oItems[m_iId];
	scene()->setFont(item.m_oDiagramFont);

	if (item.m_iDataType != VIEW_DIAG)
	{
		return;
	}
	if (!item.m_sDiag.isEmpty())
	{
		from_string(item.m_sDiag);
		item.m_sDiag = "";
	}

	foreach (data_box *box, item.m_oBoxes.values())
	{
		connectable *l_o = NULL;
		if (box->m_iType == data_box::ACTIVITY)
		{
			l_o = new box_item(this, box->m_iId);
		}
		else if (box->m_iType == data_box::LABEL)
		{
			l_o = new box_label(this, box->m_iId);
		}
		else if (box->m_iType == data_box::ACTOR)
		{
			l_o = new box_actor(this, box->m_iId);
		}
		else if (box->m_iType == data_box::USECASE)
		{
			l_o = new box_usecase(this, box->m_iId);
		}
		else if (box->m_iType == data_box::COMPONENT)
		{
			l_o = new box_component(this, box->m_iId);
		}
		else if (box->m_iType == data_box::RECTANGLE)
		{
			l_o = new box_rectangle(this, box->m_iId);
		}
		else if (box->m_iType == data_box::NODE)
		{
			l_o = new box_node(this, box->m_iId);
		}
		else if (box->m_iType == data_box::DECISION)
		{
			l_o = new box_decision(this, box->m_iId);
		}
		else if (box->m_iType == data_box::ACTIVITY_START)
		{
			l_o = new box_dot(this, box->m_iId);
		}
		else if (box->m_iType == data_box::ACTIVITY_PARALLEL)
		{
			l_o = new box_fork(this, box->m_iId);
		}
		else if (box->m_iType == data_box::MATRIX)
		{
			l_o = new box_matrix(this, box->m_iId);
		}
		else if (box->m_iType == data_box::SEQUENCE)
		{
			l_o = new box_sequence(this, box->m_iId);
		}
		else if (box->m_iType == data_box::FRAME)
		{
			l_o = new box_frame(this, box->m_iId);
		}
		else if (box->m_iType == data_box::CLASS)
		{
			l_o = new box_class(this, box->m_iId);
		}
		else if (box->m_iType == data_box::ENTITY)
		{
			l_o = new box_entity(this, box->m_iId);
		}
		else if (box->m_iType == data_box::DATABASE)
		{
			l_o = new box_database(this, box->m_iId);
		}
		else if (box->m_iType == data_box::PIPE)
		{
			l_o = new box_pipe(this, box->m_iId);
		}
		else if (box->m_iType == data_box::RECTANGLE)
		{
			l_o = new box_rectangle(this, box->m_iId);
		}
		else
		{
			Q_ASSERT(false);
		}
		m_oItems[box->m_iId] = l_o;
		(dynamic_cast<QGraphicsItem*> (l_o))->setPos(QPointF(box->m_iXX, box->m_iYY));
		l_o->update_data();
	}

	foreach (data_link *link, item.m_oLinks) {
		box_link *l_o = new box_link(this);
		l_o->m_oInnerLink.copy_from(*link);
		l_o->m_oLink = link;
		l_o->update_text();
		l_o->update_pos();
		m_oLinks.append(l_o);
	}
}

void box_view::notify_export_item(int id)
{
	int l_iOldId = m_iId;
	if (m_oMediator->m_oItems[id].m_iDataType != VIEW_DIAG)
		return;
	clear_diagram();

	m_iId = id;
	sync_view();

	QRectF l_oRect = scene()->itemsBoundingRect();

	l_oRect = l_oRect.adjusted(-15, -15, 15, 15);

	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());

	QPair<int, int> p = m_oMediator->hint_size_diagram(id);

	if (p.first != 0) {
		l_oR.setWidth(p.first);
		if (p.second == 0) {
			l_oR.setHeight((p.first * l_oRect.height()) / (double) l_oRect.width());
		}
	}
	if (p.second != 0) {
		l_oR.setHeight(p.second);
		if (p.first == 0) {
			l_oR.setWidth((p.second * l_oRect.width()) / (double) l_oRect.height());
		}
	}

	Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
	data_item& item = m_oMediator->m_oItems[m_iId];
	item.m_iObjectWidthHint = l_oR.width();
	item.m_iObjectHeightHint = l_oR.height();

	// fill with white
	QImage l_oImage((int) l_oR.width(), (int) l_oR.height(), QImage::Format_RGB32);
	l_oImage.fill(qRgb(255,255,255));

	Qt::AspectRatioMode rat = (p.first == 0 || p.second == 0) ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;

	QPainter l_oP;
	if (l_oP.begin(&l_oImage))
	{
		l_oP.setRenderHints(QPainter::Antialiasing);
		scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
		scene()->render(&l_oP, l_oR, l_oRect, rat);
		scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
		l_oP.end();
	}
	l_oImage.save(notr("%1/%2/diag-%3.png").arg(m_oMediator->m_sOutDir, m_oMediator->m_sOutProject, QString::number(m_iId)));

	QPrinter l_oPrinter;
	//l_oPrinter.setResolution(QPrinter::HighResolution);
	l_oPrinter.setOrientation(QPrinter::Portrait);
	l_oPrinter.setOutputFormat(QPrinter::PdfFormat);
	l_oPrinter.setPaperSize(l_oR.size(), QPrinter::DevicePixel);
	l_oPrinter.setPageMargins(0, 0, 0, 0, QPrinter::DevicePixel);
	l_oPrinter.setOutputFileName(notr("%1/%2/diag-%3.pdf").arg(m_oMediator->m_sOutDir, m_oMediator->m_sOutProject, QString::number(m_iId)));

	QPainter l_oPdf;
	if (l_oPdf.begin(&l_oPrinter))
	{
		m_bDisableGradient = true;
		scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
		scene()->render(&l_oPdf, l_oR, l_oRect, rat);
		scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
		l_oPdf.end();
		m_bDisableGradient = false;
	}

	// and the svg for html/openoffice
	QSvgGenerator l_oGenerator;
	l_oGenerator.setFileName(notr("%1/%2/diag-%3.svg").arg(m_oMediator->m_sOutDir, m_oMediator->m_sOutProject, QString::number(m_iId)));
	l_oGenerator.setSize(QSize(l_oR.width(), l_oR.height()));
	l_oGenerator.setViewBox(l_oR);
	l_oGenerator.setTitle(i18n("Semantik diagram"));
	l_oGenerator.setDescription(notr("Generated by Semantik, if it does not render properly check your system fonts!"));
	l_oGenerator.setResolution(QApplication::desktop()->logicalDpiX());

	QPainter l_oSvg;
	if (l_oSvg.begin(&l_oGenerator))
	{
		l_oSvg.setRenderHints(QPainter::Antialiasing);
		scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
		scene()->render(&l_oSvg, l_oR, l_oRect, rat);
		scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
		l_oSvg.end();
	}

	clear_diagram();
	m_iId = l_iOldId;
	if (m_iId != NO_ITEM)
	{
		sync_view();
	}
}

void box_view::slot_delete()
{
	QList<data_box*> boxes;
	QSet<data_link*> links;
	foreach (QGraphicsItem* el, scene()->selectedItems())
	{
		if (box_link *l = dynamic_cast<box_link*>(el))
		{
			links << l->m_oLink;
			Q_ASSERT(l->m_oLink);
		}
		else if (connectable* c = dynamic_cast<connectable*>(el))
		{
			data_box *b = c->m_oBox;
			boxes.append(b);
			foreach (box_link* l, m_oLinks)
			{
				if (l->m_oLink->m_iParent == b->m_iId || l->m_oLink->m_iChild == b->m_iId)
				{
					links << l->m_oLink;
				}
			}
		}
	}

	if (boxes.size() > 0 || links.size() > 0) {
		mem_del_box *del = new mem_del_box(m_oMediator, m_iId);
		del->init(boxes, links.toList());
		del->apply();
	}
}

void box_view::enable_menu_actions()
{
	QList<QGraphicsItem*> selection = scene()->selectedItems();
	int selected = selection.size();

	m_oAddItemAction->setEnabled(selected <= 1);
	m_oDeleteAction->setEnabled(selected >= 1);
	m_oColorMenu->setEnabled(selected >= 1);

	m_oPropertiesAction->setEnabled(selected == 0 or (selected == 1 and dynamic_cast<editable*>(selection.at(0))));

	m_oSizeMenu->setEnabled(selected > 1);
	foreach(QAction* l_o, m_oSizeGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}

	/*m_oWidthMenu->setEnabled(selected >= 1 and dynamic_cast<box_link*>(selection.at(0)));
	foreach(QAction* l_o, m_oWidthGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}

	m_oStyleMenu->setEnabled(selected >= 1 and dynamic_cast<box_link*>(selection.at(0)));
	foreach(QAction* l_o, m_oStyleGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}*/

	m_oTextAlignMenu->setEnabled(selected >= 1);
	foreach(QAction* l_o, m_oTextAlignGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}

	m_oAlignMenu->setEnabled(selected > 1);
	foreach(QAction* l_o, m_oAlignGroup->actions())
	{
		l_o->setEnabled(selected > 1);
	}
}

void box_view::enable_actions()
{
	foreach (QAction* l_o, actions())
	{
		l_o->setEnabled(true); // TODO we could disable the move actions one by one
	}
	enable_menu_actions();
}

void box_view::slot_add_item()
{
	foreach(QGraphicsItem *l_o, scene()->items()) {
		l_o->setSelected(false);
	}

	mem_add_box *add = new mem_add_box(m_oMediator, m_iId, next_seq());
	add->box->m_iXX = m_oLastPoint.x() - 20;
	add->box->m_iYY = m_oLastPoint.y() - 20;
	add->box->m_iWW = 80;
	add->apply();

	foreach (QGraphicsItem *l_o, scene()->selectedItems())
	{
		l_o->setSelected(false);
	}

	box_item *l_o = (box_item*) m_oItems.value(add->box->m_iId);
	l_o->setSelected(true);
	// l_o->setPos(m_oLastPoint - QPointF(l_oRect.width()/2, l_oRect.height()/2)); // TODO
}

void box_view::change_colors(QAction* i_oAct)
{
	if (!hasFocus()) return;
	if (scene()->selectedItems().size() < 1) return;

	int l_iColor = -1;
	QColor l_oColor;
	static QColor selected_color;

	QList<QAction*> l_oActs = i_oAct->actionGroup()->actions();
	for (int i=1; i < l_oActs.size(); ++i)
	{
		if (l_oActs[i] == i_oAct)
		{
			if (i == l_oActs.size()-1)
			{
				selected_color = QColorDialog::getColor(selected_color, this);
				if (!selected_color.isValid())
				{
					return;
				}
				l_oColor = selected_color;
			}
			else
			{
				selected_color = l_oColor = m_oMediator->m_oColorSchemes[i].m_oInnerColor;
			}
			l_iColor = i;
			break;
		}
	}

	mem_prop_box *mem = new mem_prop_box(m_oMediator, m_iId);
	foreach (QGraphicsItem *l_o, scene()->selectedItems())
	{
		if (box_link *k = dynamic_cast<box_link*>(l_o))
		{
			mem->items.append(k->m_oLink);
		}
		else if (connectable* t = dynamic_cast<connectable*>(l_o))
		{
			mem->items.append(t->m_oBox);
		}
	}
	mem->change_type = CH_COLOR;
	mem->new_props.m_iColor = l_iColor;
	mem->new_props.m_oCustom.m_oInnerColor = l_oColor;
	mem->apply();
}

void box_view::slot_penstyle()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();
	mem_prop_box *mem = new mem_prop_box(m_oMediator, m_iId);
	foreach (QGraphicsItem* l_o, scene()->selectedItems())
	{
		if (box_link *k = dynamic_cast<box_link*>(l_o))
		{
			mem->items.append(k->m_oLink);
		}
	}
	mem->change_type = CH_PENST;
	mem->new_props.pen_style = (Qt::PenStyle) l_i;
	mem->apply();
}

void box_view::slot_add_element()
{
	QAction *sender = (QAction*) QObject::sender();
	mem_add_box *add = new mem_add_box(m_oMediator, m_iId, next_seq());
	add->box->m_iXX = GRID * (int) (m_oLastPoint.x() / GRID);
	add->box->m_iYY = GRID * (int) (m_oLastPoint.y() / GRID);

	if (sender == m_oAddDotStart || sender == m_oAddDotEnd)
	{
		add->box->m_iHH = 20;
		add->box->m_iWW = 20;
	}
	else if (sender == m_oAddParallelVertical)
	{
		add->box->m_iWW = FORK_WIDTH;
		add->box->m_iHH = FORK_LENGTH;
		add->box->m_bIsVertical = true;
	}
	else if (sender == m_oAddParallelHorizontal)
	{
		add->box->m_iHH = FORK_WIDTH;
		add->box->m_iWW = FORK_LENGTH;
		add->box->m_bIsVertical = false;
	}

	add->box->m_bIsEnd = sender == m_oAddDotEnd;

	if (sender == m_oAddDotEnd || sender == m_oAddDotStart) {
		add->box->m_iType = data_box::ACTIVITY_START;
		add->box->m_oCustom.m_oInnerColor = QColor(Qt::black);
	}
	if (sender == m_oAddParallelHorizontal || sender == m_oAddParallelVertical) {
		add->box->m_iType = data_box::ACTIVITY_PARALLEL;
		add->box->m_oCustom.m_oInnerColor = QColor(Qt::black);
	}
	if (sender == m_oAddLabel)
	{
		add->box->m_iType = data_box::LABEL;
		add->box->m_iAlign = Qt::AlignLeft;
		add->box->m_iWW = 60;
		add->box->m_iHH = 30;
		add->box->m_oCustom.m_oInnerColor = Qt::black;
		add->box->m_sText = QString("...");
	}
	else if (sender == m_oAddActor)
	{
		add->box->m_iType = data_box::ACTOR;
		add->box->m_iWW = 30;
		add->box->m_iHH = 70;
	}
	else if (sender == m_oAddUsecase)
	{
		add->box->m_iType = data_box::USECASE;
		add->box->m_iWW = 100;
		add->box->m_iHH = 50;
	}
	else if (sender == m_oAddComponent)
	{
		add->box->m_iType = data_box::COMPONENT;
		add->box->m_iWW = 120;
		add->box->m_iHH = 60;
		add->box->m_oCustom.m_oInnerColor = QColor("#FCF2E2");
	}
	else if (sender == m_oAddRectangle)
	{
		add->box->m_iType = data_box::RECTANGLE;
		add->box->m_iWW = 100;
		add->box->m_iHH = 40;
		add->box->m_oCustom.m_oInnerColor = QColor("#FFFFCC");
	}
	else if (sender == m_oAddNode)
	{
		add->box->m_iType = data_box::NODE;
		add->box->m_iWW = 180;
		add->box->m_iHH = 180;
		add->box->m_oCustom.m_oInnerColor = QColor("#FCF2E2");
	}
	else if (sender == m_oAddDecision)
	{
		add->box->m_iType = data_box::DECISION;
		add->box->m_iWW = 40;
		add->box->m_iHH = 40;
	}
	else if (sender == m_oAddMatrix)
	{
		add->box->m_iType = data_box::MATRIX;
		add->box->m_iWW = 510;
		add->box->m_iHH = 320;
		add->box->m_oRowSizes.push_back(160);
		add->box->m_oColSizes.push_back(170);
		add->box->m_oColSizes.push_back(170);
	}
	else if (sender == m_oAddSequence)
	{
		add->box->m_iType = data_box::SEQUENCE;
		add->box->m_iWW = 80;
		add->box->m_iHH = 280;
		add->box->m_iBoxHeight = 40;
	}
	else if (sender == m_oAddFrame)
	{
		add->box->m_iType = data_box::FRAME;
		add->box->m_iWW = 510;
		add->box->m_iHH = 320;
	}
	else if (sender == m_oAddDatabase)
	{
		add->box->m_iType = data_box::DATABASE;
		add->box->m_oCustom.m_oInnerColor = QColor("#FCF2E2");
		add->box->m_iWW = 60;
		add->box->m_iHH = 90;
	}
	else if (sender == m_oAddPipe)
	{
		add->box->m_iType = data_box::PIPE;
		add->box->m_oCustom.m_oInnerColor = QColor("#FCF2E2");
		add->box->m_iWW = 90;
		add->box->m_iHH = 30;
	}
	else if (sender == m_oAddClass)
	{
		add->box->m_iType = data_box::CLASS;
		add->box->m_oCustom.m_oInnerColor = QColor("#FCF2E2");
		add->box->m_iWW = 150;
		add->box->m_iHH = 80;

		add->box->m_sText = "MyClass";
		add->box->m_sStereotype = "entity";
		add->box->m_bStatic = false;
		add->box->m_bAbstract = false;

		{
			data_box_method l_o;
			l_o.m_oVisibility = visibility::PUBLIC;
			l_o.m_sText = "hello() : void";
			l_o.m_bStatic = false;
			l_o.m_bAbstract = false;
			add->box->m_oMethods.push_back(l_o);
		}

		{
			data_box_attribute l_o;
			l_o.m_oVisibility = visibility::PUBLIC;
			l_o.m_bStatic = false;
			l_o.m_sText = "id: int";
			add->box->m_oAttributes.push_back(l_o);
		}
	}
	else if (sender == m_oAddEntity)
	{
		add->box->m_iType = data_box::ENTITY;
		add->box->m_oCustom.m_oInnerColor = QColor("#FCF2E2");
		add->box->m_iWW = 220;
		add->box->m_iHH = 80;
		add->box->m_sText = "MyTable";

		{
			data_box_entity_value l_o;
			l_o.m_sName = "id";
			l_o.m_sKey = "PK";
			l_o.m_sType = "INTEGER";
			add->box->m_oEntityValues.push_back(l_o);
		}
		{
			data_box_entity_value l_o;
			l_o.m_sName = "customer_id";
			l_o.m_sKey = "FK";
			l_o.m_sType = "INTEGER";
			add->box->m_oEntityValues.push_back(l_o);
		}
		{
			data_box_entity_value l_o;
			l_o.m_sName = "name";
			l_o.m_sType = "VARCHAR2(50)";
			add->box->m_oEntityValues.push_back(l_o);
		}
	}

	add->apply();

	foreach (QGraphicsItem *l_o, scene()->selectedItems())
	{
		l_o->setSelected(false);
	}

	QGraphicsItem *l_o = dynamic_cast<QGraphicsItem*>(m_oItems.value(add->box->m_iId));
	l_o->setSelected(true);
}

void box_view::slot_penwidth()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();
	mem_prop_box *mem = new mem_prop_box(m_oMediator, m_iId);
	foreach (QGraphicsItem* l_o, scene()->selectedItems())
	{
		if (box_link *k = dynamic_cast<box_link*>(l_o))
		{
			mem->items.append(k->m_oLink);
		}
	}
	mem->change_type = CH_BORDER;
	mem->new_props.border_width = l_i;
	mem->apply();
}

void box_view::slot_edit_properties()
{
	QList<QGraphicsItem*> l_o = scene()->selectedItems();
	if (l_o.length() == 1)
	{
		if (editable*e = dynamic_cast<editable*>(l_o.at(0)))
		{
			e->properties();
		}
	}
	else if (l_o.length() == 0)
	{
		box_document_properties l_oProps(this);
		l_oProps.exec();
	}
}

#if 0
void box_view::slot_toggle_edit()
{
	if (!hasFocus()) {
		return;
	}

	box_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items())
	{
		box_item* k;
		if (tmp->isSelected() && (k = dynamic_cast<box_item*>(tmp)))
		{
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = k;
			}
		}
	}

	if (sel) {
		if (sel->textInteractionFlags() & Qt::TextEditorInteraction) {
			sel->setTextInteractionFlags(Qt::NoTextInteraction);
			/*if (sel->toPlainText() == i18n("")) {
				sel->setPlainText(i18n("Empty"));
				sel->update(); // seems to update faster
				//sel->update_links();

			}*/
			if (sel->toPlainText() != m_oMediator->m_oItems[m_iId]->m_oBoxes[sel->m_iId]->m_sText) {
				mem_edit_box *ed = new mem_edit_box(m_oMediator, m_iId, sel->m_iId);
				ed->newText = sel->toPlainText();
				ed->apply();
			}
		} else {
			sel->setTextInteractionFlags(Qt::TextEditorInteraction);
			sel->setFocus();

			m_oAddItemAction->setEnabled(false);
			m_oDeleteAction->setEnabled(false);
			m_oCancelEditAction->setEnabled(true);

			/*
			m_oInsertSiblingAction->setEnabled(false);
			m_oNextRootAction->setEnabled(false);

			m_oMoveUpAction->setEnabled(false);
			m_oMoveDownAction->setEnabled(false);
			m_oMoveLeftAction->setEnabled(false);
			m_oMoveRightAction->setEnabled(false);
			m_oSelectUpAction->setEnabled(false);
			m_oSelectDownAction->setEnabled(false);
			m_oSelectLeftAction->setEnabled(false);
			m_oSelectRightAction->setEnabled(false);
			m_oMediator->check_undo(false);
			*/
			return;
		}
	}

	m_oAddItemAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
	m_oCancelEditAction->setEnabled(false);

	/*
	m_oInsertSiblingAction->setEnabled(true);
	m_oNextRootAction->setEnabled(true);

	m_oMoveUpAction->setEnabled(true);
	m_oMoveDownAction->setEnabled(true);
	m_oMoveLeftAction->setEnabled(true);
	m_oMoveRightAction->setEnabled(true);
	m_oSelectUpAction->setEnabled(true);
	m_oSelectDownAction->setEnabled(true);
	m_oSelectLeftAction->setEnabled(true);
	m_oSelectRightAction->setEnabled(true);
	m_oMediator->check_undo(true);
	*/
}

void box_view::slot_cancel_edit()
{
	//qDebug()<<"slot cancel edit";
	box_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		box_item *k;
		if (tmp->isSelected() && (k = dynamic_cast<box_item*>(tmp)))
		{
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = k;
			}
		}
	}

	if (sel && sel->textInteractionFlags() & Qt::TextEditorInteraction) {
		//qDebug()<<"cancel edit!"<<sel;
		sel->setTextInteractionFlags(Qt::NoTextInteraction);
		sel->update_data();
	}

	m_oAddItemAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
}
#endif

void box_view::slot_move_up()
{
	foreach (QGraphicsItem *l_oItem, scene()->selectedItems())
	{
		l_oItem->setZValue(l_oItem->zValue()+1);
	}
}

void box_view::slot_move_down()
{
	foreach (QGraphicsItem *l_oItem, scene()->selectedItems())
	{
		l_oItem->setZValue(l_oItem->zValue()-1);
	}
}

void box_view::from_string(const QString &i_s)
{
	box_reader l_oHandler(this);
	QXmlInputSource l_oSource;
	l_oSource.setData(i_s);
	QXmlSimpleReader l_oReader;
	l_oReader.setContentHandler(&l_oHandler);
	if (!l_oReader.parse(l_oSource))
	{
		//qDebug()<<"parse error!!";
		clear_diagram();
	}
}

void box_view::clear_diagram()
{
	foreach (box_link *l_o, m_oLinks) {
		l_o->hide();
		scene()->removeItem(l_o);
		delete l_o;
	}
	m_oLinks.clear();
	foreach (connectable *l_o, m_oItems.values()) {
		QGraphicsItem* l_oTmp = dynamic_cast<QGraphicsItem*>(l_o);
		l_oTmp->hide();
		scene()->removeItem(l_oTmp);
		delete l_o;
	}
	scene()->update();
	m_oItems.clear();
}

void box_view::check_canvas_size()
{
	QRectF br = scene()->itemsBoundingRect();
	br.adjust(-GAP, -GAP, GAP, GAP);
	br = br.united(scene()->sceneRect());
	scene()->setSceneRect(br);
}

void box_view::focusInEvent(QFocusEvent *i_oEv)
{
	QGraphicsView::focusInEvent(i_oEv);
	m_oMediator->notify_focus(this);
}

void box_view::focusOutEvent(QFocusEvent *i_oEv)
{
	QGraphicsView::focusOutEvent(i_oEv);
}

void box_view::notify_add_box(int id, int box)
{
	Q_ASSERT(m_iId == id);
	data_item& item = m_oMediator->m_oItems[m_iId];
	data_box *db = item.m_oBoxes[box];
	connectable *l_o = NULL;
	if (db->m_iType == data_box::ACTIVITY)
	{
		l_o = new box_item(this, box);
	}
	else if (db->m_iType == data_box::LABEL)
	{
		l_o = new box_label(this, box);
	}
	else if (db->m_iType == data_box::ACTOR)
	{
		l_o = new box_actor(this, box);
	}
	else if (db->m_iType == data_box::USECASE)
	{
		l_o = new box_usecase(this, box);
	}
	else if (db->m_iType == data_box::COMPONENT)
	{
		l_o = new box_component(this, box);
	}
	else if (db->m_iType == data_box::RECTANGLE)
	{
		l_o = new box_rectangle(this, box);
	}
	else if (db->m_iType == data_box::NODE)
	{
		l_o = new box_node(this, box);
	}
	else if (db->m_iType == data_box::DECISION)
	{
		l_o = new box_decision(this, box);
	}
	else if (db->m_iType == data_box::ACTIVITY_START)
	{
		l_o = new box_dot(this, box);
	}
	else if (db->m_iType == data_box::MATRIX)
	{
		l_o = new box_matrix(this, box);
	}
	else if (db->m_iType == data_box::SEQUENCE)
	{
		l_o = new box_sequence(this, box);
	}
	else if (db->m_iType == data_box::FRAME)
	{
		l_o = new box_frame(this, box);
	}
	else if (db->m_iType == data_box::CLASS)
	{
		l_o = new box_class(this, box);
	}
	else if (db->m_iType == data_box::ENTITY)
	{
		l_o = new box_entity(this, box);
	}
	else if (db->m_iType == data_box::ACTIVITY_PARALLEL)
	{
		l_o = new box_fork(this, box);
	}
	else if (db->m_iType == data_box::DATABASE)
	{
		l_o = new box_database(this, box);
	}
	else if (db->m_iType == data_box::PIPE)
	{
		l_o = new box_pipe(this, box);
	}
	Q_ASSERT(l_o != NULL);
	m_oItems[box] = l_o;
	l_o->update_data();
}

void box_view::notify_del_box(int id, int box)
{
	connectable *l_o = m_oItems.value(box);
	Q_ASSERT(l_o!=NULL);
	QGraphicsItem *l_oTmp = dynamic_cast<QGraphicsItem*>(l_o);
	l_oTmp->hide();
	scene()->removeItem(l_oTmp);
	m_oItems.remove(box);
	delete l_o;
	scene()->update();
}

void box_view::notify_link_box(int id, data_link* link)
{
	box_link *l_o = m_oCurrent;
	if (!l_o)
	{
		l_o = new box_link(this);
	}
	m_oLinks.push_back(l_o);
	l_o->m_oInnerLink.copy_from(*link);
	l_o->m_oLink = link;
	l_o->update_text();
	l_o->update_pos();
}

void box_view::notify_unlink_box(int id, data_link* link)
{
	Q_ASSERT(!m_oCurrent);
	foreach (box_link *cur, m_oLinks) {
		if (cur->m_oLink == link)
		{
			delete cur;
			m_oLinks.removeAll(cur);
			break;
		}
	}
}

void box_view::notify_change_link_box(int id, data_link*link)
{
	foreach (box_link *cur, m_oLinks)
	{
		if (cur->m_oLink == link)
		{
			cur->m_oInnerLink.copy_from(*cur->m_oLink);
			cur->update_pos();
			break;
		}
	}
}

void box_view::wheelEvent(QWheelEvent *i_oEvent)
{
	bool l_bCtrl = i_oEvent->modifiers() & Qt::ControlModifier;
	if (m_oMediator->m_oWindef->m_bUseTouchpad xor l_bCtrl)
	{
		QGraphicsView::wheelEvent(i_oEvent);
		return;
	}

	QPointF l_o = mapToScene(i_oEvent->pos());
	qreal i_iScaleFactor = pow(2.0, i_oEvent->delta() / 440.0);
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;

	if (scene()->selectedItems().size())
	{
		QRectF l_oRect = scene()->selectedItems().at(0)->sceneBoundingRect();
		foreach (QGraphicsItem *l_o, scene()->selectedItems())
		{
			l_oRect |= l_o->sceneBoundingRect();
		}
		l_oRect = QRectF(l_oRect.topLeft() - QPointF(10, 10), l_oRect.bottomRight() + QPointF(10, 10));

		QRectF l_oViewRect = viewport()->rect();
		QRectF l_oNewRect = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(l_oRect);
		if (l_oNewRect.width() > l_oViewRect.width() or l_oNewRect.height() > l_oViewRect.height())
		{
			return;
		}
		scale(i_iScaleFactor, i_iScaleFactor);
		centerOn(l_o + mapToScene(viewport()->rect().center()) - mapToScene(i_oEvent->pos()));
		ensureVisible(l_oRect, 5, 5);
	}
	else
	{
		QRectF l_oRect = scene()->itemsBoundingRect();
		l_oRect = QRectF(l_oRect.topLeft() - QPointF(PIPAD, PIPAD), l_oRect.bottomRight() + QPointF(PIPAD, PIPAD));
		l_oRect = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(l_oRect);

		QRectF l_oViewRect = viewport()->rect();
		if (i_rFactor < 1 and i_iScaleFactor < 1 and 1.1 * l_oRect.width() < l_oViewRect.width() and 1.1 * l_oRect.height() < l_oViewRect.height())
		{
			ensureVisible(scene()->itemsBoundingRect(), 10, 10);
			return;
		}

		scale(i_iScaleFactor, i_iScaleFactor);
		centerOn(l_o + mapToScene(viewport()->rect().center()) - mapToScene(i_oEvent->pos()));
	}

}

void box_view::fit_zoom()
{
	if (scene()->selectedItems().size())
	{
		QRectF l_oRect = scene()->selectedItems().at(0)->sceneBoundingRect();
		foreach (QGraphicsItem *l_o, scene()->selectedItems())
		{
			l_oRect |= l_o->sceneBoundingRect();
		}
		l_oRect = QRectF(l_oRect.topLeft() - QPointF(PIPAD, PIPAD), l_oRect.bottomRight() + QPointF(PIPAD, PIPAD));
		fitInView(l_oRect, Qt::KeepAspectRatio);
	}
	else
	{
		fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
	}
}

void box_view::keyPressEvent(QKeyEvent *i_oEvent)
{
	if (i_oEvent->key() == Qt::Key_Enter || i_oEvent->key() == Qt::Key_Return)
	{
		slot_edit_properties();
	}
	QGraphicsView::keyPressEvent(i_oEvent);
}

void box_view::keyReleaseEvent(QKeyEvent *i_oEvent)
{
	QGraphicsView::keyReleaseEvent(i_oEvent);
	setCursor(Qt::ArrowCursor);
}

void box_view::slot_size()
{
	if (scene()->selectedItems().size() < 2) return;

	int l_i = ((QAction*) QObject::sender())->data().toInt();
	int w = 0;
	int h = 0;
	foreach (QGraphicsItem* l_oItem, scene()->selectedItems())
	{
		if (connectable* c = dynamic_cast<connectable*>(l_oItem))
		{
			QRectF r = c->rectPos();
			if ((l_i == SAME_WIDTH || l_i == SAME_WIDTH_HEIGHT) && r.width() > w)
			{
				w = r.width();
			}
			if ((l_i == SAME_HEIGHT || l_i == SAME_WIDTH_HEIGHT) && r.height() > h)
			{
				h = r.height();
			}
		}
	}
	mem_size_box *mem = new mem_size_box(m_oMediator, m_iId);
	foreach (QGraphicsItem* l_oItem, scene()->selectedItems())
	{
		if (connectable* c = dynamic_cast<connectable*>(l_oItem))
		{
			data_box *box = c->m_oBox;
			mem->prev_values[box] = QRect(box->m_iXX, box->m_iYY, box->m_iWW, box->m_iHH);
			mem->next_values[box] = QRect(box->m_iXX, box->m_iYY, w, h);
		}
	}
	mem->apply();
}

void box_view::slot_text_align()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();
	Qt::AlignmentFlag l_oAlign = Qt::AlignCenter;
	if (l_i == ALIGN_LEFT)
	{
		l_oAlign = Qt::AlignLeft;
	}
	else if (l_i == ALIGN_RIGHT)
	{
		l_oAlign = Qt::AlignRight;
	}

	mem_text_align_box *l_oMem = new mem_text_align_box(m_oMediator, m_iId);
	l_oMem->m_oAlign = l_oAlign;
	foreach (QGraphicsItem* l_oItem, scene()->selectedItems())
        {
		if (connectable* c = dynamic_cast<connectable*>(l_oItem))
		{
			data_box *box = c->m_oBox;
			l_oMem->m_oPrevValues.append(*box);
		}
	}
	l_oMem->apply();
}

void box_view::slot_align()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();

	int n = 0;
	qreal best = 0;
	if (l_i == ALIGN_LEFT || l_i == ALIGN_TOP)
		best =  99999999;
	else if (l_i == ALIGN_BOTTOM || l_i == ALIGN_RIGHT)
		best = -99999999;

	foreach (QGraphicsItem* l_oItem, scene()->selectedItems())
	{
		if (connectable* c = dynamic_cast<connectable*>(l_oItem))
		{
			n++;
			QRectF r = c->rectPos();
			if (l_i == ALIGN_LEFT)
				best = qMin(r.x(), best);
			if (l_i == ALIGN_RIGHT)
				best = qMax(r.x() + r.width(), best);
			if (l_i == ALIGN_BOTTOM)
				best = qMax(r.y() + r.height(), best);
			if (l_i == ALIGN_TOP)
				best = qMin(r.y(), best);
			if (l_i == ALIGN_CENTER)
				best = r.x() + r.width() / 2.; // TODO do not pick one randomly
			if (l_i == ALIGN_MIDDLE)
				best = r.y() + r.height()/2.;
		}
	}

	if (l_i == ALIGN_CENTER || l_i == ALIGN_MIDDLE)
		best = (int) best;

	mem_pos_box *mem = new mem_pos_box(m_oMediator, m_iId);

	foreach (QGraphicsItem* l_oItem, scene()->selectedItems())
	{
		if (connectable* c = dynamic_cast<connectable*>(l_oItem))
		{
			data_box *box = c->m_oBox;
			mem->prev_values[box] = QPointF(box->m_iXX, box->m_iYY);
			switch (l_i)
			{
				case ALIGN_LEFT:
					mem->next_values[box] = QPoint(best, box->m_iYY);
					break;
				case ALIGN_RIGHT:
					mem->next_values[box] = QPoint(best - box->m_iWW, box->m_iYY);
					break;
				case ALIGN_TOP:
					mem->next_values[box] = QPoint(box->m_iXX, best);
					break;
				case ALIGN_BOTTOM:
					mem->next_values[box] = QPoint(box->m_iXX, best - box->m_iHH);
					break;
				case ALIGN_CENTER:
					mem->next_values[box] = QPoint(best - box->m_iWW / 2., box->m_iYY);
					break;
				case ALIGN_MIDDLE:
					mem->next_values[box] = QPoint(box->m_iXX, best - box->m_iHH / 2.);
					break;
				default:
					Q_ASSERT(false);
			}
		}
	}
	mem->apply();
}

void box_view::mouseDoubleClickEvent(QMouseEvent* i_oEv)
{
	if (i_oEv->button() != Qt::LeftButton)
	{
		//qDebug()<<"left button on click, leaving";
		return;
	}

	QGraphicsItem *l_oItem = itemAt(i_oEv->pos());
	if (l_oItem)
	{
		editable *l_o = dynamic_cast<editable*>(l_oItem);
		if (l_o)
		{
			l_o->properties();
		}
	}
	else
	{
		slot_add_item();
	}
}

void box_view::mousePressEvent(QMouseEvent *i_oEv)
{
	if (m_oMenu == NULL) {
		init_menu();
	}

	if (i_oEv->button() == Qt::RightButton)
	{
		m_oLastPoint = mapToScene(i_oEv->pos());
		enable_menu_actions();
		m_oMenu->popup(i_oEv->globalPos());
		return;
	}

	m_bPressed = true;
	m_oLastMovePoint = m_oLastPoint = mapToScene(i_oEv->pos());

	if (i_oEv->button() == Qt::MidButton)
	{
		m_oScrollPoint = i_oEv->pos();
		viewport()->setCursor(Qt::OpenHandCursor);
		m_bScroll = true;
		return;
	}

	box_item *k = NULL;
	box_chain *kk = NULL;
	foreach (QGraphicsItem *l_oTmp, items(i_oEv->pos()))
	{
		if (!kk)
		{
			kk = dynamic_cast<box_chain*>(l_oTmp);
			if (kk)
			{
				break;
			}
		}
		if (!k)
		{
			k = dynamic_cast<box_item*>(l_oTmp);
			if (k )
			{
				break;
			}
		}
	}

	if (kk)
	{
		Q_ASSERT(!m_oCurrent);

		foreach (QGraphicsItem *l_o, scene()->selectedItems())
		{
			l_o->setSelected(false);
		}

		connectable *l_oParent = dynamic_cast<connectable*>(kk->parentItem());
		Q_ASSERT(l_oParent);

		QPoint p = QPoint(m_oLastPoint.x(), m_oLastPoint.y());

		m_oCurrent = new box_link(this);
		m_oCurrent->m_oInnerLink.m_iParent = l_oParent->m_iId;
		m_oCurrent->m_oInnerLink.m_iParentPos = l_oParent->optimize_position(p);
		m_oCurrent->m_oInnerLink.m_iChild = NO_ITEM;
		m_oCurrent->m_oInnerLink.m_iChildPos = data_link::NORTH;
		m_oCurrent->m_oInnerLink.m_oStartPoint = m_oCurrent->m_oInnerLink.m_oEndPoint = p;
		m_oCurrent->m_oInnerLink.m_iId = m_oMediator->next_box_link_seq(m_iId);
		m_oCurrent->m_oInnerLink.m_iLeftArrow = m_oMediator->m_oLinkState.m_iLeftArrow;
		m_oCurrent->m_oInnerLink.m_iRightArrow = m_oMediator->m_oLinkState.m_iRightArrow;
		m_oCurrent->m_oInnerLink.m_iLineType = m_oMediator->m_oLinkState.m_iLineType;
		m_oCurrent->m_oInnerLink.pen_style = m_oMediator->m_oLinkState.pen_style;

		if (dynamic_cast<box_label*>(l_oParent))
		{
			m_oCurrent->m_oInnerLink.pen_style = Qt::DotLine;
			m_oCurrent->m_oInnerLink.m_iLeftArrow = data_link::NONE;
			m_oCurrent->m_oInnerLink.m_iRightArrow = data_link::NONE;
			m_oCurrent->m_oInnerLink.m_iLineType = data_link::LINE;
		}

		m_oCurrent->setSelected(true);
	}
	else if (k)
	{
		QPointF l_o = k->pos();
		// TODO used by the handle for resizing the boxes - check that the click was on the handle
		if (m_oLastPoint.x() + m_oLastPoint.y() - l_o.x() - l_o.y() >
			k->rect().width() + k->rect().height() - 2*GRID)
		{
			m_oOffsetPoint = QPointF(k->rect().width(), k->rect().height());
		}
		else
		{
			m_oOffsetPoint = QPointF(-1, -1);
		}
	}

	QGraphicsView::mousePressEvent(i_oEv);
}

void box_view::mouseMoveEvent(QMouseEvent *i_oEv)
{
	if (m_bScroll)
	{
		QScrollBar *l_h_bar = horizontalScrollBar();
		QScrollBar *l_v_bar = verticalScrollBar();

		QPoint l_o = i_oEv->pos() - m_oScrollPoint;

		l_h_bar->setValue(l_h_bar->value() + (isRightToLeft() ? l_o.x() : - l_o.x()));
		l_v_bar->setValue(l_v_bar->value() - l_o.y());

		m_oScrollPoint = i_oEv->pos();
		return;
	}

	if (m_bPressed) m_oLastMovePoint = mapToScene(i_oEv->pos());
	QGraphicsView::mouseMoveEvent(i_oEv);
}

void box_view::mouseReleaseEvent(QMouseEvent *i_oEv)
{
	if (m_oCurrent)
	{
		if (m_oItems.value(m_oCurrent->m_oInnerLink.m_iChild))
		{
			mem_link_box *ln = new mem_link_box(m_oMediator, m_iId);
			ln->link = new data_link();
			ln->link->copy_from(m_oCurrent->m_oInnerLink);
			ln->apply();

			m_oCurrent = NULL;
		}
		else
		{
			// assume cancel
			delete m_oCurrent;
			m_oCurrent = NULL;
		}
		m_bPressed = false;
		QGraphicsView::mouseReleaseEvent(i_oEv);
		return;
	}


	if (m_bScroll)
	{
		QGraphicsView::mouseReleaseEvent(i_oEv);
		m_bPressed = false;
		m_bScroll = false;
		viewport()->setCursor(Qt::ArrowCursor);
		return;
	}

	QGraphicsView::mouseReleaseEvent(i_oEv);

	m_bPressed = false;
	{
		//if (qAbs(p->m_iXX - v->pos().x()) + qAbs(p->m_iYY - v->pos().y()) > 0.1) {

		QPointF p = mapToScene(i_oEv->pos()) - m_oLastPoint;
		if (qAbs(p.x()) > 3 || qAbs(p.y()) > 3)
		{
			mem_pos_box *mem = new mem_pos_box(m_oMediator, m_iId);
			foreach (QGraphicsItem *l_oI1, scene()->selectedItems())
			{
				if (connectable* item = dynamic_cast<connectable*>(l_oI1))
				{
					data_box *box = item->m_oBox;
					mem->prev_values[box] = QPointF(box->m_iXX, box->m_iYY);
					mem->next_values[box] = l_oI1->pos();
				}
			}
			if (mem->prev_values.size() > 0)
			{
				mem->apply();
			}
			else
			{
				delete mem;
			}
		}
	}
	check_canvas_size();
}

void box_view::notify_box_props(int id, const QList<diagram_item*>& items)
{
	Q_ASSERT(id == m_iId);
	foreach (diagram_item* it, items) {
		foreach (box_link *lnk, m_oLinks)
		{
			if (lnk->m_oLink == it) {
				data_link* dat = static_cast<data_link*>(it);
				lnk->m_oInnerLink.copy_from(*dat);
				lnk->update_text();
				lnk->update_pos();
				lnk->update();
				goto end;
			}
		}

		foreach (connectable *box, m_oItems.values())
		{
			if (box->m_oBox == it) {
				(dynamic_cast<QGraphicsItem*>(box))->update();
				goto end;
			}
		}
		end:
			;
	}
}

void box_view::notify_pos_box(int id, const QList<data_box*>& items)
{
	Q_ASSERT(id == m_iId);
	foreach (data_box *box, items)
	{
		m_oItems[box->m_iId]->update_data();
	}
}

void box_view::notify_text_align(int id, const QList<data_box>& items)
{
	Q_ASSERT(id == m_iId);
	foreach (data_box box, items)
	{
		m_oItems[box.m_iId]->update_data();
	}
}

void box_view::notify_size_box(int id, const QList<data_box*>& items)
{
	Q_ASSERT(id == m_iId);
	foreach (data_box *box, items)
	{
		Q_ASSERT(m_oItems.contains(box->m_iId));
		m_oItems[box->m_iId]->update_size();
	}
}

void box_view::notify_sequence_box(int i_iId, int i_iBoxId)
{
	Q_ASSERT(i_iId == m_iId);
	m_oItems[i_iBoxId]->update_data();
}

int box_view::next_seq()
{
	do {
		++num_seq;
	} while (m_oItems.contains(num_seq));
	return num_seq;
}

void box_view::notify_focus(void* ptr)
{
	bool cond = ptr == this;
	m_oAddItemAction->setEnabled(cond);
	m_oDeleteAction->setEnabled(cond);
	//m_oEditAction->setEnabled(cond);
}

void box_view::message(const QString &s, int d)
{
	emit sig_message(s, d);
}

bool box_view::slot_import_from_file() {
	QUrl l_o = QFileDialog::getOpenFileUrl(this, i18n("Choose a file to open"),
                QUrl(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
		i18n("*.semd|Semantik diagram (*.semd)"));
	return import_from_file(l_o);
}

bool box_view::import_from_file(const QUrl& l_o)
{
	if (l_o.path().isEmpty()) {
		return false;
	}

	// use a full semantik document, even if we are only interested in one item
	sem_mediator *x = new sem_mediator(this);

	bool l_bOk = false;
	if (x->open_raw(l_o.path()) && x->m_oItems.size() == 1) {
		l_bOk = true;
		data_item& tmp = x->m_oItems[x->m_oItems.keys().at(0)];

		mem_import_box *imp = new mem_import_box(m_oMediator, m_iId);
		imp->init(tmp.m_oBoxes.values(), tmp.m_oLinks);
		imp->m_iNewFont = tmp.m_oDiagramFont;
		imp->m_oOldColorSchemes = m_oMediator->m_oColorSchemes;
		imp->m_oNewColorSchemes = x->m_oColorSchemes;
		imp->apply();

		m_oCurrentUrl = l_o;
		emit sig_Url(m_oCurrentUrl);
	}

	delete x;

	return l_bOk;
}

bool box_view::slot_export_to_file() {
	choose_export:
	QUrl l_o = QFileDialog::getSaveFileUrl(this, i18n("Choose a file name"),
                QUrl(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
		i18n("*.semd|Semantik diagram (*.semd)"));

	if (l_o.path().isEmpty()) return false;
	if (!l_o.path().endsWith(notr(".semd")))
	{
		l_o = QUrl(l_o.path()+notr(".semd"));
	}

	m_oCurrentUrl = l_o;
	emit sig_Url(m_oCurrentUrl);

	// now the magic
	sem_mediator *x = new sem_mediator(this);
	Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	l_oData.m_iDataType = VIEW_DIAG;
	x->m_oItems.insert(1, l_oData);
	x->m_oColorSchemes = m_oMediator->m_oColorSchemes;

	if (x->save_file(l_o.path()))
	{
		emit sig_message(i18n("Saved '%1'", l_o.path()), 2000);
		m_oMediator->set_dirty(false);
		return true;
	}
	else
	{
		int mu = KMessageBox::questionYesNo(NULL, //this,
		i18n("The file \"%1\" could not be saved because an error happened.\nTry again?", l_o.path()),
		i18n("Try again?"),
		KStandardGuiItem::yes(),
		KStandardGuiItem::no(),
		notr("ContinueSaveAs"));
		if (!mu) {
			goto choose_export;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool box_view::slot_save() {
	if (m_oCurrentUrl.isValid())
	{
		sem_mediator *x = new sem_mediator(this);
		Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
		data_item& l_oData = m_oMediator->m_oItems[m_iId];
		l_oData.m_iDataType = VIEW_DIAG;
		x->m_oItems.insert(1, l_oData);
		x->m_oColorSchemes = m_oMediator->m_oColorSchemes;

		if (x->save_file(m_oCurrentUrl.path())) {
			m_oMediator->set_dirty(false);
			emit sig_message(i18n("Saved '%1'", m_oCurrentUrl.path()), 2000);
			return true;
		}
	}
	else
	{
		return slot_export_to_file();
	}
	return false;
}

QRectF box_view::drawThumb(QPainter* i_oPainter, QRectF& i_oRect, int i_iId)
{
	clear_diagram();

	m_iId = i_iId;
	Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	if (l_oData.m_iDataType == VIEW_DIAG)
	{
		if (!l_oData.m_sDiag.isEmpty())
		{
			from_string(l_oData.m_sDiag);
			l_oData.m_sDiag = notr("");
		}
		sync_view();
		check_canvas_size();
	}
	else
	{
		return QRectF();
	}

	QRectF l_oRect = scene()->itemsBoundingRect();

	l_oRect = l_oRect.adjusted(-15, -15, 15, 15);
	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());

	QRectF l_oDrawRect(i_oRect);

	if (l_oRect.width() > l_oRect.height())
	{
		l_oDrawRect.setHeight(l_oDrawRect.height() * (1. * l_oRect.height() / l_oRect.width()));
	}
	else
	{
		l_oDrawRect.setWidth(l_oDrawRect.width() * (1. * l_oRect.width() / l_oRect.height()));
	}

	i_oPainter->save();
	QPen l_oPen;
	l_oPen.setCosmetic(true);
	i_oPainter->setPen(l_oPen);
	i_oPainter->setBrush(Qt::white);
	i_oPainter->drawRect(l_oDrawRect);
	i_oPainter->restore();

	scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene()->render(i_oPainter, i_oRect, l_oRect, Qt::KeepAspectRatio);
	scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
	return l_oDrawRect;
}


int box_view::batch_print_map(const QUrl& i_oUrl, QPair<int, int> & p)
{
	QString url = i_oUrl.path();
	QRectF l_oRect = scene()->itemsBoundingRect();

	l_oRect = l_oRect.adjusted(-15, -15, 15, 15);

	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());

	if (p.first != 0) {
		l_oR.setWidth(p.first);
		if (p.second == 0) {
			l_oR.setHeight((p.first * l_oRect.height()) / (double) l_oRect.width());
		}
	}
	if (p.second != 0) {
		l_oR.setHeight(p.second);
		if (p.first == 0) {
			l_oR.setWidth((p.second * l_oRect.width()) / (double) l_oRect.height());
		}
	}

	Qt::AspectRatioMode rat = (p.first == 0 || p.second == 0) ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;

	if (url.endsWith("png")) {
		// fill with white
		QImage l_oImage((int) l_oR.width(), (int) l_oR.height(), QImage::Format_RGB32);
		l_oImage.fill(qRgb(255,255,255));

		QPainter l_oP;
		l_oP.begin(&l_oImage);
		l_oP.setRenderHints(QPainter::Antialiasing);
		scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
		scene()->render(&l_oP, l_oR, l_oRect, rat);
		scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
		l_oP.end();

		l_oImage.save(url);
	} else if (url.endsWith("pdf") or url.endsWith("ps") or url.endsWith("eps")) {
		QPrinter l_oPrinter;
		l_oPrinter.setOrientation(QPrinter::Portrait);
		if (url.endsWith("pdf"))
		{
			l_oPrinter.setOutputFormat(QPrinter::PdfFormat);
		}
		else
		{
			l_oPrinter.setOutputFormat(QPrinter::NativeFormat);
			l_oPrinter.setResolution(QPrinter::HighResolution);
		}
		l_oPrinter.setPaperSize(l_oR.size(), QPrinter::DevicePixel);
		l_oPrinter.setPageMargins(0, 0, 0, 0, QPrinter::DevicePixel);
		l_oPrinter.setOutputFileName(url);

		QPainter l_oPdf;
		if (l_oPdf.begin(&l_oPrinter))
		{
			m_bDisableGradient = true;
			scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
			scene()->render(&l_oPdf, l_oR, l_oRect, rat);
			scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
			l_oPdf.end();
			m_bDisableGradient = false;
		}
	}
	else if (url.endsWith("svg"))
	{
		QSvgGenerator l_oGenerator;
		l_oGenerator.setFileName(url);
		l_oGenerator.setSize(QSize(l_oR.width(), l_oR.height()));
		l_oGenerator.setViewBox(l_oR);
		l_oGenerator.setTitle(i18n("Semantik diagram"));
		l_oGenerator.setDescription(notr("Generated by Semantik, if it does not render properly check your system fonts!"));
		l_oGenerator.setResolution(QApplication::desktop()->logicalDpiX());

		QPainter l_oP;
		l_oP.begin(&l_oGenerator);
		l_oP.setRenderHints(QPainter::Antialiasing);
		scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
		scene()->render(&l_oP, l_oR, l_oRect, rat);
		scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
		l_oP.end();
	} else {
		return 12;
	}
	return 0;
}

// The following is copy-pasted. Watch carefully
void box_view::slot_print()
{
        QPrinter *l_oP = new QPrinter;

	QRectF l_oRect = scene()->itemsBoundingRect();
	foreach (QGraphicsItem*it, scene()->items())
	{
		it->setCacheMode(QGraphicsItem::NoCache); // the magic happens here
	}

	l_oRect = l_oRect.adjusted(-15, -15, 15, 15);

	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());

	l_oP->setOrientation(QPrinter::Portrait);
	l_oP->setOutputFormat(QPrinter::PdfFormat);
	l_oP->setPaperSize(l_oR.size(), QPrinter::DevicePixel);
	l_oP->setPageMargins(0, 0, 0, 0, QPrinter::DevicePixel);

        QPrintDialog l_oD(l_oP, this);
        if (l_oD.exec() != QDialog::Accepted)
        {
		emit sig_message(i18n("Printing cancelled"), 3000);
		return;
	}

	QPainter l_oPdf;
	if (l_oPdf.begin(l_oP))
	{
		m_bDisableGradient = true;
		scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
		scene()->render(&l_oPdf, QRectF(), l_oRect, Qt::KeepAspectRatio);
		scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
		l_oPdf.end();
		m_bDisableGradient = false;
		emit sig_message(i18n("Printing completed"), 5000);
	}
	else
	{
		emit sig_message(i18n("Problem during printing :-("), 5000);
	}
}

void box_view::slot_copy_picture()
{
	QRectF l_oRect = scene()->itemsBoundingRect();
	l_oRect = l_oRect.adjusted(-15, -15, 15, 15);

	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());
	Qt::AspectRatioMode rat = Qt::KeepAspectRatio;

	QImage l_oImage((int) l_oR.width(), (int) l_oR.height(), QImage::Format_RGB32);
	l_oImage.fill(qRgb(255,255,255));

	QPainter l_oP;
	l_oP.begin(&l_oImage);
	l_oP.setRenderHints(QPainter::Antialiasing);
	scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene()->render(&l_oP, l_oR, l_oRect, rat);
	scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
	l_oP.end();

	//QApplication::clipboard()->setMimeType("application/x-png");
	QApplication::clipboard()->setImage(l_oImage);
}

void box_view::notify_change_properties(void *)
{
	// this may be the wrong approach entirely
	Q_ASSERT(m_oMediator->m_oItems.contains(m_iId));
	data_item& l_oData = m_oMediator->m_oItems[m_iId];
	if (l_oData.m_oDiagramFont != font()) {
		scene()->setFont(l_oData.m_oDiagramFont);
		foreach (QGraphicsItem *l_o, scene()->items())
		{
			if (connectable* t = dynamic_cast<connectable*>(l_o))
			{
				t->update_size();
			}
			else if (box_link *t = dynamic_cast<box_link*>(l_o))
			{
				t->update_text();
				t->update_pos();
			}
			l_o->update();
		}
	}
}

void box_view::export_fig_size()
{
	QRectF l_oRect = scene()->itemsBoundingRect();
	l_oRect = QRectF(l_oRect.topLeft() - QPointF(PIPAD, PIPAD), l_oRect.bottomRight() + QPointF(PIPAD, PIPAD));

	export_fig_dialog* exp = new export_fig_dialog(this);

	exp->kurlrequester->setMode(KFile::File | KFile::LocalOnly);
	exp->kurlrequester->setFilter(i18n("*.png|PNG Files (*.png)\n*.svg|SVG Files (*.svg)\n*.pdf|PDF Files (*.pdf)"));

	exp->kurlrequester->setUrl(QUrl(m_oMediator->m_sExportUrl));
	exp->m_oWidthC->setChecked(m_oMediator->m_bExportIsWidth);
	exp->m_iBaseWidth = l_oRect.width();
	exp->m_iBaseHeight = l_oRect.height();

	if (m_oMediator->m_bExportIsWidth)
	{
		if (m_oMediator->m_iExportWidth > 0)
			exp->m_oWidth->setValue(m_oMediator->m_iExportWidth);
		else
			exp->m_oWidth->setValue(l_oRect.width());
	}
	else
	{
		if (m_oMediator->m_iExportHeight > 0)
			exp->m_oHeight->setValue(m_oMediator->m_iExportHeight);
		else
			exp->m_oHeight->setValue(l_oRect.height());
	}

	if (exp->exec() == QDialog::Accepted)
	{
		if (m_oMediator->m_iExportWidth != exp->m_oWidth->value())
		{
			m_oMediator->m_iExportWidth = exp->m_oWidth->value();
			m_oMediator->set_dirty();
		}

		if (m_oMediator->m_iExportHeight != exp->m_oHeight->value())
		{
			m_oMediator->m_iExportHeight = exp->m_oHeight->value();
			m_oMediator->set_dirty();
		}

		if (!exp->kurlrequester->url().isValid() || exp->kurlrequester->url().isEmpty())
		{
			emit sig_message(i18n("No destination file selected"), 5000);
			return;
		}

		if (m_oMediator->m_sExportUrl != exp->kurlrequester->url().url())
		{
			m_oMediator->m_sExportUrl = exp->kurlrequester->url().url();
			m_oMediator->set_dirty();
		}

		QPair<int, int> p;
		if (exp->m_oWidthC->isChecked()) {
			p.first = exp->m_oWidth->value();
		} else {
			p.second = exp->m_oHeight->value();
		}

		// TODO upload remote files?
		QUrl url = exp->kurlrequester->url();
		if (url.isRelative()) {
			url.setPath(QDir::homePath() + notr("/") + url.toLocalFile());
		}
		int status = batch_print_map(url, p);
		if (status == 0)
			emit sig_message(i18n("Exported '%1'", url.fileName()), 2000);
		else
			KMessageBox::sorry(this, i18n("Could not save to %1", url.fileName()), i18n("Missing picture"));
	}
}


