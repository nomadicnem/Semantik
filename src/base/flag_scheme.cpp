// Thomas Nagy 2007-2018 GPLV3

#include <QIcon>
#include <QSvgRenderer>

#include <QtDebug>
#	include <QPainter>
 #include	"con.h"
#include	 "flag_scheme.h"	

flag_scheme::flag_scheme(QObject *i_oParent) : QObject(i_oParent)
{
	//m_sId = "key";
	m_sName = i18n("Flag item(s) as...");
	m_sIconPath = ":/images/erase.png";
	m_oRenderer = NULL;
}

// name : libellé du flag en question, le path représente le nom de fichier
flag_scheme::flag_scheme(QObject *i_oParent, QString i_sPath, QString i_sName) : QObject(i_oParent)
{
	m_oRenderer = NULL;
	m_sId = i_sPath;
	m_sName = i_sName;

	if (m_sName.length()>0)
		m_sIconPath = QString(SEMANTIK_DIR)+"/flags/"+i_sPath+notr(".svg");
	else
		m_sIconPath = QString(SEMANTIK_DIR)+"/images/"+i_sPath+notr(".svg");
}

flag_scheme::~flag_scheme()
{
	delete m_oRenderer;
}

QSvgRenderer* flag_scheme::_renderer()
{
	if (!m_oRenderer)
	{
		m_oRenderer = new QSvgRenderer(m_sIconPath, this);
		m_oRenderer->setFramesPerSecond(0);
	}
	return m_oRenderer;
}

const QIcon flag_scheme::_icon()
{
	return QIcon(m_sIconPath);
}

