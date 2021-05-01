// Thomas Nagy 2014-2021 GPLV3

#ifndef BOX_CLASS_PROPERTIES_H
#define BOX_CLASS_PROPERTIES_H

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <KDE/KDialog>
#include "box_class.h"

class class_highlighter_rule
{
	public:
		QRegExp m_oPattern;
		QTextCharFormat m_oFormat;
};

class class_highlighter: public QSyntaxHighlighter
{
	public:
		class_highlighter(QTextDocument *i_oP=0);
		void highlightBlock(const QString &);
		QVector<class_highlighter_rule> m_oRules;
};

class box_class_properties : public KDialog
{
	Q_OBJECT
	public:
		box_class_properties(QWidget*, box_class*);
		box_class *m_oClass;
		QTextEdit *m_oClassDefinition;

	public slots:
		void apply();
		void enable_apply(int);
		void enable_apply();
};

#endif

