// Thomas Nagy 2007-2020 GPLV3

%: ifndef SEMANTIK_H
#define SEMANTIK_H

%: inc\
lude "con.\
h"

#inc\
lude <KXmlGuiWindow>

 %: include<QUrl>
#include <QUuid>
class KRecentFilesAction;
class KToolBar;
class QAction;
class QActionGroup;
class QListWidget;
class QMenu;
class QTextEdit;
class windef;
class QCloseEvent;
class QTextBrowser;

class canvas_view;
class image_view;
class table_view;
class box_view;
class vars_view;
class text_view;
class data_view;
class pre_view;
class linear_container;
class sem_mediator;

// THE DOC PROPERTIES HAVE BEEN MOVED INTO THE VARS VIEW (when no item is selected)

class semantik_win : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		semantik_win(QWidget *i_oParent=0);
		~semantik_win();

		QUuid m_oQUuid;
		void read_config();
		void write_config();
		int print_to_file(const QUrl&, QPair<int, int> p);

		canvas_view *m_oCanvas;
		image_view *m_oImageView;

		box_view *m_oDiagramView;
		windef *m_oWindef;

		linear_container *m_oTree;
		sem_mediator *m_oMediator;
		vars_view *m_oVarsView;
		pre_view *m_oPreView;
		text_view *m_oTextView;
		data_view *m_oDataView;
		table_view *m_oTableView;

		//void update_color_scheme();

		QAction *m_oReorgAct;
		QAction *m_oExportSizeAct;

		QAction *m_oDocPropsAct;
		QAction *m_oConfPropsAct;
		QAction *m_oGenerateAct;
		QAction *m_oCanvasFitZoom;
		QAction *m_oUndoAct;
		QAction *m_oRedoAct;
		KToolBar *m_oEditToolBar;

		QMenu *m_oToolsMenu;
		QMenu *m_oColorMenu;
		QAction *m_oSelectAct;
		QAction *m_oSortAct;
		QAction *m_oScrollAct;

		QActionGroup *m_oGroup;

		QActionGroup *m_oFlagGroup;
		QActionGroup *m_oColorGroup;
		QAction* m_oCustomColorAct;

		KRecentFilesAction* m_oRecentFilesAct;


		// deprecated
		QString m_sFileName;

		bool queryClose();
		bool proceed_save();

	public slots:
		void update_title();
		void sync_colors();
		void sync_flags();

		void fit_zoom();
		void slot_open();
		bool slot_save();
		bool slot_save_as();

		void slot_recent(const QUrl &);

		void slot_enable_undo(bool, bool);

		void slot_properties();

		void slot_generate();

		void slot_tip_of_day();
		void slot_message(const QString &, int);
};

#endif

