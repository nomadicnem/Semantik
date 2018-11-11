// Thomas Nagy 2012 GPLV3

#ifndef BOX_FRAME_PROPERTIES_H
#define BOX_FRAME_PROPERTIES_H

#include <KDE/KDialog>
#include "box_frame.h"

class QComboBox;
class QTextEdit;
class box_frame_properties : public KDialog
{
	Q_OBJECT
	public:
		box_frame_properties(QWidget*, box_frame*);
		QComboBox *m_oLabelPosition;
		QLineEdit *m_oEdit;
		box_frame *m_oFrame;

	public slots:
		void apply();
		void enable_apply(int);
		void enable_apply(const QString &);
};

#endif

