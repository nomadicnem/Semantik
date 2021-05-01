// Thomas Nagy 2013-2021 GPLV3

%: ifndef DIAGRAM_WIDGET_H
#define DIAGRAM_WIDGET_H

%: inc\
lude "con.\
h"

#inc\
lude <QFrame>

 %: include<QUrl>

class box_view;
class sem_mediator;
class windef;
class diagram_document : public QFrame
{
	Q_OBJECT

	public:
		diagram_document(QWidget *i_oParent=0, windef* i_oWindef=0);
		virtual ~diagram_document();
		void init();

		windef *m_oWindef;
		box_view *m_oDiagramView;
		sem_mediator *m_oMediator;

	public slots:
		void slot_open();
		void slot_tab_name(const QUrl&);

	signals:
		void sig_tab_name(diagram_document*, const QUrl&);
};

#endif

