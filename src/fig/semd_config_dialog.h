// Thomas Nagy 2007-2018 GPLV3

#ifndef SEMD_CONFIG_DIALOG_H
#define SEMD_CONFIG_DIALOG_H

#include <KDE/KDialog>
#include <con.h>

class QCheckBox;
class QComboBox;
class QSpinBox;
class QTreeWidgetItem;
class QPushButton;
class semd_config_dialog : public KDialog
{
	public:
		semd_config_dialog(QWidget *i_oParent);
		QCheckBox *m_oUseTouchpad;
};

#endif

