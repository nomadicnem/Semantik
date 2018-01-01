// Thomas Nagy 2007-2018 GPLV3

#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include "mem_box.h"
#include "box_view.h"
//#include "kiconloader.h"

#include <QtDebug>

#include "sem_mediator.h"
#include "box_document_properties.h"

box_document_properties::box_document_properties(box_view *i_oParent):
	QFontDialog(i_oParent)
{
	m_oView = i_oParent;

	setWindowTitle(trUtf8("Font properties"));

	data_item *l_oItem = m_oView->m_oMediator->m_oItems.value(m_oView->m_iId);
	setCurrentFont(l_oItem->m_oDiagramFont);

	connect(this, SIGNAL(accepted()), this, SLOT(apply()));

	QSize size(650, 620);
	resize(size.expandedTo(minimumSizeHint()));
}

void box_document_properties::apply()
{
	data_item *l_oItem = m_oView->m_oMediator->m_oItems.value(m_oView->m_iId);
	mem_diagram_properties *mem = new mem_diagram_properties(m_oView->m_oMediator, m_oView->m_iId);
	mem->m_oOldFont = l_oItem->m_oDiagramFont;
	mem->m_oNewFont = currentFont();
	mem->apply();

	/*
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}
	enableButtonApply(false);*/
}

