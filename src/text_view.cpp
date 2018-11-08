// Thomas Nagy 2007-2018 GPLV3

#include <QAction>
#include <QtDebug>
#include <QTextEdit>
#include <QGridLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QPalette>
#include <QCoreApplication>
#include <QToolButton>

#include <sonnet/spellcheckdecorator.h>

//#include "aux.h"
//#include "con.h"
#include "data_item.h"
#include "sem_mediator.h"
#include "text_view.h"
#include "semantik.h"
#include "mem_command.h"

text_view::text_view(QWidget *i_oParent, sem_mediator *i_oControl) : QWidget(i_oParent)
{
	QGridLayout *l_oLayout = new QGridLayout();
	setLayout(l_oLayout);

	m_oMediator = i_oControl;

	m_oEdit = new QTextEdit(this);
	new Sonnet::SpellCheckDecorator(m_oEdit);
	//m_oEdit->setCheckSpellingEnabled(true);
	l_oLayout->addWidget(m_oEdit, 1, 0, 1, 4);
	l_oLayout->setMargin(0);
	setMinimumHeight(30);

	m_iId = NO_ITEM;

	connect(m_oEdit, SIGNAL(textChanged()), this, SLOT(update_edit()));
	connect(m_oEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(char_format_changed(const QTextCharFormat &)));

	m_oLinkAct = new QAction(QIcon::fromTheme(notr("link")), i18n("&Link"), this);
	m_oLinkAct->setShortcut(i18n("Ctrl+L"));

	m_oBoldAct = new QAction(QIcon::fromTheme(notr("format-text-bold")), i18n("&Bold"), this);
	m_oBoldAct->setShortcut(i18n("Ctrl+B"));
	m_oBoldAct->setCheckable(true);

	m_oItalicAct = new QAction(QIcon::fromTheme(notr("format-text-italic")), i18n("&Italic"), this);
	m_oItalicAct->setShortcut(i18n("Ctrl+I"));
	m_oItalicAct->setCheckable(true);

	m_oUnderLineAct = new QAction(QIcon::fromTheme(notr("format-text-underline")), i18n("&Underline"), this);
	m_oUnderLineAct->setShortcut(i18n("Ctrl+U"));
	m_oUnderLineAct->setCheckable(true);

	QToolButton *l_o = NULL;

	l_o = new QToolButton(this);
	l_o->setDefaultAction(m_oLinkAct);
	l_oLayout->addWidget(l_o, 0, 1);

	l_o = new QToolButton(this);
	l_o->setDefaultAction(m_oBoldAct);
	l_oLayout->addWidget(l_o, 0, 2);

	l_o = new QToolButton(this);
	l_o->setDefaultAction(m_oItalicAct);
	l_oLayout->addWidget(l_o, 0, 3);

	l_o = new QToolButton(this);
	l_o->setDefaultAction(m_oUnderLineAct);
	l_oLayout->addWidget(l_o, 0, 4);

	connect(m_oLinkAct, SIGNAL(triggered()), this, SLOT(text_link()));
	connect(m_oBoldAct, SIGNAL(triggered()), this, SLOT(text_bold()));
	connect(m_oItalicAct, SIGNAL(triggered()), this, SLOT(text_italic()));
	connect(m_oUnderLineAct, SIGNAL(triggered()), this, SLOT(text_underLine()));
	//connect(m_oEdit, SIGNAL(languageChanged(const QString &)), this, SLOT(spelling_language_changed(const QString &)));
	connect(m_oEdit, SIGNAL(selectionChanged()), this, SLOT(selection_changed()));
}

void text_view::update_edit()
{
	if (!m_iId) return;
	data_item& l_oData = m_oMediator->m_oItems[m_iId];

	mem_text* tmp = NULL;

	mem_command *c = NULL;
	if (!m_oMediator->m_oUndoStack.empty())
	{
		c = m_oMediator->m_oUndoStack.pop();
		m_oMediator->m_oUndoStack.push(c);
		if (c->type() != mem_command::TEXT)
		{
			c = NULL;
		}
	}

	tmp = (mem_text*) c;
	if (!c) {
		tmp = new mem_text(m_oMediator);
		tmp->m_iId = l_oData.m_iId;
		tmp->oldText = l_oData.m_sText;
		tmp->add();
	}
	tmp->newText = m_oMediator->m_oItems[tmp->m_iId].m_sText = m_oEdit->toHtml().replace(QChar(0), "");
	m_oMediator->m_oItems[m_iId].m_iTextLength = m_oEdit->toPlainText().length();
}

