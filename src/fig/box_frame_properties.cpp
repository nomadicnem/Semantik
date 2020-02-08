// Thomas Nagy 2007-2020 GPLV3

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

#include "box_frame_properties.h"


box_frame_properties::box_frame_properties(QWidget *i_oParent, box_frame *i_oFrame):
	KDialog(i_oParent)
{
	QWidget *l_oWidget = new QWidget(this);
	setCaption(i18n("Boundary properties"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);
	m_oFrame = i_oFrame;

	QGridLayout *l_oGridLayout = new QGridLayout(l_oWidget);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(l_oWidget);
	l_sLabel->setText(i18n("Text position"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0);

	l_sLabel = new QLabel(l_oWidget);
	l_sLabel->setText(i18n("Text"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0);

	m_oEdit = new QLineEdit(l_oWidget);
	m_oEdit->setText(i_oFrame->m_oBox->m_sText);
	l_oGridLayout->addWidget(m_oEdit, 0, 1);

	m_oLabelPosition = new QComboBox(l_oWidget);
	m_oLabelPosition->addItem(i18n("Up"), Qt::TopEdge);
	m_oLabelPosition->addItem(i18n("Down"), Qt::BottomEdge);
	m_oLabelPosition->setCurrentIndex(i_oFrame->m_oBox->m_iLabelPosition == Qt::TopEdge ? 0 : 1);
	l_oGridLayout->addWidget(m_oLabelPosition, 1, 1);

	l_oWidget->setTabOrder(m_oEdit, m_oLabelPosition);

	setMainWidget(l_oWidget);
	QSize size(350, 120);
	resize(size.expandedTo(minimumSizeHint()));

	connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
	connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	connect(m_oLabelPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oEdit, SIGNAL(textChanged(const QString&)), this, SLOT(enable_apply(const QString&)));

	m_oEdit->setFocus();
}

void box_frame_properties::enable_apply(int) {
	enableButtonApply(true);
}

void box_frame_properties::enable_apply(const QString &) {
	enableButtonApply(true);
}

void box_frame_properties::apply() {
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	mem_edit_box *mem = new mem_edit_box(m_oFrame->m_oView->m_oMediator, m_oFrame->m_oView->m_iId, m_oFrame->m_iId);
	mem->newText = m_oEdit->text();

	QTextDocument doc;
	QTextOption l_oOption = doc.defaultTextOption();
	l_oOption.setAlignment(m_oFrame->m_oBox->m_iAlign);
	doc.setDefaultTextOption(l_oOption);
	doc.setPlainText(mem->newText);

	doc.setTextWidth(m_oFrame->m_oBox->m_iWW - 2 * OFF);
	mem->newHeight = fceil(doc.size().height() + 2 * OFF, GRID);
	if (mem->newHeight < m_oFrame->m_oBox->m_iHH)
		mem->newHeight = m_oFrame->m_oBox->m_iHH;

	mem->m_iOldLabelPosition = m_oFrame->m_oBox->m_iLabelPosition;
	mem->m_iNewLabelPosition = (Qt::Edge) m_oLabelPosition->currentData().toInt();

	mem->apply();
	enableButtonApply(false);
}

