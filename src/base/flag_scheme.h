// Thomas Nagy 2007-2021 GPLV3

#ifndef FLAG_SCHEME_H
#define FLAG_SCHEME_H

//#include <QIcon>
class QIcon;
//#include <QSvgRenderer>
class QSvgRenderer;
#include <QString>
#include <con.h>

class flag_scheme
{
	public:
		flag_scheme();
		~flag_scheme();
		flag_scheme(QString i_sPath, QString i_sName);

		QString m_sId;
		QString m_sName;
		QString m_sIconPath;
		QSvgRenderer *m_oRenderer;

		QSvgRenderer* _renderer();
		flag_scheme& operator=(const flag_scheme &);
		const QIcon _icon();
};

#endif

