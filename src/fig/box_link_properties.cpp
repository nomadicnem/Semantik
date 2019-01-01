// Thomas Nagy 2007-2019 GPLV3

#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QLineEdit>
#include <QDirModel>
#include <QCompleter>
#include "mem_box.h"
#include "box_view.h"
#include "sem_mediator.h"

#include <QtDebug>

#include "box_link_properties.h"

box_link_properties::box_link_properties(QWidget *i_oParent, box_link *i_oLink):
	KDialog(i_oParent)
{
	QWidget *widget = new QWidget(this);
	setCaption(i18n("Link properties"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);
	m_oLink = i_oLink;

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	QLabel *l_sLabel;


	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Link text"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Origin cardinality"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Target cardinality"));
	l_oGridLayout->addWidget(l_sLabel, 2, 0);


	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Thickness"));
	l_oGridLayout->addWidget(l_sLabel, 3, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Line Type"));
	l_oGridLayout->addWidget(l_sLabel, 4, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Line Style"));
	l_oGridLayout->addWidget(l_sLabel, 5, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Origin arrow"));
	l_oGridLayout->addWidget(l_sLabel, 6, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(i18n("Target arrow"));
	l_oGridLayout->addWidget(l_sLabel, 7, 0);


	m_oArrowEdit = new QLineEdit(widget);
	l_oGridLayout->addWidget(m_oArrowEdit, 0, 1);

	m_oLeftArrowEdit = new QLineEdit(widget);
	l_oGridLayout->addWidget(m_oLeftArrowEdit, 1, 1);

	m_oRightArrowEdit = new QLineEdit(widget);
	l_oGridLayout->addWidget(m_oRightArrowEdit, 2, 1);

	m_oThickness = new QSpinBox(widget);
	m_oThickness->setMinimum(1);
	m_oThickness->setMaximum(3);
	l_oGridLayout->addWidget(m_oThickness, 3, 1);

	m_oType = new QComboBox(widget);
	m_oType->addItem(i18n("Zigzag"), 0);
	m_oType->addItem(i18n("Straight"), 0);
	l_oGridLayout->addWidget(m_oType, 4, 1);

	m_oStyle = new QComboBox(widget);
	m_oStyle->addItem(i18n("Invisible line"), (int) Qt::NoPen);
	m_oStyle->addItem(i18n("Solid Line"), (int) Qt::SolidLine);
	m_oStyle->addItem(i18n("Dash Line"),  (int) Qt::DashLine);
	m_oStyle->addItem(i18n("Dot Line"),   (int) Qt::DotLine);
	l_oGridLayout->addWidget(m_oStyle, 5, 1);

	m_oLeftArrow = new QComboBox(widget);
	m_oLeftArrow->addItem(i18n("No arrow"), 0);
	m_oLeftArrow->addItem(i18n("Triangle"), 0);
	m_oLeftArrow->addItem(i18n("Link"), 0);
	m_oLeftArrow->addItem(i18n("Inheritance"), 0);
	m_oLeftArrow->addItem(i18n("Aggregation"), 0);
	l_oGridLayout->addWidget(m_oLeftArrow, 6, 1);

	m_oRightArrow = new QComboBox(widget);
	m_oRightArrow->addItem(i18n("No arrow"), 0);
	m_oRightArrow->addItem(i18n("Triangle"), 0);
	m_oRightArrow->addItem(i18n("Link"), 0);
	m_oRightArrow->addItem(i18n("Inheritance"), 0);
	m_oRightArrow->addItem(i18n("Aggregation"), 0);
	l_oGridLayout->addWidget(m_oRightArrow, 7, 1);

	setMainWidget(widget);
	QSize size(350, 120);
	resize(size.expandedTo(minimumSizeHint()));

	connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
	connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	connect(m_oThickness, SIGNAL(valueChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oType, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oLeftArrow, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oRightArrow, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oLeftArrowEdit, SIGNAL(textChanged(const QString&)), this, SLOT(enable_apply(const QString&)));
	connect(m_oRightArrowEdit, SIGNAL(textChanged(const QString&)), this, SLOT(enable_apply(const QString&)));

	m_oArrowEdit->setFocus();
}

void box_link_properties::enable_apply(int) {
	enableButtonApply(true);
}

void box_link_properties::enable_apply(const QString &) {
	enableButtonApply(true);
}

void box_link_properties::apply() {
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	mem_edit_link *mem = new mem_edit_link(m_oLink->m_oView->m_oMediator, m_oLink->m_oView->m_iId);
	mem->link = m_oLink->m_oLink;

	mem->prev.copy_from(m_oLink->m_oInnerLink);
	mem->next.copy_from(m_oLink->m_oInnerLink);
	mem->next.border_width = m_oThickness->value();
	mem->next.m_iLineType = (data_link::LineType) m_oType->currentIndex();
	mem->next.pen_style = (Qt::PenStyle) m_oStyle->currentIndex();
	mem->next.m_iLeftArrow = (data_link::Arrow) m_oLeftArrow->currentIndex();
	mem->next.m_iRightArrow = (data_link::Arrow) m_oRightArrow->currentIndex();
	mem->next.m_sCaption = m_oArrowEdit->text();
	mem->next.m_sParentCaption = m_oLeftArrowEdit->text();
	mem->next.m_sChildCaption = m_oRightArrowEdit->text();

	mem->m_oPrevLinkState.copy_from(m_oLink->m_oView->m_oMediator->m_oLinkState);

	mem->apply();
	enableButtonApply(false);
}

