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
		QColor m_oColor;

	public slots:
		void select_color();
};

#endif

