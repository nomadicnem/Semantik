// Thomas Nagy 2007-2018 GPLV3

#include <QtGui>
#include <QFile>
 # include <QDir>
#include <QTextBrowser>
#include <QProgressDialog>
#include <KDE/KStatusBar>
#include <QFileDialog>
#include <KConfigGroup>
#include <QMenuBar>
#include <KDE/KApplication>
#include <KStandardAction>
#include <KRecentFilesAction>
#include <KActionCollection>
#include <KToolBar>
#include <QMenu>
#include <QIcon>
#include <KMessageBox>
#include <ktip.h>
#include <windef.h>
#include <QFrame>
#include <QtWidgets>
#include <QUuid>
#include <QStandardPaths>

#include "box_view.h"
#include "sembind.h"
#include "semantik.h"
#include "canvas_view.h"
#include "sem_mediator.h"
#include "image_view.h"
#include "config_dialog.h"
#include "text_view.h"
#include "table_view.h"
#include "vars_view.h"
#include "pre_view.h"
#include "linear_container.h"
#include "template_entry.h"
#include "generator_dialog.h"
#include "data_view.h"
#include "doc_dialog.h"
#include "aux.h" 
#include "linear_view.h"
#include      "mem_command.h"

void semantik_win::slot_generate()
{
	if (m_oMediator->choose_root() <= 0)
	{
		KMessageBox::sorry(this, i18n("The map is empty, Semantik\ncannot create documents from it"), i18n("Empty map"));
		return;
	}

	QDir l_oDir(TEMPLATE_DIR);
	if (!l_oDir.exists() || !l_oDir.isReadable())
	{
		slot_message(i18n("Template folder %1 is not readable", TEMPLATE_DIR), 5000);
		return;
	}
	QFileInfoList l_o = l_oDir.entryInfoList(QStringList()<<"*.py", QDir::Files, QDir::Name);
	//qDebug()<<l_o.size();

	QList<template_entry> l_oLst;
	foreach (QFileInfo l_oInfo, l_o)
	{
		template_entry l_oTemplate;
		int l_iCnt = 0;
		if (l_oInfo.isReadable())
		{
			QFile l_oFile(l_oInfo.absoluteFilePath());
			if (!l_oFile.open(QIODevice::ReadOnly))
			{
				//qDebug()<<"could not open the file"<<l_oInfo.absoluteFilePath();
				continue;
			}

			if (l_oFile.fileName().contains(notr("semantik.sem.py")))
			{
				continue;
			}

			l_oTemplate.m_sFile = l_oInfo.absoluteFilePath();

			QTextStream l_oS(&l_oFile);
			while (!l_oS.atEnd())
			{
				QString l_sLine = l_oS.readLine(1000);
				if (l_sLine.length() < 2)
				{
					continue;
				}
				else if (l_sLine.startsWith("#"))
				{
					if (l_sLine.startsWith(notr("#sem:name:")))
					{
						l_oTemplate.m_sName = l_sLine.mid(11);
						l_iCnt++;
					}
					else if (l_sLine.startsWith(notr("#sem:tip:")))
					{
						l_oTemplate.m_sTip = l_sLine.mid(10);
						l_iCnt++;
					}
				}
				else
				{
					break;
				}

			}
			l_oFile.close();

			if (l_iCnt<2) continue;
			l_oLst.push_front(l_oTemplate);
			//qDebug()<<l_oTemplate.m_sFile;
			//qDebug()<<l_oTemplate.m_sName;
			//qDebug()<<l_oTemplate.m_sTip;
		}
	}

	generator_dialog l_oGen(this, l_oLst);
	l_oGen.m_oName->setText(m_oMediator->m_sOutProject);
	l_oGen.m_oLocation->setText(m_oMediator->m_sOutDir);
	if (l_oGen.m_oLocation->text().length()<2) l_oGen.m_oLocation->setText(bind_node::get_var(notr("outdir")));

	l_oGen.activate_from_name(m_oMediator->m_sOutTemplate);
	if (l_oGen.exec() == QDialog::Accepted)
	{
		m_oMediator->m_sOutDir = l_oGen.m_oLocation->text();
		m_oMediator->m_sOutProject = l_oGen.m_oName->text();
		m_oMediator->m_sOutTemplate = l_oGen.m_sShortName;
		m_oMediator->generate_docs(l_oGen.m_sCurrent, l_oGen.m_oName->text(), l_oGen.m_oLocation->text());
	}
}

