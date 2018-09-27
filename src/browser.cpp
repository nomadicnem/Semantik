// Thomas Nagy 2007-2018 GPLV3

#include "browser.h"
#include <QtDebug>
#include <KLocalizedString>
#include <QMenu>
#include <QContextMenuEvent>

browser::browser(QWidget *w) : QWebEngineView(w)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	m_oMenu = NULL;
}

void browser::contextMenuEvent(QContextMenuEvent * event)
{
	if (!m_oMenu)
	{
		m_oMenu = new QMenu(this);
        m_oMenu->addAction(pageAction(QWebEnginePage::Reload));

		QAction *toggle = new QAction(i18n("Toggle fullscreen"), this);
		connect(toggle, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
		m_oMenu->addAction(toggle);
	}
	m_oMenu->popup(event->globalPos());

}

void browser::toggleFullScreen()
{
	if (isFullScreen())
	{
		setWindowModality(Qt::NonModal);
		//setFullScreen(false);
		setWindowState(Qt::WindowNoState);
		setWindowFlags(Qt::Widget);
		show();
	}
	else
	{
		setWindowFlags(Qt::Window);
		//setFullScreen(true);
		setWindowState(Qt::WindowFullScreen);
		setWindowModality(Qt::ApplicationModal);
		show();
	}
}

