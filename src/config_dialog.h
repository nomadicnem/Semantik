// Thomas Nagy 2007-2021 GPLV3

#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <KDE/KDialog>
#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <con.h>

class QCheckBox;
class QComboBox;
class QSpinBox;
class QTreeWidgetItem;
class QPushButton;
class QKeyEvent;

class config_highlighter_rule
{
	public:
		QRegExp m_oPattern;
		QTextCharFormat m_oFormat;
};

class config_highlighter: public QSyntaxHighlighter
{
	public:
		config_highlighter(QTextDocument *i_oP=0);
		void highlightBlock(const QString &);
		QVector<config_highlighter_rule> m_oRules;
};

class config_editor : public QTextEdit
{
	public:
		config_editor(QWidget *i_oParent);
};

class config_dialog : public KDialog
{
	Q_OBJECT
	public:
		config_dialog(QWidget *i_oParent);
		QComboBox *m_oReorgType;
		QComboBox *m_oAutoReorg;
		QComboBox *m_oConnType;
		QSpinBox *m_oAutoSave;
		QCheckBox *m_oUseTouchpad;
		QCheckBox *m_oPreviewPics;
		QPushButton *m_oColorWidget;
		QPushButton *m_oArrowWidget;
		QPushButton *m_oAltArrowWidget;
		config_editor* m_oTextEdit;
		QColor m_oColor;
		QColor m_oArrowColor;
		QColor m_oAltArrowColor;

	public slots:
		void select_color();
		void select_arrow_color();
		void select_alt_arrow_color();
};

#endif