semantik_win::semantik_win(QWidget *i_oParent) : KXmlGuiWindow(i_oParent)
{
	setObjectName("semantik_win");

	setWindowIcon(QIcon("semantik"));

	m_oWindef = new windef();
	m_oMediator = new sem_mediator(this);
	m_oMediator->m_oWindef = m_oWindef;
	connect(m_oMediator, SIGNAL(sig_message(const QString&, int)), this, SLOT(slot_message(const QString&, int)));
	connect(m_oMediator, SIGNAL(sig_update_title()), this, SLOT(update_title()));

	m_oColorMenu = new QMenu(i18n("Colors"), this);

	QFrame *fr = new QFrame(this);
	fr->setLineWidth(0);
	fr->setFrameStyle(QFrame::NoFrame);
	m_oCanvas = new canvas_view(fr, m_oMediator, m_oColorMenu);
	setCentralWidget(fr);
	QGridLayout *ll = new QGridLayout(fr);
	ll->addWidget(m_oCanvas);
	m_oCanvas->m_oSemantikWindow = this;

	connect(m_oCanvas, SIGNAL(sig_message(const QString&, int)), this, SLOT(slot_message(const QString&, int)));

	//BEGIN setup_actions();
	KStandardAction::quit(this, SLOT(close()), actionCollection());
	KStandardAction::save(this, SLOT(slot_save()), actionCollection());
	KStandardAction::saveAs(this, SLOT(slot_save_as()), actionCollection());
	KStandardAction::open(this, SLOT(slot_open()), actionCollection());
	KStandardAction::print(m_oCanvas, SLOT(slot_print()), actionCollection());
	KStandardAction::tipOfDay(this, SLOT(slot_tip_of_day()), actionCollection());
	m_oUndoAct = KStandardAction::undo(m_oMediator, SLOT(slot_undo()), actionCollection());
	m_oUndoAct->setEnabled(false);
	m_oRedoAct = KStandardAction::redo(m_oMediator, SLOT(slot_redo()), actionCollection());
	m_oRedoAct->setEnabled(false);

	m_oRecentFilesAct = KStandardAction::openRecent(this, SLOT(slot_recent(const QUrl&)), actionCollection());

	m_oReorgAct = new QAction(i18n("&Reorganize the map"), this);
	actionCollection()->addAction(notr("reorganize_map"), m_oReorgAct);
	m_oReorgAct->setShortcut(i18n("Ctrl+R"));
	connect(m_oReorgAct, SIGNAL(triggered(bool)), m_oCanvas, SLOT(reorganize()));

	m_oExportSizeAct = new QAction(i18n("&Export the map..."), this);
	actionCollection()->addAction(notr("export_map_size"), m_oExportSizeAct);
	m_oExportSizeAct->setShortcut(i18n("Ctrl+Shift+E"));
	connect(m_oExportSizeAct, SIGNAL(triggered(bool)), m_oCanvas, SLOT(export_map_size()));

	KStandardAction::preferences(this, SLOT(slot_properties()), actionCollection());

	m_oGenerateAct = new QAction(i18n("&Generate..."), this);
	actionCollection()->addAction(notr("generate_doc"), m_oGenerateAct);
	m_oGenerateAct->setIcon(QIcon(notr("run-build-file")));
	m_oGenerateAct->setShortcut(i18n("Ctrl+G"));
	connect(m_oGenerateAct, SIGNAL(triggered(bool)), SLOT(slot_generate()));

	//m_oCanvasFitZoom = new QAction(i18n("Fit zoom"), this);
	//actionCollection()->addAction(notr("zoom_map"), m_oCanvasFitZoom);
	//m_oCanvasFitZoom->setIcon(QIcon(notr("zoom-best-fit")));
	//connect(m_oCanvasFitZoom, SIGNAL(triggered(bool)), m_oCanvas, SLOT(fit_zoom()));
	m_oCanvasFitZoom = KStandardAction::fitToPage(this, SLOT(fit_zoom()), actionCollection());
	m_oCanvasFitZoom->setIcon(QIcon(notr("zoom-best-fit")));
	m_oCanvasFitZoom->setShortcut(i18n("Ctrl+H"));

	KStandardAction::zoomIn(m_oCanvas, SLOT(zoom_in()), actionCollection());
	KStandardAction::zoomOut(m_oCanvas, SLOT(zoom_out()), actionCollection());

	//END setupActions()
	//setStandardToolBarMenuEnabled(true);


	QDockWidget *l_oDockData = new QDockWidget(i18n("Data"), this);
	l_oDockData->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	m_oDataView = new data_view(l_oDockData, m_oMediator);
	l_oDockData->setWidget(m_oDataView);
	addDockWidget(Qt::BottomDockWidgetArea, l_oDockData);
	actionCollection()->addAction(notr("show_dock_data"), l_oDockData->toggleViewAction());
	l_oDockData->setObjectName(notr("DataDock"));

	m_oTextView = new text_view(m_oDataView, m_oMediator);
	m_oDataView->addWidget(m_oTextView);

	m_oImageView = new image_view(m_oDataView, m_oMediator);
	m_oDataView->addWidget(m_oImageView);

	m_oTableView = new table_view(m_oDataView, m_oMediator);
	m_oDataView->addWidget(m_oTableView);

	m_oDiagramView = new box_view(m_oDataView, m_oMediator);
	m_oDiagramView->m_bShowFileMenu = true;
	m_oDiagramView->m_oColorMenu = m_oColorMenu;
	m_oDataView->addWidget(m_oDiagramView);
	connect(m_oDiagramView, SIGNAL(sig_message(const QString&, int)), this, SLOT(slot_message(const QString&, int)));


	QDockWidget *l_oDockVars = new QDockWidget(i18n("Variables"), this);
	l_oDockVars->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	m_oVarsView = new vars_view(l_oDockVars, m_oMediator);
	l_oDockVars->setWidget(m_oVarsView);
	addDockWidget(Qt::BottomDockWidgetArea, l_oDockVars);
	l_oDockVars->setObjectName(notr("VarsDock"));
	actionCollection()->addAction(notr("show_dock_vars"), l_oDockVars->toggleViewAction());
	m_oVarsView->init_completer();

	QDockWidget *l_oDockPreview = new QDockWidget(i18n("Preview"), this);
	l_oDockPreview->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	m_oPreView = new pre_view(l_oDockPreview, m_oMediator);
	l_oDockPreview->setWidget(m_oPreView);
	addDockWidget(Qt::RightDockWidgetArea, l_oDockPreview);
	actionCollection()->addAction(notr("show_dock_preview"), l_oDockPreview->toggleViewAction());
	l_oDockPreview->setObjectName(notr("PreviewDock"));


	tabifyDockWidget(l_oDockData, l_oDockVars);


	QDockWidget *l_oDock = new QDockWidget(i18n("Linear view"), this);
	m_oTree = new linear_container(l_oDock, m_oMediator);
	l_oDock->setWidget(m_oTree);
	addDockWidget(Qt::LeftDockWidgetArea, l_oDock);
	actionCollection()->addAction(notr("show_dock_linear"), l_oDock->toggleViewAction());
	l_oDock->setObjectName(notr("LinearDock"));

	flag_scheme l_oScheme(this, notr("crsc-app-colors"), "");
	m_oColorGroup = new QActionGroup(this);
	for (int i = 0; i < 9; ++i)
	{
		QAction *l_oAct = new QAction(QIcon(), i18n("Color"), m_oColorGroup);
		m_oColorGroup->addAction(l_oAct);
		actionCollection()->addAction(QString("color_%1").arg(QString::number(i)), l_oAct);
		if (i != 0)
		{
			m_oColorMenu->addAction(l_oAct);
		}
	}

	m_oCustomColorAct = new QAction(l_oScheme._icon(), i18n("Custom color"), m_oColorGroup);
	m_oColorMenu->addAction(m_oCustomColorAct);
	m_oColorGroup->setExclusive(true);
	actionCollection()->addAction(notr("color_custom"), m_oCustomColorAct);

	m_oFlagGroup = new QActionGroup(this);
	m_oFlagGroup->setExclusive(false);
	connect(m_oFlagGroup, SIGNAL(triggered(QAction*)), m_oCanvas, SLOT(change_flags(QAction*)));

	for (int i = 0; i < 16; ++i)
	{
		QAction *l_oAct = new QAction(QIcon(notr("crsc-app-colors")), i18n("Flag"), m_oFlagGroup);
		l_oAct->setCheckable(true);
		m_oFlagGroup->addAction(l_oAct);
		actionCollection()->addAction(QString("flag_%1").arg(QString::number(i)), l_oAct);
	}

	setupGUI(QSize(800, 800), Default, notr("semantikui.rc"));

	connect(m_oMediator, SIGNAL(sync_colors()), this, SLOT(sync_colors()));
	connect(m_oMediator, SIGNAL(sync_font()), m_oCanvas, SLOT(notify_font()));
	connect(m_oColorGroup, SIGNAL(triggered(QAction*)), m_oDiagramView, SLOT(change_colors(QAction*)));
	connect(m_oColorGroup, SIGNAL(triggered(QAction*)), m_oCanvas, SLOT(change_colors(QAction*)));

	connect(m_oMediator, SIGNAL(sync_background_color()), m_oCanvas, SLOT(slot_background_color()));

	m_oMediator->m_oCurrentUrl = QUrl();
	update_title();

	linear_view* ln = m_oTree->m_oView;
	connect(m_oMediator, SIGNAL(sig_preview()), m_oPreView, SLOT(notify_preview()));
	connect(m_oMediator, SIGNAL(sig_add_item(int)), m_oCanvas, SLOT(notify_add_item(int)));
	connect(m_oMediator, SIGNAL(sig_add_item(int)), ln, SLOT(notify_add_item(int)));
	connect(m_oMediator, SIGNAL(sig_add_item(int)), m_oDiagramView, SLOT(notify_add_item(int)));

	connect(m_oMediator, SIGNAL(sig_delete_item(int)), m_oCanvas, SLOT(notify_delete_item(int)));
	connect(m_oMediator, SIGNAL(sig_delete_item(int)), ln, SLOT(notify_delete_item(int)));
	connect(m_oMediator, SIGNAL(sig_link_items(int, int)), m_oCanvas, SLOT(notify_link_items(int, int)));
	connect(m_oMediator, SIGNAL(sig_link_items(int, int)), ln, SLOT(notify_link_items(int, int)));
	connect(m_oMediator, SIGNAL(sig_unlink_items(int, int)), m_oCanvas, SLOT(notify_unlink_items(int, int)));
	connect(m_oMediator, SIGNAL(sig_unlink_items(int, int)), ln, SLOT(notify_unlink_items(int, int)));
	connect(m_oMediator, SIGNAL(sig_ref_items(int, int)), m_oCanvas, SLOT(notify_ref_items(int, int)));
	connect(m_oMediator, SIGNAL(sig_unref_items(int, int)), m_oCanvas, SLOT(notify_unref_items(int, int)));
	connect(m_oMediator, SIGNAL(sig_repaint(int)), m_oCanvas, SLOT(notify_repaint(int)));
	connect(m_oMediator, SIGNAL(sig_repaint(int)), ln, SLOT(notify_repaint(int)));

	connect(m_oMediator, SIGNAL(sig_edit(int)), m_oCanvas, SLOT(notify_edit(int)));
	connect(m_oMediator, SIGNAL(sig_edit(int)), ln, SLOT(notify_edit(int)));
	connect(m_oMediator, SIGNAL(sig_text(int)), m_oTextView, SLOT(notify_text(int)));
	connect(m_oMediator, SIGNAL(sig_vars(int)), m_oVarsView, SLOT(notify_vars(int)));

	connect(m_oMediator, SIGNAL(sig_table(int)), m_oTableView, SLOT(notify_table(int)));

	connect(m_oMediator, SIGNAL(sig_flag(int)), m_oCanvas, SLOT(notify_flag(int)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oCanvas, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), ln, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oDataView, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oTextView, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oVarsView, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oImageView, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oTableView, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oDiagramView, SLOT(notify_select(const QList<int>&, const QList<int>&)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), ln, SLOT(notify_select(const QList<int>&, const QList<int>&)));

	connect(m_oMediator, SIGNAL(sig_sort(int, bool)), m_oCanvas, SLOT(notify_sort(int, bool)));
	connect(m_oMediator, SIGNAL(sig_sort(int, bool)), ln, SLOT(notify_sort(int, bool)));


	connect(m_oMediator, SIGNAL(sig_datatype(int)), m_oDataView, SLOT(notify_datatype(int)));
	connect(m_oMediator, SIGNAL(sig_datatype(int)), ln, SLOT(notify_datatype(int)));

	connect(m_oMediator, SIGNAL(sig_pic(int)), m_oCanvas, SLOT(notify_pic(int)));
	connect(m_oMediator, SIGNAL(sig_pic(int)), m_oImageView, SLOT(notify_pic(int)));

	connect(m_oMediator, SIGNAL(sig_move(const QList<int>&, const QList<QPointF>&)), m_oCanvas, SLOT(notify_move(const QList<int>&, const QList<QPointF>&)));

	connect(m_oMediator, SIGNAL(sync_flags()), this, SLOT(sync_flags()));
	connect(m_oImageView, SIGNAL(sig_message(const QString&, int)), this, SLOT(slot_message(const QString&, int)));

	connect(m_oMediator, SIGNAL(sig_open_map()), m_oCanvas, SLOT(notify_open_map()));
	connect(m_oMediator, SIGNAL(sig_change_data(int)), m_oCanvas, SLOT(notify_change_data(int)));

	connect(m_oMediator, SIGNAL(sig_export_doc()), m_oCanvas, SLOT(notify_export_doc()));
	connect(m_oMediator, SIGNAL(sig_export_item(int)), m_oDiagramView, SLOT(notify_export_item(int)));
	connect(m_oMediator, SIGNAL(sig_export_item(int)), m_oImageView, SLOT(notify_export_item(int)));

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
	connect(m_oMediator, SIGNAL(sig_sequence_box(int, int)), m_oDiagramView, SLOT(notify_sequence_box(int, int)));
	connect(m_oMediator, SIGNAL(sig_focus(void *)), m_oDiagramView, SLOT(notify_focus(void *)));
	connect(m_oMediator, SIGNAL(sig_focus(void *)), m_oCanvas, SLOT(notify_focus(void *)));
	connect(m_oMediator, SIGNAL(sig_change_properties(void *)), m_oDiagramView, SLOT(notify_change_properties(void *)));

	m_oTree->m_oView->addAction(m_oCanvas->m_oDeleteAction); // FIXME

	//qDebug()<<"instance is "<<l_oInst<<endl;

	m_oMediator->init_colors();
	m_oMediator->init_flags();

	read_config();
	m_oMediator->notify_background_color();
	statusBar()->showMessage(i18n("Welcome to Semantik"), 2000);
	setAutoSaveSettings();

	connect(m_oMediator, SIGNAL(enable_undo(bool, bool)), this, SLOT(slot_enable_undo(bool, bool)));
	KTipDialog::showTip(this, notr("semantik/tips"));
}

