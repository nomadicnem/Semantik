// Thomas Nagy 2018-2020 GPLV3

#include <KMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QAction>
#include <KDE/KDialog>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QLineEdit>
#include <QTextEdit>
#include <QRegExp>
#include <QTreeView>
#include <QStandardItem>
#include <QKeyEvent>
#include <QFont>
#include "mem_box.h"
#include "box_view.h"
#include "special_edit.h"
#include <QtDebug>

#include "box_entity_properties.h"

entity_highlighter::entity_highlighter(QTextDocument *p): QSyntaxHighlighter(p)
{
	entity_highlighter_rule l_oRule;

	QTextCharFormat l_oSingle;
	l_oSingle.setForeground(Qt::darkGreen);
	l_oRule.m_oPattern = QRegExp("^\\s*entity\\s+");
	l_oSingle.setFontWeight(QFont::Bold);
	l_oRule.m_oFormat = l_oSingle;
	m_oRules.append(l_oRule);

	QTextCharFormat l_oKeyword;
	l_oKeyword.setForeground(Qt::darkGreen);
	l_oKeyword.setFontWeight(QFont::Bold);
	l_oRule.m_oPattern = QRegExp("\\s\\([a-zA-Z0-9_- /|]+\\)\\s*$");
	l_oRule.m_oFormat = l_oKeyword;
	m_oRules.append(l_oRule);
}

void entity_highlighter::highlightBlock(const QString &i_sText)
{
	foreach (entity_highlighter_rule l_oRule, m_oRules)
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

box_entity_properties::box_entity_properties(QWidget *i_oParent, box_entity *i_oEntity):
	KDialog(i_oParent), m_oEntity(i_oEntity)
{
	QWidget *widget = new QWidget(this);
	setCaption(i18n("Entity properties"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	m_oEntityDefinition = new special_edit(widget);
	new entity_highlighter(m_oEntityDefinition->document());

	l_oGridLayout->addWidget(m_oEntityDefinition, 0, 0);
        setMainWidget(widget);
        QSize size(700, 320);
        resize(size.expandedTo(minimumSizeHint()));

	connect(m_oEntityDefinition, SIGNAL(textChanged()), this, SLOT(enable_apply()));

        connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
        connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	m_oEntityDefinition->setFocus();
}

void box_entity_properties::enable_apply(int) {
	enableButtonApply(true);
}

void box_entity_properties::enable_apply() {
	enableButtonApply(true);
}

void box_entity_properties::apply() {
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	QStringList l_oTmp = m_oEntityDefinition->toPlainText().split(notr("\n"));

	if (l_oTmp.size() < 1) {
		KMessageBox::sorry(this, i18n("No entity name given"), i18n("Missing entity name"));
		return;
	}

	QRegExp rx("entity\\s+(\\w.*)");
	if (rx.indexIn(l_oTmp[0]) != -1)
	{
		mem_entity *mem = new mem_entity(m_oEntity->m_oView->m_oMediator, m_oEntity->m_oView->m_iId);
		mem->init(m_oEntity->m_oBox);

		mem->m_oNewBox.m_sText = rx.cap(1);
		mem->m_oNewBox.m_oEntityValues.clear();

		QRegExp l_oRegName("^(\\S+)");
		QRegExp l_oRegKey("\\s+\\(([a-zA-Z0-9_- /|]+)\\)\\s*$");
		for (int i=1; i < l_oTmp.size(); ++i)
		{
			QString l_s = l_oTmp[i].trimmed();
			if (l_oRegName.indexIn(l_s) >= 0)
			{
				data_box_entity_value l_o;
				l_o.m_sName = l_oRegName.cap(1);

				int l_iLen = l_oRegName.cap(0).size();
				QString l_sText = l_s.mid(l_iLen).trimmed();
				if (!l_sText.isEmpty() && l_iLen < l_s.size())
				{
					if (l_oRegKey.lastIndexIn(l_sText) >= 0)
					{
						l_o.m_sKey = l_oRegKey.cap(1);
						int l_iLeftLen = l_sText.size() - l_oRegKey.cap(0).size();
						if (l_iLeftLen > 0)
						{
							l_o.m_sType = l_sText.mid(0, l_iLeftLen).trimmed();
						}
					}
					else
					{
						l_o.m_sType = l_sText.trimmed();
					}
				}
				mem->m_oNewBox.m_oEntityValues.push_back(l_o);
			}
		}

		// must ask the ui to know the size :/
		*m_oEntity->m_oBox = mem->m_oNewBox;
		m_oEntity->force_size();
		//if (m_oEntity->m_iWW > mem->m_oOldBox.m_iWW or m_oEntity->m_iHH > mem->m_oOldBox.m_iHH)
		{
			mem->m_oNewBox.m_iWW = m_oEntity->m_iWW;
			mem->m_oNewBox.m_iHH = m_oEntity->m_iHH;
		}

		mem->apply();
		enableButtonApply(false);
	} else {
		qDebug()<<"no match for"<<l_oTmp[0];
		return;
	}
}

