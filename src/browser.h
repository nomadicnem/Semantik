// Thomas Nagy 2007-2020 GPLV3

#include <QWebEngineView>

#ifndef _BROWSER_H
#define _BROWSER_H

class QMenu;
class browser: public QWebEngineView
{
	Q_OBJECT
	public:
		browser(QWidget*);
		void contextMenuEvent(QContextMenuEvent * event);
	public slots:
		void toggleFullScreen();
	private:
		QMenu * m_oMenu;
};

#endif