void semantik_win::fit_zoom()
{
	if (m_oCanvas->hasFocus())
	{
		m_oCanvas->fit_zoom();
	}
	else if (m_oDiagramView->hasFocus())
	{
		m_oDiagramView->fit_zoom();
	}
}

void semantik_win::read_config()
{
	KConfig l_oCfg("semantik");
	KConfigGroup l_oConfig(&l_oCfg, notr("General Options"));

	KConfigGroup l_oSaveConfig(&l_oCfg, notr("Recent Files"));
	m_oRecentFilesAct->loadEntries(l_oSaveConfig);

	move(l_oConfig.readEntry(notr("winpos"), QPoint(0, 0)));
	m_oMediator->m_oColor = QColor(l_oConfig.readEntry(notr("bgcolor"), notr("#FFFDE8")));
	m_oMediator->m_oArrowColor = QColor(l_oConfig.readEntry(notr("arrowcolor"), notr("#000000")));
	m_oMediator->m_oAltArrowColor = QColor(l_oConfig.readEntry(notr("altarrowcolor"), notr("#e0e0e0")));
	m_oMediator->m_sOutDir = l_oConfig.readEntry(notr("outdir"), QDir::homePath());
	bind_node::set_var(notr("outdir"), m_oMediator->m_sOutDir);
	m_oWindef->m_bUseTouchpad = l_oConfig.readEntry(notr("touchpad"), false);

	QString l_oGuidString = l_oConfig.readEntry(notr("guid"));
	if (!l_oGuidString.isEmpty())
	{
		m_oQUuid = QUuid(l_oGuidString);
	}
	else
	{
		m_oQUuid = QUuid::createUuid();
		write_config();
	}

	//QString lang_code(config.readEntry("currentLanguageCode", QVariant(QString())).toString());
	//if (lang_code.isEmpty()) lang_code = "en_US";  // null-string are saved as empty-strings
	//setCurrentLanguage(lang_code);
}

