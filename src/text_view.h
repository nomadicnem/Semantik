// Thomas Nagy 2007-2021 GPLV3

#ifndef TEXT_VIEW_H
#define TEXT_VIEW_H

#include <QTreeWidget>

#include "con.h"

#include <QTextListFormat>
#include <QToolBar>
#include <QList>

class QTextCharFormat;
class QTextEdit;
class semantik_win;
class sem_mediator;
class text_view : public QWidget
{
	Q_OBJECT
	public:
		text_view(QWidget *i_oParent, sem_mediator *i_oControl);
		sem_mediator *m_oMediator;
		QTextEdit *m_oEdit;

		int m_iId;

		semantik_win *m_oWin;

		QToolBar *m_oTextToolBar;

		void merge_format(const QTextCharFormat &i_oFormat);

		QAction *m_oLinkAct;
		QAction *m_oBoldAct;
		QAction *m_oItalicAct;
		QAction *m_oTextColorAct;
		QAction *m_oClearAct;
		QAction *m_oUnderLineAct;
		QAction *m_oTextBulletAct;
		QAction *m_oTextNumberAct;
		QAction *m_oDecreaseIndentAct;
		QAction *m_oIncreaseIndentAct;

		void text_list(bool, QTextListFormat::Style);
		void change_indent(int);

	public slots:
		void update_edit();

		void text_link();
		void text_bold();
		void text_italic();
		void text_underLine();
		void text_clear();
		void text_color();
		void text_bullet(bool);
		void text_number(bool);
		void decrease_indent();
		void increase_indent();
		void selection_changed();
		void cursor_changed();
		void spelling_language_changed(const QString &);

		void char_format_changed(const QTextCharFormat&);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_text(int id);
};

#endif

