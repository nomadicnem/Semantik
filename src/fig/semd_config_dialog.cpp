// Thomas Nagy 2007-2021 GPLV3

#include <QVariant>
#include <QAction>
#include <QSpinBox>
#include <QCheckBox>
#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <QPushButton>
#include <QGroupBox>
#include <QTabWidget>
#include <QCoreApplication>
#include <QtDebug>
#include <QSpacerItem>
#include  <QColorDialog>

#include "semd_config_dialog.h"

semd_config_dialog::semd_config_dialog(QWidget *i_oParent):
	KDialog(i_oParent)
{
	setCaption(i18n("Settings"));
	setButtons(KDialog::Ok | KDialog::Cancel);

	QLabel *l_sLabel;

	QTabWidget *l_oTabWidget = new QTabWidget(this);

	//QWidget *l_oDocumentGroupBox = new QGroupBox(l_oTabWidget);
	//l_oTabWidget->addTab(l_oDocumentGroupBox, i18n("Document settings and defaults"));

	QWidget *l_oGlobalGroupBox = new QGroupBox(l_oTabWidget);
	l_oTabWidget->addTab(l_oGlobalGroupBox, i18n("Global settings"));

	//QGridLayout *l_oDocumentGridLayout = new QGridLayout(l_oDocumentGroupBox);

	QGridLayout *l_oGlobalGridLayout = new QGridLayout(l_oGlobalGroupBox);

	l_sLabel = new QLabel(l_oGlobalGroupBox);
	l_sLabel->setText(i18n("Touchpad mode"));
	l_oGlobalGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);


	m_oUseTouchpad = new QCheckBox(l_oGlobalGroupBox);
	l_oGlobalGridLayout->addWidget(m_oUseTouchpad, 0, 1, 1, 1);

	QSpacerItem *l_oSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	l_oGlobalGridLayout->addItem(l_oSpacer, 1, 1, 2);


	setMainWidget(l_oTabWidget);

	QSize size(321, 120);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

