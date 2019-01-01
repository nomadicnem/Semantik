// Thomas Nagy 2007-2019 GPLV3

#include <QVariant>
#include <QAction>
#include <QSpinBox>
#include <QCheckBox>
#include  <QTextEdit>
#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <QPushButton>
#include <QGroupBox>
#include <QTabWidget>
#include <QCoreApplication>
#include <QtDebug>
#include <QSpacerItem>
#include  <QColorDialog>

#include "config_dialog.h"

config_dialog::config_dialog(QWidget *i_oParent):
	KDialog(i_oParent)
{
	setCaption(i18n("Settings"));
	setButtons(KDialog::Ok | KDialog::Cancel);

	QLabel *l_sLabel;

	QTabWidget *l_oTabWidget = new QTabWidget(this);

	QWidget *l_oDocumentGroupBox = new QGroupBox(l_oTabWidget);
	l_oTabWidget->addTab(l_oDocumentGroupBox, i18n("Document settings"));

	QWidget *l_oGlobalGroupBox = new QGroupBox(l_oTabWidget);
	l_oTabWidget->addTab(l_oGlobalGroupBox, i18n("Global settings"));

	QWidget *l_oGlobalTemplateBox = new QGroupBox(l_oTabWidget);
	l_oTabWidget->addTab(l_oGlobalTemplateBox, i18n("Global variables"));


	QGridLayout *l_oDocumentGridLayout = new QGridLayout(l_oDocumentGroupBox);

	l_sLabel = new QLabel(l_oDocumentGroupBox);
	l_sLabel->setText(i18n("Background color"));
	l_oDocumentGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);

	l_sLabel = new QLabel(l_oDocumentGroupBox);
	l_sLabel->setText(i18n("Arrow color"));
	l_oDocumentGridLayout->addWidget(l_sLabel, 1, 0, 1, 1);

	l_sLabel = new QLabel(l_oDocumentGroupBox);
	l_sLabel->setText(i18n("Reference color"));
	l_oDocumentGridLayout->addWidget(l_sLabel, 2, 0, 1, 1);

	l_sLabel = new QLabel(l_oDocumentGroupBox);
	l_sLabel->setText(i18n("Preview pictures"));
	l_oDocumentGridLayout->addWidget(l_sLabel, 3, 0, 1, 1);

	m_oColorWidget = new QPushButton(l_oDocumentGroupBox);
	l_oDocumentGridLayout->addWidget(m_oColorWidget, 0, 1, 1, 1);
	connect(m_oColorWidget, SIGNAL(clicked()), this, SLOT(select_color()));

	m_oArrowWidget = new QPushButton(l_oDocumentGroupBox);
	l_oDocumentGridLayout->addWidget(m_oArrowWidget, 1, 1, 1, 1);
	connect(m_oArrowWidget, SIGNAL(clicked()), this, SLOT(select_arrow_color()));

	m_oAltArrowWidget = new QPushButton(l_oDocumentGroupBox);
	l_oDocumentGridLayout->addWidget(m_oAltArrowWidget, 2, 1, 1, 1);
	connect(m_oAltArrowWidget, SIGNAL(clicked()), this, SLOT(select_alt_arrow_color()));

	m_oPreviewPics = new QCheckBox(l_oDocumentGroupBox);
	l_oDocumentGridLayout->addWidget(m_oPreviewPics, 3, 1, 1, 1);

	QSpacerItem *l_oSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	l_oDocumentGridLayout->addItem(l_oSpacer, 4, 1, 2);



	QGridLayout *l_oGlobalGridLayout = new QGridLayout(l_oGlobalGroupBox);

	l_sLabel = new QLabel(l_oGlobalGroupBox);
	l_sLabel->setText(i18n("Reorganization type"));
	l_oGlobalGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);

	l_sLabel = new QLabel(l_oGlobalGroupBox);
	l_sLabel->setText(i18n("Item position after keyboard insertion"));
	l_oGlobalGridLayout->addWidget(l_sLabel, 1, 0, 1, 1);

	l_sLabel = new QLabel(l_oGlobalGroupBox);
	l_sLabel->setText(i18n("Connection type"));
	l_oGlobalGridLayout->addWidget(l_sLabel, 2, 0, 1, 1);

	l_sLabel = new QLabel(l_oGlobalGroupBox);
	l_sLabel->setText(i18n("Save document automatically"));
	l_oGlobalGridLayout->addWidget(l_sLabel, 3, 0, 1, 1);

	l_sLabel = new QLabel(l_oGlobalGroupBox);
	l_sLabel->setText(i18n("Touchpad mode"));
	l_oGlobalGridLayout->addWidget(l_sLabel, 4, 0, 1, 1);


	m_oReorgType = new QComboBox(l_oGlobalGroupBox);
	m_oReorgType->addItems(QStringList()<<i18n("Fixed")); //<<i18n("Force-based")<<i18n("Incremental"));
	l_oGlobalGridLayout->addWidget(m_oReorgType, 0, 1, 1, 1);

	m_oAutoReorg = new QComboBox(l_oGlobalGroupBox);
	m_oAutoReorg->addItems(QStringList()<<i18n("Close to parent")<<i18n("Reorganize map"));
	l_oGlobalGridLayout->addWidget(m_oAutoReorg, 1, 1, 1, 1);

	m_oConnType = new QComboBox(l_oGlobalGroupBox);
	m_oConnType->addItems(QStringList()<<i18n("Lines")<<i18n("Splines"));
	l_oGlobalGridLayout->addWidget(m_oConnType, 2, 1, 1, 1);

	m_oAutoSave = new QSpinBox(l_oGlobalGroupBox);
	m_oAutoSave->setSuffix(i18n(" min"));
	m_oAutoSave->setRange(0, 69 /* 69 hmmm */);
	l_oGlobalGridLayout->addWidget(m_oAutoSave, 3, 1, 1, 1);
	m_oAutoSave->setToolTip(i18n("Interval in minutes for saving the document automatically, 0 for disabling this feature"));

	m_oUseTouchpad = new QCheckBox(l_oGlobalGroupBox);
	l_oGlobalGridLayout->addWidget(m_oUseTouchpad, 4, 1, 1, 1);

	l_oSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	l_oGlobalGridLayout->addItem(l_oSpacer, 5, 1, 2);


	QGridLayout *l_oTemplateGridLayout = new QGridLayout(l_oGlobalTemplateBox);

	///l_sLabel = new QLabel(l_oGlobalTemplateBox);
	//l_sLabel->setText(i18n("Custom template values"));
	//l_oTemplateGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);

	m_oTextEdit = new config_editor(l_oGlobalTemplateBox);
	l_oTemplateGridLayout->addWidget(m_oTextEdit, 0, 0);

	setMainWidget(l_oTabWidget);

	QSize size(421, 150);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

