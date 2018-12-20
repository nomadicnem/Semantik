// Thomas Nagy 2013-2018 GPLV3

#ifndef MEM_BASE
#define MEM_BASE

#include "data_item.h"
#include <QSet>
#include <QPoint>

class flag_scheme;
class sem_mediator;
class mem_command {
	public:
		mem_command(sem_mediator*);
		virtual ~mem_command() {};
		sem_mediator *model;
		virtual void undo() = 0;
		virtual void redo() = 0;
		virtual void apply();
		virtual void redo_dirty();
		virtual void undo_dirty();
		void add();

		bool was_dirty;
		enum IType {DELETE, ADD, LINK, UNLINK, SELECT, MOVE, COLOR, FLAG, EDIT, DATATYPE, TEXT, VARS, PIC, TABLE, SORT,
			ADD_BOX, DEL_BOX, EDIT_BOX, LINK_BOX, UNLINK_BOX, PROP_BOX, POS_BOX, CHANGE_LINK_BOX, SIZE_BOX,
			EDIT_LINK, IMPORT_BOX, SIZE_MATRIX, CHANGE_CLASS_BOX, DIAGRAM_PROPERTIES, SIZE_SEQUENCE, DOC,
			TEXT_ALIGN_BOX, REF, UNREF, ALL_COLOR};
		virtual IType type() = 0;
};

class mem_delete : public mem_command {
	public:
		mem_delete(sem_mediator*);
		void init(QList<int> ids);
		void undo();
		void redo();

		QList<data_item> items;
		QSet<QPoint> links;

		IType type() { return DELETE; }
};

class mem_sel : public mem_command {
	public:
		mem_sel(sem_mediator*);
		void undo();
		void redo();
		void apply();
		QList<int> unsel;
		QList<int> sel;
		int m_iSortSel;
		int m_iSortUnsel;
		IType type() { return SELECT; }
};

class mem_link : public mem_command {
	public:
		mem_link(sem_mediator*);
		void undo();
		void redo();

		int parent;
		int child;

		IType type() { return LINK; }
};

class mem_ref : public mem_command {
	public:
		mem_ref(sem_mediator*);
		void undo();
		void redo();

		int m_iParent;
		int m_iChild;

		IType type() { return REF; }
};


class mem_unref : public mem_command {
	public:
		mem_unref(sem_mediator*);
		void undo();
		void redo();

		int m_iParent;
		int m_iChild;
		int m_iPos;
		IType type() { return UNREF; }
};

class mem_add : public mem_command {
	public:
		mem_add(sem_mediator*);
		void undo();
		void redo();

		data_item item;
		int parent;

		mem_sel *sel;

		IType type() { return ADD; }
};

class mem_doc_open : public mem_command {
	public:
		mem_doc_open(sem_mediator*);
		void undo();
		void redo();

		mem_delete *m_oDelete;
		mem_sel *m_oSel;

		QString m_sOutDirNew;
		QString m_sOutProjectNew;
		QString m_sOutTemplateNew;
		bool m_bExportIsWidthNew;
		bool m_bShowPicsNew;
		int m_iExportWidthNew;
		int m_iExportHeightNew;
		QString m_sExportUrlNew;
		QColor m_oColorNew;
		QColor m_oArrowColorNew;
		QColor m_oAltArrowColorNew;
		QString m_sSpellingLanguageNew;
		QUrl m_oCurrentUrlNew;
		QFont m_oFontNew;
		QString m_sHintsNew;
		QList<color_scheme> m_oColorSchemesNew;
		QList<flag_scheme*> m_oFlagSchemesNew;
		QHash<int, data_item> m_oItemsNew;
		QList<QPoint> m_oLinksNew;
		QList<data_ref> m_oRefsNew;
		QString m_sLastSavedNew;

		QString m_sOutDirOld;
		QString m_sOutProjectOld;
		QString m_sOutTemplateOld;
		bool m_bExportIsWidthOld;
		bool m_bShowPicsOld;
		int m_iExportWidthOld;
		int m_iExportHeightOld;
		QString m_sExportUrlOld;
		QColor m_oColorOld;
		QColor m_oArrowColorOld;
		QColor m_oAltArrowColorOld;
		QString m_sSpellingLanguageOld;
		QUrl m_oCurrentUrlOld;
		QFont m_oFontOld;
		QString m_sHintsOld;
		QList<color_scheme> m_oColorSchemesOld;
		QList<flag_scheme*> m_oFlagSchemesOld;
		QHash<int, data_item> m_oItemsOld;
		QList<QPoint> m_oLinksOld;
		QList<data_ref> m_oRefsOld;
		QString m_sLastSavedOld;

		void init_data(sem_mediator*, sem_mediator*);

		IType type() { return DOC; }
};

#endif
