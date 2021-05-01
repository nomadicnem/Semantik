// Thomas Nagy 2007-2021 GPLV3

#ifndef DATA_CONTROL_H
#define DATA_CONTROL_H

#include <QObject>
#include <QStack>
#include <QXmlDefaultHandler>
#include<QUrl> 
 #include   <QMutex> 
// FIXME
// #include <kio/netaccess.h>

#include "aux.h"
#include "con.h"
#include "mem_base.h"
#include "flag_scheme.h"
#include "color_scheme.h"

#include "data_item.h"

class QTimer;
class html_converter : public QXmlDefaultHandler
{
	public:
		html_converter();

		bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
		bool endElement(const QString&, const QString&, const QString&);
		bool characters(const QString &i_sStr);

		QString m_sBuf;
		QStringList m_oTotale;
};


class semantik_reader;
class data_item;
class windef;
class bind_node;
class data_link;
class sem_mediator: public QObject
{
	Q_OBJECT

	signals:
		//void rectangle_changed(int i);
		void sig_message(const QString&, int);
		void dirty(bool);
		void sig_update_title();

		void sig_add_item(int id);
		void sig_delete_item(int id);
		void sig_link_items(int id1, int id2);
		void sig_unlink_items(int id1, int id2);
		void sig_ref_items(int id1, int id2);
		void sig_unref_items(int id1, int id2);
		void sync_flags();
		void sync_colors();
		void sync_background_color();
		void sync_font();
		void sig_select(const QList<int>& unsel, const QList<int>& sel);
		void sig_move(const QList<int>&sel, const QList<QPointF>&pos);
		void sig_repaint(int id);
		void sig_flag(int id);
		void sig_edit(int id);
		void sig_text(int id);
		void sig_datatype(int id);
		void sig_preview();
		void sig_vars(int id);
		void sig_pic(int id);
		void sig_table(int id);
		void sig_sort(int, bool);
		void sig_open_map();
		void sig_change_data(int id);
		void sig_export_item(int id);
		void sig_export_doc();
		void sig_focus(void*);

		void sig_add_box(int, int);
		void sig_del_box(int, int);
		void sig_edit_box(int, int);
		void sig_link_box(int, data_link*);
		void sig_unlink_box(int, data_link*);
		void sig_box_props(int id, const QList<diagram_item*>&);
		void sig_pos_box(int id, const QList<data_box*>&);
		void sig_change_link_box(int id, data_link*);
		void sig_size_box(int id, const QList<data_box*>&);
		void sig_text_align(int id, const QList<data_box>&);
		void sig_sequence_box(int id, int);
		void sig_change_properties(void*);

		void enable_undo(bool, bool);

	public:
		sem_mediator(QObject *i_oParent);
		~sem_mediator();

		windef *m_oWindef;

		QString m_sOutDir;
		QString m_sOutProject;
		QString m_sOutTemplate;


		void set_show_pics(bool);
		bool m_bShowPics;
		bool m_bExportIsWidth;
		int m_iExportWidth;
		int m_iExportHeight;
		QString m_sExportUrl;

		QColor m_oColor;
		QColor m_oArrowColor;
		QColor m_oAltArrowColor;
		QString m_sSpellingLanguage;
		QUrl m_oCurrentUrl;
		QFont m_oFont;
		QString m_sHints;
		QString m_sGlobalHints;

		QHash<int, data_item> m_oItems;
		QList<QPoint> m_oLinks;
		QList<data_ref> m_oRefs;
		QList<color_scheme> m_oColorSchemes;
		QList<flag_scheme> m_oFlagSchemes;

		// settings
		double m_dTriSize;
		int m_iTimerValue;
		int m_iConnType;
		int m_iAutoReorg;
		int m_iReorgType;

		// operational
		bool m_bIsDiagram;
		data_link m_oLinkState;

		QStack<mem_command*> m_oUndoStack;
		QStack<mem_command*> m_oRedoStack;

		int m_iSortId;
		int m_iSortCursor;

		QString m_sTempDir;
		QHash<int, data_pic*> m_oPixCache;
		QString m_sLastSaved;
		bool m_bDirty;
		///////////////////////////////////////////7
		void set_dirty(bool b = true);

		void init_temp_dir();
		void clean_temp_dir();

		bool save_file(QString);
		bool open_file(const QString&);
		bool open_raw(const QString&);

		void init_colors();
		void init_flags();

		// first parameter is the parent, second parameter is the wanted id
		// and the third parameter is for copying the parents data
		bool link_items(int id1, int id2);
		bool ref_items(int, int);

		int num_children(int i_iParent);
		void select_root_item(int);
		QList<int> all_roots();
		int root_of(int i_iId);
		void next_root();
		void prev_root();
		void notify_add_item(int id);
		void notify_delete_item(int id);
		void notify_link_items(int id1, int id2);
		void notify_unlink_items(int id1, int id2);
		void notify_ref_items(int, int);
		void notify_unref_items(int, int);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_move(const QList<int>&sel, const QList<QPointF>&pos);
		void notify_repaint(int id);
		void notify_edit(int id);
		void notify_text(int id);
		void notify_vars(int id);
		void notify_flag(int id);
		void notify_pic(int id);
		void notify_datatype(int id);
		void notify_table(int id);
		void notify_sort(int, bool);
		void notify_change_data(int id);
		void notify_export_item(int id);
		void notify_export_doc();
		void notify_focus(void *);

		void notify_add_box(int id, int box);
		void notify_del_box(int id, int box);
		void notify_edit_box(int id, int box);
		void notify_link_box(int id, data_link *link);
		void notify_unlink_box(int id, data_link *link);
		void notify_box_props(int id, const QList<diagram_item*>&);
		void notify_pos_box(int id, const QList<data_box*>&);
		void notify_change_link_box(int id, data_link*);
		void notify_size_box(int id, const QList<data_box*>&);
		void notify_text_align(int id, const QList<data_box>&);
		void notify_sequence_box(int id, int);
		void notify_change_properties(void*);
		void notify_colors();
		void notify_background_color();
		void notify_flags();
		void notify_font();
		void notify_open_map();

		void notify_message(const QString& msg, int duration);

		int next_seq();
		int next_pic_seq();
		int next_box_link_seq(int);

		QPair<int, int> hint_size_diagram(int);

		const QPixmap getPix(int id) const;
		const QPixmap getThumb(int id) const;
		bool load_picture(const QString&, int);
		bool save_and_load_picture(const QUrl& i_sPath, int id);
		int generate_docs(const QString &i_oFile, const QString &i_sName, const QString &i_sLocation);

		int height_of(int i_iId);
		int parent_of(int i_iId);
		int size_of(int i_iId);
		int choose_root();

		QMutex m_oSaveMutex;
		void stop_timer();
		void init_timer();

		QString doc_to_xml();
		data_item& operator+(const int x);

		void select_item_keyboard(int, int);
		void private_select_item(int i_oId);

		void check_undo(bool); // check if the undo/redo actions can be enabled
		int itemSelected();

		friend class semantik_reader;

	private:
		int num_seq;
		int pic_seq;
		QTimer *m_oTimer;

	public slots:
		//void do_reorganize();
		void slot_autosave();
		void slot_undo();
		void slot_redo();
};

QIcon fetch_icon(const QString& i_sName, int i_iSize=32);

#endif