void semantik_win::write_config()
{
	KConfig l_oCfg("semantik");
	KConfigGroup l_oConfig(&l_oCfg, notr("General Options"));
	l_oConfig.writeEntry(notr("winpos"), pos());
	l_oConfig.writeEntry(notr("outdir"), bind_node::get_var(notr("outdir")));
	l_oConfig.writeEntry(notr("guid"), m_oQUuid.toString());
	l_oConfig.sync();

	KConfigGroup l_oSaveConfig(&l_oCfg, notr("Recent Files"));
	m_oRecentFilesAct->saveEntries(l_oSaveConfig);
	l_oSaveConfig.sync();
}

semantik_win::~semantik_win()
{

}

int semantik_win::print_to_file(const QUrl & url, QPair<int, int> p)
{
	return m_oCanvas->batch_print_map(url, p);
}

bool semantik_win::slot_save_as()
{
	choose:
	QUrl l_o = QFileDialog::getSaveFileUrl(this, i18n("Choose a file name"), 
                QUrl(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
		i18n("*.sem|Semantik file (*.sem)"));

	if (l_o.path().isEmpty()) return false;
	if (!l_o.path().endsWith(notr(".sem")))
	{
		l_o = QUrl(l_o.path()+notr(".sem"));
	}

	if (m_oMediator->m_sLastSaved != l_o.path())
	{
		if (l_o.isLocalFile() && QFile::exists(l_o.path()))
		{
			int mu = KMessageBox::questionYesNo(NULL, //this,
			i18n("The file \"%1\" already exists.\nOverwrite it?", l_o.path()),
			i18n("Overwrite existing file"),
			KStandardGuiItem::yes(),
			KStandardGuiItem::no(),
			notr("OverwriteExistingFile"));
			if (!mu)
				goto choose;
		}
	}

	if (m_oMediator->save_file(l_o.path()))
	{
		statusBar()->showMessage(i18n("Saved '%1'", l_o.path()), 2000);
		m_oMediator->m_oCurrentUrl = l_o;
		update_title();
		return true;
	}
	else
	{
		m_oMediator->m_oCurrentUrl = QUrl();
		update_title();

		int mu = KMessageBox::questionYesNo(NULL, //this,
		i18n("The file \"%1\" could not be saved because an error happened.\nTry again?", l_o.path()),
		i18n("Try again?"),
		KStandardGuiItem::yes(),
		KStandardGuiItem::no(),
		notr("ContinueSaveAs"));
		if (!mu) {
			goto choose;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool semantik_win::slot_save()
{
	if (m_oMediator->m_sLastSaved.isEmpty())
	{
		return slot_save_as();
	}
	if (m_oMediator->save_file(m_oMediator->m_sLastSaved))
	{
		statusBar()->showMessage(i18n("Saved '%1'", m_oMediator->m_sLastSaved), 2000);
		return true;
	}
	else
	{
		int mu = KMessageBox::questionYesNo(NULL, //this,
		i18n("The file \"%1\" could not be saved because an error happened.\nTry again?", m_oMediator->m_sLastSaved),
		i18n("Try again?"),
		KStandardGuiItem::yes(),
		KStandardGuiItem::no(),
		notr("ContinueSaveAs"));
		if (!mu) {
			return slot_save_as();
		}
		else
		{
			return false;
		}
	}
	return false;
}

void semantik_win::slot_open()
{
	if (m_oMediator->m_bDirty)
	{
		if (!proceed_save()) return;
	}

	QUrl l_o = QFileDialog::getOpenFileUrl(this, i18n("Choose a file name"),
                QUrl(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
		i18n("*.sem *.kdi *.mm *.vym|All Supported Files (*.sem *.kdi *.mm *.vym)"));
	if (l_o.isValid() && m_oMediator->open_file(l_o.path()))
	{
		m_oMediator->m_oCurrentUrl = l_o;
	}
	update_title();
}

void semantik_win::update_title()
{
	QString mod;
	if (m_oMediator->m_bDirty) mod = i18n(" [Modified] ");

	QString txt;
	if (m_oMediator->m_oCurrentUrl.path().isEmpty())
	{
		txt = i18n("Semantik %1", mod);
	}
	else
	{
		txt = i18n("%1 %2 - Semantik", m_oMediator->m_oCurrentUrl.path(), mod);
		m_oRecentFilesAct->addUrl(m_oMediator->m_oCurrentUrl); // TODO
	}
	setWindowTitle(txt);

}

void semantik_win::slot_properties()
{
	config_dialog l_oGen(this);

	KConfig l_oCfg("semantik");
	KConfigGroup l_oSettings(&l_oCfg, notr("General Options"));
	l_oGen.m_oConnType->setCurrentIndex(l_oSettings.readEntry(notr("conn"), 0));
	l_oGen.m_oReorgType->setCurrentIndex(l_oSettings.readEntry(notr("reorg"), 0));
	l_oGen.m_oAutoSave->setValue(l_oSettings.readEntry(notr("auto"), 5));
	l_oGen.m_oAutoReorg->setCurrentIndex(l_oSettings.readEntry(notr("autoReorg"), 1));
	l_oGen.m_oUseTouchpad->setChecked(l_oSettings.readEntry(notr("touchpad"), false));
	l_oGen.m_oPreviewPics->setChecked(m_oMediator->m_bShowPics);

	l_oGen.m_oColorWidget->setText(m_oMediator->m_oColor.name());
	l_oGen.m_oArrowWidget->setText(m_oMediator->m_oArrowColor.name());
	l_oGen.m_oAltArrowWidget->setText(m_oMediator->m_oAltArrowColor.name());

	{
		QPalette l_oPalette = l_oGen.m_oColorWidget->palette();
		l_oGen.m_oColor = m_oMediator->m_oColor;
		l_oPalette.setBrush(QPalette::Active, QPalette::Button, l_oGen.m_oColor);
		l_oGen.m_oColorWidget->setPalette(l_oPalette);
	}
	{
		QPalette l_oPalette = l_oGen.m_oArrowWidget->palette();
		l_oGen.m_oArrowColor = m_oMediator->m_oArrowColor;
		l_oPalette.setBrush(QPalette::Active, QPalette::Button, l_oGen.m_oArrowColor);
		l_oGen.m_oArrowWidget->setPalette(l_oPalette);
	}
	{
		QPalette l_oPalette = l_oGen.m_oAltArrowWidget->palette();
		l_oGen.m_oAltArrowColor = m_oMediator->m_oAltArrowColor;
		l_oPalette.setBrush(QPalette::Active, QPalette::Button, l_oGen.m_oAltArrowColor);
		l_oGen.m_oAltArrowWidget->setPalette(l_oPalette);
	}

	if (l_oGen.exec() == QDialog::Accepted)
	{
		//m_oMediator->m_iConnType = l_oGen.m_oConnType->currentIndex();
		//m_oMediator->m_iReorgType = l_oGen.m_oReorgType->currentIndex();
		l_oSettings.writeEntry(notr("conn"), m_oMediator->m_iConnType = l_oGen.m_oConnType->currentIndex());
		l_oSettings.writeEntry(notr("reorg"), m_oMediator->m_oWindef->m_iReorgType = l_oGen.m_oReorgType->currentIndex());
		l_oSettings.writeEntry(notr("auto"), m_oMediator->m_iTimerValue = l_oGen.m_oAutoSave->value());
		l_oSettings.writeEntry(notr("bgcolor"), l_oGen.m_oColor.name());
		l_oSettings.writeEntry(notr("arrowcolor"), l_oGen.m_oArrowColor.name());
		l_oSettings.writeEntry(notr("altarrowcolor"), l_oGen.m_oAltArrowColor.name());
		l_oSettings.writeEntry(notr("autoReorg"), m_oMediator->m_iAutoReorg = l_oGen.m_oAutoReorg->currentIndex());
		l_oSettings.writeEntry(notr("touchpad"), m_oMediator->m_oWindef->m_bUseTouchpad = l_oGen.m_oUseTouchpad->isChecked());

		m_oMediator->set_show_pics(l_oGen.m_oPreviewPics->isChecked());

		if (l_oGen.m_oColor != m_oMediator->m_oColor || m_oMediator->m_oArrowColor != l_oGen.m_oArrowColor || l_oGen.m_oAltArrowColor != m_oMediator->m_oAltArrowColor)
		{
			mem_all_color *l_oMem = new mem_all_color(m_oMediator);
			l_oMem->m_oColor = l_oGen.m_oColor;
			l_oMem->m_oArrowColor = l_oGen.m_oArrowColor;
			l_oMem->m_oAltArrowColor = l_oGen.m_oAltArrowColor;

			l_oMem->m_oOldColor = m_oMediator->m_oColor;
			l_oMem->m_oOldArrowColor = m_oMediator->m_oArrowColor;
			l_oMem->m_oOldAltArrowColor = m_oMediator->m_oAltArrowColor;
			l_oMem->apply();
		}
		m_oMediator->init_timer();
		l_oCfg.sync();
	}
}

bool semantik_win::queryClose()
{
	write_config();
	if (!m_oMediator->m_bDirty) return true;
	return proceed_save();
}

bool semantik_win::proceed_save()
{
	QString l_oTitle = m_oMediator->m_sLastSaved;
	if (l_oTitle.isEmpty()) l_oTitle = i18n("Untitled");

	int l_o = KMessageBox::warningYesNoCancel(NULL, //this,
	i18n("The document \"%1\" has been modified.\nDo you want to save your changes or discard them?", l_oTitle),
	i18n("Close Document"),
	KStandardGuiItem::save(), KStandardGuiItem::discard());

	switch (l_o)
	{
		case KMessageBox::Yes:
			return slot_save();
		case KMessageBox::No:
			return true;
		default:
			return false;
	}
	return false;
}

void semantik_win::slot_recent(const QUrl& i_oBadUrl)
{
	// deep copy or it will crash
	QUrl i_oUrl = i_oBadUrl;
	if (i_oUrl.path().isEmpty()) return;
	if (m_oMediator->open_file(i_oUrl.path()))
	{
		m_oMediator->m_oCurrentUrl = i_oUrl;
	}
	update_title();
}

void semantik_win::slot_message(const QString & i_sMsg, int i_iDur)
{
	statusBar()->showMessage(i_sMsg, i_iDur);
}

void semantik_win::slot_tip_of_day()
{
	KTipDialog::showTip(this, notr("semantik/tips"), true);
}

void semantik_win::slot_enable_undo(bool undo, bool redo) {
	m_oUndoAct->setEnabled(undo);
	m_oRedoAct->setEnabled(redo);
}

void process_icon(QIcon* i_oIcon, const QColor& i_oColor, int i_iSize)
{
	QPixmap l_oPix(i_iSize, i_iSize);
	QPainter l_oP(&l_oPix);
	l_oPix.fill(i_oColor);
	l_oP.drawRect(0, 0, i_iSize -1, i_iSize -1);
	i_oIcon->addPixmap(l_oPix);
}

void semantik_win::sync_colors()
{
	for (int i=0; i<m_oMediator->m_oColorSchemes.size(); ++i)
	{
		color_scheme l_oScheme = m_oMediator->m_oColorSchemes[i];
		QAction *l_oAction = m_oColorGroup->actions().at(i);

		QIcon l_oIcon;
		process_icon(&l_oIcon, l_oScheme.m_oInnerColor, 22);
		process_icon(&l_oIcon, l_oScheme.m_oInnerColor, 32);
		process_icon(&l_oIcon, l_oScheme.m_oInnerColor, 64);
		process_icon(&l_oIcon, l_oScheme.m_oInnerColor, 128);
		l_oAction->setText(l_oScheme.m_sName);
		l_oAction->setIcon(l_oIcon);
	}
}

void semantik_win::sync_flags()
{
	for (int i=0; i<m_oMediator->m_oFlagSchemes.size(); ++i)
	{
		flag_scheme *l_oScheme = m_oMediator->m_oFlagSchemes[i];
		QAction *l_oAction = m_oFlagGroup->actions().at(i);
		l_oAction->setText(l_oScheme->m_sName);
		l_oAction->setIcon(l_oScheme->_icon());
	}
}

