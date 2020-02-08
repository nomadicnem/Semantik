// Thomas Nagy 2013-2020 GPLV3

# include <filetree.h>
#include <QFileSystemModel>
#include <QStringList>
#include <QSortFilterProxyModel>
#include <KDirModel>
#include <KDirLister>

fileproxy::fileproxy(QObject* i_oParent) : QSortFilterProxyModel(i_oParent)
{
	setDynamicSortFilter(true);
	setSortCaseSensitivity(Qt::CaseSensitive);
	sort(KDirModel::Name, Qt::AscendingOrder);
}

bool fileproxy::filterAcceptsRow(int i_iRow, const QModelIndex& i_oParent) const
{
	KDirModel* l_o = static_cast<KDirModel*>(sourceModel());

	const QModelIndex l_oIndex = sourceModel()->index(i_iRow, 1, i_oParent);
	const KFileItem l_oFileItem = l_o->itemForIndex(l_oIndex);
	if (l_oFileItem.isDir() || l_oFileItem.name().endsWith(".semd"))
	{
		return true;
	}
	return false;
}

bool fileproxy::lessThan(const QModelIndex& i_oLeft, const QModelIndex& i_oRight) const
{
	KDirModel* l_o = static_cast<KDirModel*>(sourceModel());

	const KFileItem i_oLeftFileItem = l_o->itemForIndex(i_oLeft);
	const KFileItem i_oRightFileItem = l_o->itemForIndex(i_oRight);

	if (i_oLeftFileItem.isDir() and !i_oRightFileItem.isDir()) {
		return true;
	} else if (!i_oLeftFileItem.isDir() and i_oRightFileItem.isDir()) {
		return false;
	}

	bool ret = QString::localeAwareCompare(i_oLeftFileItem.name(), i_oRightFileItem.name()) < 0;
	return ret;
}

int fileproxy::columnCount(const QModelIndex&) const
{
	return 1;
}

filetree::filetree(QWidget *i_oWidget) : QTreeView(i_oWidget)
{
	setHeaderHidden(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_oModel = new KDirModel(this);
	m_oProxy = new fileproxy(this);
	m_oProxy->setSourceModel(m_oModel);
	m_oProxy->setDynamicSortFilter(true);
	setModel(m_oProxy);

	m_oModel->dirLister()->openUrl(QUrl("file:///"), KDirLister::Keep);

	setSortingEnabled(true);
	sortByColumn(0, Qt::AscendingOrder);

	connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(reclick(const QModelIndex&)));
	connect(m_oModel, SIGNAL(expand(const QModelIndex&)), this, SLOT(slot_expand(const QModelIndex&)));
}

void filetree::slot_expand(const QModelIndex& i_oIndex)
{
	QModelIndex l_o = m_oProxy->mapFromSource(i_oIndex);
	expand(l_o);
	selectionModel()->select(l_o, QItemSelectionModel::ClearAndSelect);
}

void filetree::reclick(const QModelIndex& i_oModelIndex)
{
	const KFileItem l_oFileItem = m_oModel->itemForIndex(m_oProxy->mapToSource(i_oModelIndex));
	if (!l_oFileItem.isDir())
	{
		emit url_selected(l_oFileItem.url());
	}
}

Qt::DropActions filetree::supportedDragActions() const {
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction;
}

