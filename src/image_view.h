// Thomas Nagy 2007-2019 GPLV3

#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <QSortFilterProxyModel>
#include <QSplitter>
#include "c\
on.h"
#include <QUrl>

class sem_mediator;
class QMenu;
class image_view : public QWidget
{
	Q_OBJECT
	public:
		image_view(QWidget*, sem_mediator*);
		sem_mediator *m_oMediator;
		QPixmap m_oPixmap;
		int m_iId;

		// yes, we could use a QLabel...
		void paintEvent(QPaintEvent*);

		QMenu *m_oMenu;

		QAction *m_oClearPictureAction;
		QAction *m_oChangePictureAction;

		bool event(QEvent*);
		void mouseReleaseEvent(QMouseEvent*);
		void do_change_pic(const QUrl&);

		void dragEnterEvent(QDragEnterEvent*);
		void dragMoveEvent(QDragMoveEvent*);
		void dropEvent(QDropEvent*);
		void dragLeaveEvent(QDragLeaveEvent*);

	signals:
		void sig_message(const QString&, int);

	public slots:
		void clear_pic();
		void change_pic();
		void notify_pic(int id);
		void context_menu(const QPoint&);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_export_item(int id);
};

#endif

