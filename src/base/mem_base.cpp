// Thomas Nagy 2013-2018 GPLV3

#include <QPoint>
#include <QDebug>

#include "data_item.h"
#include "mem_command.h"
#include "sem_mediator.h"

#include "mem_base.h"

inline uint qHash(const QPoint &p) {
	return p.x() + 100000 * p.y();
}


mem_command::mem_command(sem_mediator* mod) {
	model = mod;
}

void mem_command::apply() {
	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();
	redo();
	model->m_oUndoStack.push(this);
	model->check_undo(true);
}

void mem_command::add() {
	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();
	model->m_oUndoStack.push(this);
	model->check_undo(true);
}

void mem_command::redo_dirty() {
	was_dirty = model->m_bDirty;
	model->set_dirty(true);
}

void mem_command::undo_dirty() {
	model->set_dirty(was_dirty);
}

///////////////////////////////////////////////////////////////////

mem_delete::mem_delete(sem_mediator* mod) : mem_command(mod) {

}

void mem_delete::init(QList<int> lst) {
	foreach (int id, lst) {
		items.append(*model + id);
		for (int i = 0; i < model->m_oLinks.size(); i++) {
			QPoint p = model->m_oLinks.at(i);
			if (p.x() == id || p.y() == id) {
				links.insert(p);
			}
		}
	}
}

void mem_delete::redo() {
	foreach (QPoint p, links) {
		Q_ASSERT(model->m_oLinks.contains(p));
		model->m_oLinks.removeAll(p);
		model->notify_unlink_items(p.x(), p.y());
	}
	foreach (const data_item& d, items) {
		Q_ASSERT(model->m_oItems.contains(d.m_iId));
		model->notify_delete_item(d.m_iId);
		model->m_oItems.remove(d.m_iId);
	}
	redo_dirty();
}

