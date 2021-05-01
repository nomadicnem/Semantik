// Thomas Nagy 2018-2021 GPLV3

#include <QLabel>
#include <QGridLayout>
#include "box_item.h"
#include "mem_box.h"
#include <special_edit.h>
#include <QDebug>

void special_edit::keyPressEvent(QKeyEvent *i_oEv)
{
	if (i_oEv->key() == Qt::Key_Enter || i_oEv->key() == Qt::Key_Return)
	{
		if (i_oEv->modifiers() & Qt::AltModifier)
		{
			i_oEv->setModifiers(Qt::NoModifier);
			QTextEdit::keyPressEvent(i_oEv);
		}
		else
		{
			i_oEv->ignore();
		}
	}
	else
	{
		QTextEdit::keyPressEvent(i_oEv);
	}
}

special_edit::special_edit(QWidget *i_oParent) : QTextEdit(i_oParent)
{
        setStyleSheet("QTextEdit { font-family: monospace;}");
	setTabChangesFocus(true);
}

special_edit_properties::special_edit_properties(QWidget *i_oParent, box_item *i_oItem):
	KDialog(i_oParent), m_oItem(i_oItem)
{
	QWidget *widget = new QWidget(this);
	setCaption(i18n("Diagram box properties:"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	QLabel *l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Text:"));
		l_oGridLayout->addWidget(l_sLabel, 0, 0);

	m_oTextEdit = new special_edit(widget);
	l_oGridLayout->addWidget(m_oTextEdit, 1, 0);
	setMainWidget(widget);
	QSize size(400, 220);
	resize(size.expandedTo(minimumSizeHint()));

	connect(m_oTextEdit, SIGNAL(textChanged()), this, SLOT(enable_apply()));

        connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
        connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	m_oTextEdit->setFocus();
	m_oTextEdit->setPlainText(i_oItem->m_oBox->m_sText);
	m_oTextEdit->selectAll();
}

void special_edit_properties::enable_apply(int) {
	enableButtonApply(m_oTextEdit->toPlainText() != m_oItem->m_oBox->m_sText);
}

void special_edit_properties::enable_apply() {
	enableButtonApply(m_oTextEdit->toPlainText() != m_oItem->m_oBox->m_sText);
}

void special_edit_properties::apply()
{
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	if (m_oTextEdit->toPlainText() != m_oItem->m_oBox->m_sText)
	{
		mem_edit_box *ed = new mem_edit_box(m_oItem->m_oView->m_oMediator, m_oItem->m_oView->m_iId, m_oItem->m_oBox->m_iId);
		ed->newText = m_oTextEdit->toPlainText();

		QTextDocument doc;
		doc.setDefaultFont(m_oItem->scene()->font());
		QTextOption l_oOption = doc.defaultTextOption();
		l_oOption.setAlignment(m_oItem->m_oBox->m_iAlign);
		doc.setDefaultTextOption(l_oOption);
		doc.setPlainText(ed->newText);
		doc.setTextWidth(m_oItem->m_oBox->m_iWW - 2 * OFF);

		int l_iNewHeight = fceil(doc.size().height() + 2 * OFF, GRID);
		if (m_oItem->m_oBox->m_iType == data_box::DECISION)
		{
			// do nothing
		}
		else if (m_oItem->m_oBox->m_iType == data_box::SEQUENCE)
		{
			ed->m_iNewBoxHeight = qMax(l_iNewHeight, m_oItem->m_oBox->m_iBoxHeight);
			ed->newHeight += ed->m_iNewBoxHeight - ed->m_iOldBoxHeight;
		}
		else
		{
			ed->newHeight = qMax(l_iNewHeight, m_oItem->m_oBox->m_iHH);
		}

		ed->apply();
	}
	enableButtonApply(false);
}

