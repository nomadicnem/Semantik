// Thomas Nagy 2007-2019 GPLV3

#include <QColorDialog>
#include <QAction>
#include <QtDebug>
#include <QTextEdit>
#include  <QTextList>  
#include <QGridLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QPalette>
#include <QCoreApplication>
#include <QApplication>
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

	m_iId = NO_ITEM;

	m_oMediator = i_oControl;

	m_oEdit = new QTextEdit(this);
	new Sonnet::SpellCheckDecorator(m_oEdit);
	//m_oEdit->setCheckSpellingEnabled(true);
	l_oLayout->addWidget(m_oEdit, 1, 0);
	l_oLayout->setMargin(0);
	setMinimumHeight(30);


	int l_iSize = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);

	QToolBar *l_oToolBar = new QToolBar(this);
	l_oToolBar->setIconSize(QSize(l_iSize, l_iSize));
	l_oLayout->addWidget(l_oToolBar, 0, 0);


	connect(m_oEdit, SIGNAL(textChanged()), this, SLOT(update_edit()));
	connect(m_oEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(char_format_changed(const QTextCharFormat &)));

	m_oBoldAct = l_oToolBar->addAction(QIcon::fromTheme(notr("format-text-bold")), i18n("&Bold"));
	m_oBoldAct->setShortcut(i18n("Ctrl+B"));
	m_oBoldAct->setCheckable(true);

	m_oItalicAct = l_oToolBar->addAction(QIcon::fromTheme(notr("format-text-italic")), i18n("&Italic"));
	m_oItalicAct->setShortcut(i18n("Ctrl+I"));
	m_oItalicAct->setCheckable(true);

	m_oUnderLineAct = l_oToolBar->addAction(QIcon::fromTheme(notr("format-text-underline")), i18n("&Underline"));
	m_oUnderLineAct->setShortcut(i18n("Ctrl+U"));
	m_oUnderLineAct->setCheckable(true);

	m_oTextColorAct = l_oToolBar->addAction(QIcon::fromTheme(notr("color-picker")), i18n("Text color"));

	m_oLinkAct = l_oToolBar->addAction(QIcon::fromTheme(notr("link")), i18n("&Link"));
	m_oLinkAct->setShortcut(i18n("Ctrl+L"));
	l_oToolBar->insertSeparator(m_oLinkAct);

	m_oTextBulletAct = l_oToolBar->addAction(QIcon::fromTheme(notr("format-list-unordered")), i18n("Unordered list"));
	m_oTextBulletAct->setCheckable(true);
	l_oToolBar->insertSeparator(m_oTextBulletAct);
	m_oTextNumberAct = l_oToolBar->addAction(QIcon::fromTheme(notr("format-list-ordered")), i18n("Ordered list"));
	m_oTextNumberAct->setCheckable(true);

	m_oIncreaseIndentAct = l_oToolBar->addAction(QIcon::fromTheme(notr("format-indent-more")), i18n("Increase indentation"));
	m_oDecreaseIndentAct = l_oToolBar->addAction(QIcon::fromTheme(notr("format-indent-less")), i18n("Decrease indentation"));

	m_oClearAct = l_oToolBar->addAction(QIcon::fromTheme(notr("edit-clear-all-symbolic")), i18n("Clear formatting"));
	l_oToolBar->insertSeparator(m_oClearAct);

	connect(m_oBoldAct, SIGNAL(triggered()), this, SLOT(text_bold()));
	connect(m_oItalicAct, SIGNAL(triggered()), this, SLOT(text_italic()));
	connect(m_oUnderLineAct, SIGNAL(triggered()), this, SLOT(text_underLine()));
	connect(m_oLinkAct, SIGNAL(triggered()), this, SLOT(text_link()));
	connect(m_oTextColorAct, SIGNAL(triggered()), this, SLOT(text_color()));
	connect(m_oClearAct, SIGNAL(triggered()), this, SLOT(text_clear()));
	connect(m_oTextBulletAct, SIGNAL(triggered(bool)), this, SLOT(text_bullet(bool)));
	connect(m_oTextNumberAct, SIGNAL(triggered(bool)), this, SLOT(text_number(bool)));
	//connect(m_oEdit, SIGNAL(languageChanged(const QString &)), this, SLOT(spelling_language_changed(const QString &)));
	connect(m_oEdit, SIGNAL(selectionChanged()), this, SLOT(selection_changed()));
	connect(m_oEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursor_changed()));
	connect(m_oIncreaseIndentAct, SIGNAL(triggered()), this, SLOT(increase_indent()));
	connect(m_oDecreaseIndentAct, SIGNAL(triggered()), this, SLOT(decrease_indent()));
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