void config_dialog::select_color()
{
	m_oColor = QColorDialog::getColor(m_oColor, this);
	if (!m_oColor.isValid()) return;
	m_oColorWidget->setText(m_oColor.name());

	QPalette l_oPalette = m_oColorWidget->palette();
	l_oPalette.setBrush(QPalette::Active, QPalette::Button, m_oColor);
	m_oColorWidget->setPalette(l_oPalette);
}

void config_dialog::select_arrow_color()
{
	m_oArrowColor = QColorDialog::getColor(m_oArrowColor, this);
	if (!m_oArrowColor.isValid()) return;
	m_oArrowWidget->setText(m_oArrowColor.name());

	QPalette l_oPalette = m_oArrowWidget->palette();
	l_oPalette.setBrush(QPalette::Active, QPalette::Button, m_oArrowColor);
	m_oArrowWidget->setPalette(l_oPalette);
}

void config_dialog::select_alt_arrow_color()
{
	m_oAltArrowColor = QColorDialog::getColor(m_oAltArrowColor, this);
	if (!m_oAltArrowColor.isValid()) return;
	m_oAltArrowWidget->setText(m_oAltArrowColor.name());

	QPalette l_oPalette = m_oAltArrowWidget->palette();
	l_oPalette.setBrush(QPalette::Active, QPalette::Button, m_oAltArrowColor);
	m_oAltArrowWidget->setPalette(l_oPalette);
}

config_editor::config_editor(QWidget *i_oParent) : QTextEdit(i_oParent)
{
	new config_highlighter(document());
	setStyleSheet("QTextEdit { font-family: monospace;}");
	setTabChangesFocus(true);
}

config_highlighter::config_highlighter(QTextDocument *p): QSyntaxHighlighter(p)
{
	config_highlighter_rule l_oRule;

	QTextCharFormat l_oSingle;
	l_oSingle.setForeground(Qt::darkBlue);
	l_oRule.m_oPattern = QRegExp("#[^\n]*");
	l_oSingle.setFontItalic(true);
	l_oRule.m_oFormat = l_oSingle;
	m_oRules.append(l_oRule);

	QTextCharFormat l_oKeyword;
	l_oKeyword.setForeground(Qt::darkGreen);
	l_oKeyword.setFontWeight(QFont::Bold);
	l_oRule.m_oPattern = QRegExp("^\\w+(\\.\\w+)*");
	l_oRule.m_oFormat = l_oKeyword;
	m_oRules.append(l_oRule);
}

void config_highlighter::highlightBlock(const QString &i_sText)
{
	foreach (config_highlighter_rule l_oRule, m_oRules)
	{
		QRegExp l_oExp(l_oRule.m_oPattern);
		int i = i_sText.indexOf(l_oExp);
		while (i >= 0)
		{
			int l_iLen = l_oExp.matchedLength();

			if (i_sText[i+l_iLen-1]==QChar('=')) l_iLen--;
			setFormat(i, l_iLen, l_oRule.m_oFormat);
			i = i_sText.indexOf(l_oExp, i + l_iLen);
		}
	}
}


