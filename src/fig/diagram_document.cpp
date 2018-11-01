// Thomas Nagy 2013-2018 GPLV3

#include <QtGui>
#include <QFile>
#include <QTextBrowser>
#include <QGridLayout>
#include <QProgressDialog>
#include <KDE/KStatusBar>
#include <QFileDialog>
#include <KConfigGroup>
#include <QMenuBar>
#include <KDE/KApplication>
#include <KStandardAction>
#include <KRecentFilesAction>
#include <KActionCollection>
#include<KToolBar>
#include <QMenu>
#include <KMessageBox>
#include <ktip.h>
#include <QFrame>

#include "diagram_document.h"
#include "sem_mediator.h"
#include "box_view.h"
# include  "sembind.h"
#include "mem_base.h"

diagram_document::diagram_document(QWidget *i_oParent) : QFrame(i_oParent)
{
        setLineWidth(0);
        setFrameStyle(QFrame::NoFrame);

	m_oMediator = new sem_mediator(this);
        m_oDiagramView = new box_view(this, m_oMediator);

        QGridLayout *ll = new QGridLayout(this);
        ll->addWidget(m_oDiagramView);
}

void diagram_document::init()
{
	m_oMediator->m_bIsDiagram = true;

	connect(m_oMediator, SIGNAL(sig_add_item(int)), m_oDiagramView, SLOT(notify_add_item(int)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oDiagramView, SLOT(notify_select(const QList<int>&, const QList<int>&)));

	connect(m_oMediator, SIGNAL(sig_add_box(int, int)), m_oDiagramView, SLOT(notify_add_box(int, int)));
	connect(m_oMediator, SIGNAL(sig_del_box(int, int)), m_oDiagramView, SLOT(notify_del_box(int, int)));
	connect(m_oMediator, SIGNAL(sig_edit_box(int, int)), m_oDiagramView, SLOT(notify_edit_box(int, int)));
	connect(m_oMediator, SIGNAL(sig_link_box(int, data_link*)), m_oDiagramView, SLOT(notify_link_box(int, data_link*)));
	connect(m_oMediator, SIGNAL(sig_unlink_box(int, data_link*)), m_oDiagramView, SLOT(notify_unlink_box(int, data_link*)));
	connect(m_oMediator, SIGNAL(sig_change_link_box(int, data_link*)), m_oDiagramView, SLOT(notify_change_link_box(int, data_link*)));
	connect(m_oMediator, SIGNAL(sig_box_props(int, const QList<diagram_item*>&)), m_oDiagramView, SLOT(notify_box_props(int, const QList<diagram_item*>&)));
	connect(m_oMediator, SIGNAL(sig_pos_box(int, const QList<data_box*>&)), m_oDiagramView, SLOT(notify_pos_box(int, const QList<data_box*>&)));
	connect(m_oMediator, SIGNAL(sig_size_box(int, const QList<data_box*>&)), m_oDiagramView, SLOT(notify_size_box(int, const QList<data_box*>&)));
	connect(m_oMediator, SIGNAL(sig_text_align(int, const QList<data_box>&)), m_oDiagramView, SLOT(notify_text_align(int, const QList<data_box>&)));
	connect(m_oMediator, SIGNAL(sig_focus(void *)), m_oDiagramView, SLOT(notify_focus(void *)));
	connect(m_oMediator, SIGNAL(sig_change_properties(void *)), m_oDiagramView, SLOT(notify_change_properties(void *)));

	connect(m_oDiagramView, SIGNAL(sig_Url(const QUrl&)), this, SLOT(slot_tab_name(const QUrl&)));

	mem_add *add = new mem_add(m_oMediator);
	add->item.m_iXX = 0;
	add->item.m_iYY = 0;
	add->parent = NO_ITEM;
	add->apply();

	m_oDiagramView->scene()->setFont(add->item.m_oDiagramFont);

	m_oMediator->notify_focus(m_oDiagramView);

	m_oMediator->m_oUndoStack.clear();
	m_oMediator->m_oRedoStack.clear();

	m_oMediator->init_colors();

	m_oMediator->set_dirty(false);
}

diagram_document::~diagram_document()
{
	m_oMediator->disconnect();
	delete m_oMediator;
	delete m_oDiagramView;
}

void diagram_document::slot_tab_name(const QUrl& i_oUrl)
{
	emit sig_tab_name(this, i_oUrl);
}

void diagram_document::slot_open() {
}