void mem_delete::undo() {
	foreach (const data_item& d, items) {
		Q_ASSERT(!model->m_oItems.contains(d.m_iId));
		model->m_oItems[d.m_iId] = d;
		model->notify_add_item(d.m_iId);
	}
	foreach (QPoint p, links) {
		Q_ASSERT(!model->m_oLinks.contains(p));
		model->m_oLinks.append(p);
		model->notify_link_items(p.x(), p.y());
	}
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_link::mem_link(sem_mediator* mod) : mem_command(mod) {

}

void mem_link::redo() {
	//qDebug()<<"redo mem_link"<<parent<<child;
	Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
	redo_dirty();
}

void mem_link::undo() {
	//qDebug()<<"undo mem_link"<<parent<<child;
	Q_ASSERT(model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.removeAll(QPoint(parent, child));
	model->notify_unlink_items(parent, child);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_ref::mem_ref(sem_mediator* mod) : mem_command(mod)
{
}

void mem_ref::redo()
{
	data_ref l_oRef(m_iParent, m_iChild);
	model->m_oRefs.append(l_oRef);
	model->notify_ref_items(m_iParent, m_iChild);
	redo_dirty();
}

void mem_ref::undo()
{
	int l_iPos = -1;
	for (int i = 0 ; i < model->m_oRefs.size() ; ++ i)
	{
		const data_ref& l_oRef = model->m_oRefs.at(i);
		if (l_oRef.m_iParent == m_iParent && l_oRef.m_iChild == m_iChild)
		{
			l_iPos = i;
			break;
		}
	}
	Q_ASSERT(l_iPos != -1);
	model->m_oRefs.removeAt(l_iPos);
	model->notify_unref_items(m_iParent, m_iChild);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////


mem_unref::mem_unref(sem_mediator* mod) : mem_command(mod)
{
	m_iPos = 0;
}

void mem_unref::redo()
{
	model->m_oRefs.removeAt(m_iPos);
	model->notify_unref_items(m_iParent, m_iChild);
	redo_dirty();
}

void mem_unref::undo()
{
	data_ref l_oRef(m_iParent, m_iChild);
	model->m_oRefs.insert(m_iPos, l_oRef);
	model->notify_ref_items(m_iParent, m_iChild);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_sel::mem_sel(sem_mediator* mod) : mem_command(mod) {
	m_iSortSel = NO_ITEM;
	m_iSortUnsel = NO_ITEM;
	foreach (const data_item& t, model->m_oItems.values()) {
		if (t.m_bSelected) {
			unsel.append(t.m_iId);
		}
	}
	if (unsel.size() == 1)
	{
		if (model->m_iSortId != NO_ITEM)
		{
			m_iSortUnsel = model->m_iSortId;
		}
	}
}

void mem_sel::apply() {
	//qDebug()<<"apply sel begin"<<model->m_oUndoStack.size()<<model->m_oRedoStack.size();

	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();

	// TODO: merge a previous selection if possible?
	/*if (m_iSortSel == NO_ITEM && m_iSortUnsel == NO_ITEM)
	{
		while (!model->m_oUndoStack.empty()) {
			mem_command *me = model->m_oUndoStack.pop();
			if (me->type() != SELECT)
			{
				model->m_oUndoStack.push(me);
				break;
			}

			mem_sel *sal = (mem_sel*) me;
			if (sal->m_iSortSel != NO_ITEM or sal->m_iSortUnsel != NO_ITEM)
			{
				model->m_oUndoStack.push(me);
				break;
			}

			foreach (int i, sal->sel) {
				if (!sel.contains(i) && !unsel.contains(i))
					sel.append(i);
			}
			foreach (int i, sal->unsel) {
				if (!sel.contains(i) && !unsel.contains(i))
					unsel.append(i);
			}
			delete sal;
		}
	}*/

	foreach (int k, sel) {
		unsel.removeAll(k);
	}

	// normal processing
	redo();
	model->m_oUndoStack.push(this);
	//qDebug()<<"apply sel end"<<model->m_oUndoStack.size()<<model->m_oRedoStack.size();
	model->check_undo(true);
}

void mem_sel::redo() {
	//qDebug()<<"redo mem_sel"<<sel<<unsel;
	foreach (int k, unsel) {
		model->m_oItems[k].m_bSelected = false;
	}
	foreach (int k, sel) {
		model->m_oItems[k].m_bSelected = true;
	}
	if (m_iSortUnsel != NO_ITEM)
	{
		model->m_iSortId = NO_ITEM;
		model->notify_sort(m_iSortUnsel, false);
		model->m_iSortCursor = 0;
		model->notify_message("", 0);
	}
	model->notify_select(unsel, sel);
	if (m_iSortSel != NO_ITEM)
	{
		model->notify_sort(m_iSortSel, true);
		model->m_iSortId = m_iSortSel;
		model->m_iSortCursor = 0;
		model->notify_message("", 0);
	}
}

void mem_sel::undo() {
	//qDebug()<<"undo mem_sel"<<sel<<unsel;
	foreach (int k, sel) {
		model->m_oItems[k].m_bSelected = false;
	}
	foreach (int k, unsel) {
		model->m_oItems[k].m_bSelected = true;
	}
	if (m_iSortSel != NO_ITEM)
	{
		model->notify_sort(m_iSortSel, false);
		model->m_iSortId = NO_ITEM;
		model->m_iSortCursor = 0;
		model->notify_message("", 0);
	}
	model->notify_select(sel, unsel);
	if (m_iSortUnsel != NO_ITEM)
	{
		model->notify_sort(m_iSortUnsel, true);
		model->m_iSortId = m_iSortUnsel;
		model->m_iSortCursor = 0;
		model->notify_message("", 0);
	}
}

///////////////////////////////////////////////////////////////////

mem_add::mem_add(sem_mediator* mod) : mem_command(mod),
item(model->next_seq())
{
	parent = NO_ITEM;
	sel = new mem_sel(model);
}

void mem_add::redo()
{
	//qDebug()<<"redo mem_add"<<item.m_iId;
	Q_ASSERT(!model->m_oItems.contains(item.m_iId));
	model->m_oItems.insert(item.m_iId, item);
	model->notify_add_item(item.m_iId);
	if (parent) {
		Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, item.m_iId)));
		model->m_oLinks.append(QPoint(parent, item.m_iId));
		model->notify_link_items(parent, item.m_iId);
	}

	if (sel->sel.size() != 1) {
		sel->sel.append(item.m_iId);
	}
	sel->redo();
	redo_dirty();
}

void mem_add::undo() {
	//qDebug()<<"undo mem_add"<<item.m_iId;
	sel->undo();

	if (parent) {
		Q_ASSERT(model->m_oLinks.contains(QPoint(parent, item.m_iId)));
		model->m_oLinks.removeAll(QPoint(parent, item.m_iId));
		model->notify_unlink_items(parent, item.m_iId);
	}
	Q_ASSERT(model->m_oItems.contains(item.m_iId));
	model->notify_delete_item(item.m_iId);
	model->m_oItems.remove(item.m_iId);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_doc_open::mem_doc_open(sem_mediator*i_oDoc) : mem_command(i_oDoc)
{
		m_oDelete = new mem_delete(i_oDoc);
		m_oDelete->init(i_oDoc->m_oItems.keys());
		m_oSel = new mem_sel(i_oDoc);
}

void mem_doc_open::init_data(sem_mediator* i_oOld, sem_mediator* i_oNew)
{
	m_sOutDirNew = i_oNew->m_sOutDir;
	m_sOutProjectNew = i_oNew->m_sOutProject;
	m_sOutTemplateNew = i_oNew->m_sOutTemplate;
	m_bExportIsWidthNew = i_oNew->m_bExportIsWidth;
	m_iExportWidthNew = i_oNew->m_iExportWidth;
	m_iExportHeightNew = i_oNew->m_iExportHeight;
	m_sExportUrlNew = i_oNew->m_sExportUrl;
	m_oColorNew = i_oNew->m_oColor;
	m_sSpellingLanguageNew = i_oNew->m_sSpellingLanguage;
	m_oCurrentUrlNew = i_oNew->m_oCurrentUrl;
	m_oFontNew = i_oNew->m_oFont;
	m_sHintsNew = i_oNew->m_sHints;
	m_oColorSchemesNew = i_oNew->m_oColorSchemes;
	m_oFlagSchemesNew = i_oNew->m_oFlagSchemes;
	m_oItemsNew = i_oNew->m_oItems;
	m_oLinksNew = i_oNew->m_oLinks;
	m_oRefsNew = i_oNew->m_oRefs;
	m_bShowPicsNew = i_oNew->m_bShowPics;

	m_sOutDirOld = i_oOld->m_sOutDir;
	m_sOutProjectOld = i_oOld->m_sOutProject;
	m_sOutTemplateOld = i_oOld->m_sOutTemplate;
	m_bExportIsWidthOld = i_oOld->m_bExportIsWidth;
	m_iExportWidthOld = i_oOld->m_iExportWidth;
	m_iExportHeightOld = i_oOld->m_iExportHeight;
	m_sExportUrlOld = i_oOld->m_sExportUrl;
	m_oColorOld = i_oOld->m_oColor;
	m_sSpellingLanguageOld = i_oOld->m_sSpellingLanguage;
	m_oCurrentUrlOld = i_oOld->m_oCurrentUrl;
	m_oFontOld = i_oOld->m_oFont;
	m_sHintsOld = i_oOld->m_sHints;
	m_oColorSchemesOld = i_oOld->m_oColorSchemes;
	m_oFlagSchemesOld = i_oOld->m_oFlagSchemes;
	m_oItemsOld = i_oOld->m_oItems;
	m_oLinksOld = i_oOld->m_oLinks;
	m_oRefsOld = i_oOld->m_oRefs;
	m_sLastSavedOld = i_oOld->m_sLastSaved;
	m_bShowPicsOld = i_oOld->m_bShowPics;
}

void mem_doc_open::redo()
{
	model->stop_timer();
	m_oSel->redo();
	m_oDelete->redo();

	model->m_sOutDir = m_sOutDirNew;
	model->m_sOutProject = m_sOutProjectNew;
	model->m_sOutTemplate = m_sOutTemplateNew;
	model->m_bExportIsWidth = m_bExportIsWidthNew;
	model->m_iExportWidth = m_iExportWidthNew;
	model->m_iExportHeight = m_iExportHeightNew;
	model->m_sExportUrl = m_sExportUrlNew;
	model->m_oColor = m_oColorNew;
	model->m_sSpellingLanguage = m_sSpellingLanguageNew;
	model->m_oCurrentUrl = m_oCurrentUrlNew;
	model->m_oFont = m_oFontNew;
	model->m_sHints = m_sHintsNew;
	model->m_oColorSchemes = m_oColorSchemesNew;
	model->m_oFlagSchemes = m_oFlagSchemesNew;
	model->m_oItems = m_oItemsNew;
	model->m_oLinks = m_oLinksNew;
	model->m_oRefs = m_oRefsNew;
	model->m_sLastSaved = m_sLastSavedNew;
	model->m_bShowPics = m_bShowPicsNew;

	model->notify_colors();
	model->notify_flags();
	model->notify_font();

	foreach (int i, model->m_oItems.keys())
	{
		model->notify_add_item(i);
	}

	foreach (QPoint p, model->m_oLinks)
	{
		model->notify_link_items(p.x(), p.y());
	}

	foreach (const data_ref&l_oRef, model->m_oRefs)
	{
		model->notify_ref_items(l_oRef.m_iParent, l_oRef.m_iChild);
	}
	model->notify_open_map();
	model->set_dirty(false);
	model->init_timer();
	model->notify_background_color();
}

void mem_doc_open::undo()
{
	model->stop_timer();
	foreach (const data_ref&l_oRef, model->m_oRefs)
	{
		model->notify_unref_items(l_oRef.m_iParent, l_oRef.m_iChild);
	}
	foreach (QPoint p, model->m_oLinks)
	{
		model->notify_unlink_items(p.x(), p.y());
	}

	foreach (int i, model->m_oItems.keys())
	{
		model->notify_delete_item(i);
	}

	model->m_sOutDir = m_sOutDirOld;
	model->m_sOutProject = m_sOutProjectOld;
	model->m_sOutTemplate = m_sOutTemplateOld;
	model->m_bExportIsWidth = m_bExportIsWidthOld;
	model->m_iExportWidth = m_iExportWidthOld;
	model->m_iExportHeight = m_iExportHeightOld;
	model->m_sExportUrl = m_sExportUrlOld;
	model->m_oColor = m_oColorOld;
	model->m_sSpellingLanguage = m_sSpellingLanguageOld;
	model->m_oCurrentUrl = m_oCurrentUrlOld;
	model->m_oFont = m_oFontOld;
	model->m_sHints = m_sHintsOld;
	model->m_oColorSchemes = m_oColorSchemesOld;
	model->m_oFlagSchemes = m_oFlagSchemesOld;
	model->m_oItems.clear();
	model->m_oLinks.clear();
	model->m_oRefs.clear();
	model->m_sLastSaved = m_sLastSavedOld;
	model->m_bShowPics = m_bShowPicsOld;

	model->notify_colors();
	model->notify_background_color();
	model->notify_flags();
	model->notify_font();

	m_oDelete->undo();
	m_oSel->undo();
	model->init_timer();
}
