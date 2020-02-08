// Thomas Nagy 2007-2020 GPLV3

#include <QIcon>
#include <QtDebug>
#include <QTreeView>
#include <QGridLayout>
#include <KDE/KPushButton>
#include <QLineEdit>
#include <QSizePolicy>
#include <QApplication>

#include  "sem_mediator.h"
#include  "linear_container.h"
#include  "linear_view.h"
#include  "con.h"

linear_container::linear_container(QWidget *i_oParent, sem_mediator *i_oControl) : QWidget(i_oParent)
{
	m_oMediator = i_oControl;
	QGridLayout *l_oLayout = new QGridLayout();
	m_oView = new linear_view(this, i_oControl);

	int l_iSize = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);

	KPushButton *l_oButton = new KPushButton(this);
	l_oButton->setIcon(QIcon::fromTheme(notr("edit-clear-locationbar-ltr")));
	l_oButton->setIconSize(QSize(l_iSize, l_iSize));
	l_oButton->setFlat(true);
	l_oButton->setFocusPolicy(Qt::NoFocus);
	l_oButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	l_oButton->setMaximumSize(QSize(27, 27));
	l_oButton->resize(25, 25);
	l_oButton->setAutoDefault(false);
	QLineEdit *l_oEdit = new QLineEdit(this);

	connect(l_oButton, SIGNAL(clicked(bool)), l_oEdit, SLOT(clear()));
	connect(l_oEdit, SIGNAL(textChanged(const QString&)), m_oView, SLOT(filter_slot(const QString&)));

	l_oLayout->addWidget(l_oButton, 0, 0);
	l_oLayout->addWidget(l_oEdit, 0, 1);
	l_oLayout->addWidget(m_oView, 1, 0, 1, 2);

	setLayout(l_oLayout);
}

