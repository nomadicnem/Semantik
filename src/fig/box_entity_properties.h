// Thomas Nagy 2018 GPLV3

#ifndef BOX_ENTITY_PROPERTIES_H
#define BOX_ENTITY_PROPERTIES_H

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <KDE/KDialog>
#include "box_entity.h"

class QCompleter;
class entity_highlighter_rule
{
	public:
		QRegExp m_oPattern;
		QTextCharFormat m_oFormat;
};

class entity_highlighter: public QSyntaxHighlighter
{
	public:
		entity_highlighter(QTextDocument *i_oP=0);
		void highlightBlock(const QString &);
		QVector<entity_highlighter_rule> m_oRules;
};

class box_entity_properties : public KDialog
{
	Q_OBJECT
	public:
		box_entity_properties(QWidget*, box_entity*);
		box_entity *m_oEntity;
		QTextEdit *m_oEntityDefinition;

	public slots:
		void apply();
		void enable_apply(int);
		void enable_apply();
};

#endif

