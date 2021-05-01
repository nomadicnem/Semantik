// Thomas Nagy 2007-2021 GPLV3

#include <QSpinBox>
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>

#include <QtDebug>

#include "table_dialog.h"

table_dialog::table_dialog(QWidget *i_oParent):
	QDialog(i_oParent)
{
	QGridLayout *l_oGridLayout = new QGridLayout(this);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(this);
	l_sLabel->setText(i18n("Rows"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(i18n("Columns"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0, 1, 1);


	m_oRows = new QSpinBox(this);
	l_oGridLayout->addWidget(m_oRows, 0, 1, 1, 1);

	m_oCols = new QSpinBox(this);
	l_oGridLayout->addWidget(m_oCols, 1, 1, 1, 1);


	QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	l_oGridLayout->addItem(spacerItem, 2, 1, 1, 1);


	QDialogButtonBox *l_oButtonBox = new QDialogButtonBox(this);
	l_oButtonBox->setOrientation(Qt::Horizontal);
	l_oButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

	l_oGridLayout->addWidget(l_oButtonBox, 3, 0, 1, 2);

	connect(l_oButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(l_oButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QSize size(321, 120);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

