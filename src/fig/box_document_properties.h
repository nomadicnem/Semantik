// Thomas Nagy 2012 GPLV3

#ifndef BOX_DOCUMENT_PROPERTIES_H
#define BOX_DOCUMENT_PROPERTIES_H

#include <QFontDialog>
#include "box_view.h"

class box_document_properties : public QFontDialog
{
	Q_OBJECT
	public:
		box_document_properties(box_view*);
		box_view *m_oView;

	public slots:
		void apply();
};

#endif