void text_view::text_bold()
{
	QTextCharFormat i_oFormat;
	i_oFormat.setFontWeight(m_oBoldAct->isChecked() ? QFont::Bold : QFont::Normal);
	merge_format(i_oFormat);
	update_edit();
}

void text_view::text_underLine()
{
	QTextCharFormat i_oFormat;
	i_oFormat.setFontUnderline(m_oUnderLineAct->isChecked());
	merge_format(i_oFormat);
	update_edit();
}

void text_view::text_italic()
{
	QTextCharFormat i_oFormat;
	i_oFormat.setFontItalic(m_oItalicAct->isChecked());
	merge_format(i_oFormat);
	update_edit();
}

void text_view::merge_format(const QTextCharFormat &i_oFormat)
{
	QTextCursor cursor = m_oEdit->textCursor();
	if (!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(i_oFormat);
	m_oEdit->mergeCurrentCharFormat(i_oFormat);
}

void text_view::char_format_changed(const QTextCharFormat &i_oFormat)
{
	QFont l_oF = i_oFormat.font();
	if (!m_oWin) return;

	m_oBoldAct->setChecked(l_oF.bold());
	m_oItalicAct->setChecked(l_oF.italic());
	m_oUnderLineAct->setChecked(l_oF.underline());
}

void text_view::notify_text(int id) {
	if (id == m_iId) {
		m_iId = NO_ITEM; // do not trigger the signal changed
		data_item& sel = *m_oMediator + id;
		m_oEdit->setHtml(sel.m_sText);
		m_iId = id;
	}
}

void text_view::spelling_language_changed(const QString & i_sLang) {
	//m_oMediator->m_sSpellingLanguage = i_sLang;
}

void text_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {
	bool one = (sel.size() == 1);
	m_iId = NO_ITEM;

	m_oEdit->setReadOnly(!one);
	m_oEdit->setEnabled(one);

	// FIXME
	//m_oEdit->setCheckSpellingEnabled(one);
	//if (!m_oMediator->m_sSpellingLanguage.isEmpty()) {
	//	m_oEdit->setSpellCheckingLanguage(m_oMediator->m_sSpellingLanguage);
	//}

	m_oBoldAct->setEnabled(one);
	m_oItalicAct->setEnabled(one);
	m_oUnderLineAct->setEnabled(one);

	if (one) {
		data_item& l_oData = m_oMediator->m_oItems[sel.at(0)];
		m_oEdit->setHtml(l_oData.m_sText);
		m_iId = sel.at(0);
	} else {
		m_oEdit->clear();
	}
}

void text_view::selection_changed()
{
	QTextCursor l_oCursor = m_oEdit->textCursor();
	m_oLinkAct->setEnabled(l_oCursor.hasSelection() || !m_oEdit->currentCharFormat().anchorHref().isEmpty());
}

void text_view::text_link()
{
	bool ok = false;
	QString l_oAnchor = QInputDialog::getText(m_oEdit, i18n("Link definition"),
			i18n("Link:"), QLineEdit::Normal, m_oEdit->currentCharFormat().anchorHref(), &ok).trimmed();
	if (ok)
	{
		QTextCursor l_oCursor = m_oEdit->textCursor();

		QTextCharFormat i_oFormat;
		if (l_oAnchor.isEmpty())
		{
			i_oFormat.setAnchor(false);
			i_oFormat.setFontUnderline(false);
			i_oFormat.setAnchorHref(QString());
			i_oFormat.setForeground(palette().color(QPalette::Text));
		}
		else
		{
			i_oFormat.setAnchor(true);
			i_oFormat.setFontUnderline(true);
			i_oFormat.setAnchorHref("http://www.google.com");
			i_oFormat.setForeground(palette().color(QPalette::Link));
		}

		merge_format(i_oFormat);
		update_edit();
	}
}

