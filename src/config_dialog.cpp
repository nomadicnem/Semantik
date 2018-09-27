// Thomas Nagy 2007-2018 GPLV3

#include <QVariant>
#include <QAction>
#include <QSpinBox>
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
#include <QCoreApplication>
#include <QtDebug>
#include  <QColorDialog>

#include "config_dialog.h"

config_dialog::config_dialog(QWidget *i_oParent):
	KDialog(i_oParent)
{
	QWidget *widget = new QWidget(this);
	setCaption(i18n("Document settings"));
	setButtons(KDialog::Ok | KDialog::Cancel);

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Reorganization type"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Item position after keyboard insertion"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0, 1, 1);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Connection type"));
	l_oGridLayout->addWidget(l_sLabel, 2, 0, 1, 1);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Save document automatically"));
	l_oGridLayout->addWidget(l_sLabel, 3, 0, 1, 1);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Background color"));
	l_oGridLayout->addWidget(l_sLabel, 4, 0, 1, 1);


	m_oReorgType = new QComboBox(widget);
	m_oReorgType->addItems(QStringList()<<i18n("Fixed")<<i18n("Force-based")<<i18n("Incremental"));
	l_oGridLayout->addWidget(m_oReorgType, 0, 1, 1, 1);

	m_oAutoReorg = new QComboBox(widget);
	m_oAutoReorg->addItems(QStringList()<<i18n("Close to parent")<<i18n("Reorganize map"));
	l_oGridLayout->addWidget(m_oAutoReorg, 1, 1, 1, 1);

	m_oConnType = new QComboBox(widget);
	m_oConnType->addItems(QStringList()<<i18n("Lines")<<i18n("Splines"));
	l_oGridLayout->addWidget(m_oConnType, 2, 1, 1, 1);

	m_oAutoSave = new QSpinBox(widget);
	m_oAutoSave->setSuffix(i18n(" min"));
	m_oAutoSave->setRange(0, 69 /* 69 hmmm */);
	l_oGridLayout->addWidget(m_oAutoSave, 3, 1, 1, 1);
	m_oAutoSave->setToolTip(i18n("Interval in minutes for saving the document automatically, 0 for disabling this feature"));

	m_oColorWidget = new QPushButton(widget);
	l_oGridLayout->addWidget(m_oColorWidget, 4, 1, 1, 1);
	connect(m_oColorWidget, SIGNAL(clicked()), this, SLOT(select_color()));

	setMainWidget(widget);

	QSize size(321, 120);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

void config_dialog::select_color()
{
	m_oColor = QColorDialog::getColor(m_oColor, this);
	if (!m_oColor.isValid()) return;
	m_oColorWidget->setText(m_oColor.name());

	QPalette l_oPalette = m_oColorWidget->palette();
	l_oPalette.setBrush(QPalette::Active, QPalette::Button, m_oColor);
	m_oColorWidget->setPalette(l_oPalette);
}