void text_view::cursor_changed()
{
	QTextCursor l_oCursor = m_oEdit->textCursor();
	QTextList *l_oList = l_oCursor.currentList();
	if (l_oList == NULL)
	{
		m_oTextBulletAct->setChecked(false);
		m_oTextNumberAct->setChecked(false);
	}
	else
	{
		QTextListFormat::Style l_oStyle = l_oList->format().style();
		bool isBullet = l_oStyle == QTextListFormat::ListDisc || l_oStyle == QTextListFormat::ListCircle || l_oStyle == QTextListFormat::ListSquare;
		m_oTextBulletAct->setChecked(isBullet);
		m_oTextNumberAct->setChecked(!isBullet);
	}
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

		QTextCharFormat l_oFormat;
		if (l_oAnchor.isEmpty())
		{
			l_oFormat.setAnchor(false);
			l_oFormat.setFontUnderline(false);
			l_oFormat.setAnchorHref(QString());
			l_oFormat.setForeground(palette().color(QPalette::Text));
		}
		else
		{
			l_oFormat.setAnchor(true);
			l_oFormat.setFontUnderline(true);
			l_oFormat.setAnchorHref("http://www.google.com");
			l_oFormat.setForeground(palette().color(QPalette::Link));
		}

		merge_format(l_oFormat);
		update_edit();
	}
}

void text_view::text_clear()
{
	QTextCharFormat l_oFormat;
	l_oFormat.setAnchor(false);
	l_oFormat.setFontUnderline(false);
	l_oFormat.setFontWeight(QFont::Normal);
	l_oFormat.setFontItalic(false);
	l_oFormat.setFontStrikeOut(false);
	l_oFormat.setAnchorHref(QString());
	l_oFormat.setForeground(palette().color(QPalette::Text));
	l_oFormat.clearBackground();
	merge_format(l_oFormat);
	update_edit();
}

void text_view::text_color()
{
	QColor l_oColor = QColorDialog::getColor(m_oEdit->textColor(), this);
        if (!l_oColor.isValid()) return;
	QTextCharFormat l_oFormat;
	l_oFormat.setForeground(l_oColor);
	merge_format(l_oFormat);
	update_edit();
}

void text_view::text_bullet(bool i_bEnable)
{
	text_list(i_bEnable, QTextListFormat::ListDisc);
}

void text_view::text_number(bool i_bEnable)
{
	text_list(i_bEnable, QTextListFormat::ListDecimal);
}

void text_view::text_list(bool i_bEnable, QTextListFormat::Style i_oStyle)
{
	QTextCursor l_oCursor = m_oEdit->textCursor();
	if (i_bEnable)
	{
		QTextList *l_oList = l_oCursor.currentList();
		if (l_oList != NULL)
		{
			QTextListFormat l_oListFormat = l_oList->format();
			l_oListFormat.setStyle(i_oStyle);
			l_oCursor.createList(l_oListFormat);
		}
		else
		{
			QTextListFormat l_oListFormat;
			l_oListFormat.setStyle(i_oStyle);
			l_oCursor.createList(l_oListFormat);
		}
	}
	else
	{
		QTextBlockFormat l_oBlockFormat;
		l_oBlockFormat.setIndent(l_oCursor.blockFormat().indent());
		l_oCursor.setBlockFormat(l_oBlockFormat);
	}
	update_edit();
}

void text_view::increase_indent()
{
	change_indent(1);
}

void text_view::decrease_indent()
{
	change_indent(-1);
}

void text_view::change_indent(int i_iIndent)
{
	QTextCursor l_oCursor = m_oEdit->textCursor();
	QTextList *l_oList = l_oCursor.currentList();
	if (l_oList != NULL)
	{
		QTextListFormat l_oListFormat = l_oList->format();
		int l_iVal = (int) l_oListFormat.style() - i_iIndent;
		QTextListFormat::Style l_oStyle;
		switch (l_iVal)
		{
			case QTextListFormat::ListDisc:
				l_oStyle = QTextListFormat::ListDisc;
				break;
			case QTextListFormat::ListCircle:
				l_oStyle = QTextListFormat::ListCircle;
				break;
			case QTextListFormat::ListSquare:
				l_oStyle = QTextListFormat::ListSquare;
				break;
			case QTextListFormat::ListDecimal:
				l_oStyle = QTextListFormat::ListDecimal;
				break;
			case QTextListFormat::ListLowerAlpha:
				l_oStyle = QTextListFormat::ListLowerAlpha;
				break;
			case QTextListFormat::ListUpperAlpha:
				l_oStyle = QTextListFormat::ListUpperAlpha;
				break;
			case QTextListFormat::ListLowerRoman:
				l_oStyle = QTextListFormat::ListLowerRoman;
				break;
			case QTextListFormat::ListUpperRoman:
				l_oStyle = QTextListFormat::ListUpperRoman;
				break;
			default:
				l_oStyle = QTextListFormat::ListDisc;
				break;
		}

		if (i_iIndent > 0 || l_oListFormat.indent() + i_iIndent >= 1)
		{
			l_oListFormat.setIndent(l_oListFormat.indent() + i_iIndent);
			l_oListFormat.setStyle(l_oStyle);
			l_oCursor.createList(l_oListFormat);
		}
	}
}
