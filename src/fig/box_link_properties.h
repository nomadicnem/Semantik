// Thomas Nagy 2012 GPLV3

#ifndef BOX_LINK_PROPERTIES_H
#define BOX_LINK_PROPERTIES_H

#include <KDE/KDialog>
#include "box_link.h"

class QSpinBox;
class QComboBox;
class QLineEdit;
class box_link_properties : public KDialog
{
	Q_OBJECT
	public:
		box_link_properties(QWidget*, box_link*);
		QSpinBox *m_oThickness;
		QComboBox *m_oType;
		QComboBox *m_oStyle;
		QComboBox *m_oLeftArrow;
		QComboBox *m_oRightArrow;
		QLineEdit *m_oArrowEdit;
		QLineEdit *m_oLeftArrowEdit;
		QLineEdit *m_oRightArrowEdit;

		box_link *m_oLink;

	public slots:
		void apply();
		void enable_apply(int);
		void enable_apply(const QString &);
};

#endif

