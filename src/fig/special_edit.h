// Thomas Nagy 2018-2020 GPLV3

#ifndef SPECIAL_EDIT_H
#define SPECIAL_EDIT_H

#include <QTextEdit>
#include <QKeyEvent>
#include "con.h"
#include <KDE/KDialog>

class box_item;

#define OFF 3

class special_edit : public QTextEdit
{
	public:
		special_edit(QWidget *i_oParent);
		void keyPressEvent(QKeyEvent *i_oEv);
};

class special_edit_properties : public KDialog
{
	Q_OBJECT
	public:
		special_edit_properties(QWidget*, box_item*);
		special_edit *m_oTextEdit;
		box_item *m_oItem;

	public slots:
		void apply();
		void enable_apply(int);
		void enable_apply();
};


#endif // SPECIAL_EDIT_H

