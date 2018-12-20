// Thomas Nagy 2007-2018 GPLV3

#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <KDE/KDialog>
#include <con.h>

class QCheckBox;
class QComboBox;
class QSpinBox;
class QTreeWidgetItem;
class QPushButton;
class config_dialog : public KDialog
{
	Q_OBJECT
	public:
		config_dialog(QWidget *i_oParent);
		QComboBox *m_oReorgType;
		QComboBox *m_oAutoReorg;
		QComboBox *m_oConnType;
		QSpinBox *m_oAutoSave;
		QCheckBox *m_oUseTouchpad;
		QCheckBox *m_oPreviewPics;
		QPushButton *m_oColorWidget;
		QPushButton *m_oArrowWidget;
		QPushButton *m_oAltArrowWidget;
		QColor m_oColor;
		QColor m_oArrowColor;
		QColor m_oAltArrowColor;

	public slots:
		void select_color();
		void select_arrow_color();
		void select_alt_arrow_color();
};

#endif

