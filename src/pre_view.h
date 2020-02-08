// Thomas Nagy 2007-2020 GPLV3

#ifndef PRE_VIEW_H
#define PRE_VIEW_H

#include <QStackedWidget>
#include  	"con.h"

class browser;
class QLabel;
class QWebEngineView;
class sem_mediator;
class pre_view : public QStackedWidget
{
	Q_OBJECT
	public:
		pre_view(QWidget *i_oParent, sem_mediator *i_oControl);
		sem_mediator *m_oMediator;

		QLabel *m_oLabel;
		browser *m_oBrowser;

	public slots:
		void notify_preview();
};

#endif

