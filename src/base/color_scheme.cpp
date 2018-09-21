// Thomas Nagy 2007-2018 GPLV3

#include "color_scheme.h"

color_scheme::color_scheme()
{
	m_sName = QString("Color");
	m_oBorderColor = QColor("#000000");
	m_oInnerColor = QColor("#ffffff");
	m_oTextColor = QColor("#000000");
}

color_scheme& color_scheme::operator=(const color_scheme &i_o)
{
	m_sName = i_o.m_sName;
	m_oBorderColor = i_o.m_oBorderColor;
	m_oInnerColor = i_o.m_oInnerColor;
	m_oTextColor = i_o.m_oTextColor;
	return *this;
}
