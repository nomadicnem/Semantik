// Thomas Nagy 2007-2021 GPLV3

#ifndef VARS_VIEW_H
#define VARS_VIEW_H

#include <QList>
#include <QTreeWidget>
#include <QTextEdit>
#include <QSyntaxHighlighter>

class QCompleter;
class sem_mediator;
class class_rule
{
	public:
		QRegExp m_oPattern;
		QTextCharFormat m_oFormat;
};

class vars_highlighter: public QSyntaxHighlighter
{
	public:
		vars_highlighter(QTextDocument *p=0);
		void highlightBlock(const QString &);
		QVector<class_rule> m_oRules;
};

class vars_view : public QTextEdit
{
	Q_OBJECT
	public:
		vars_view(QWidget *i_oParent, sem_mediator *i_oControl);
		sem_mediator *m_oMediator;
		int m_iId;
		bool m_bLockEdit;
		QCompleter *m_oCompleter;
		QCompleter *m_oCompleterItem;
		QCompleter *m_oCompleterAll;
		QAction *m_oCompleteAction;
		void keyPressEvent(QKeyEvent*);
		void init_completer();
		bool startup;

	public slots:
		void update_edit();
		void do_complete(const QString &);
		void try_complete();
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_vars(int);
		void notify_open_map();
};

#endif

