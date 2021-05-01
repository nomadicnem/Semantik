// Thomas Nagy 2017-2021 GPLV3

#ifndef EXPORT_FIG_DIALOG_H
#define EXPORT_FIG_DIALOG_H

#include <KDE/KDialog>

class QSpinBox;
class QRadioButton;
class KUrlRequester;
class export_fig_dialog : public KDialog
{
	Q_OBJECT
	public:
		export_fig_dialog(QWidget *i_oParent);

		KUrlRequester *kurlrequester;
		QRadioButton *m_oWidthC;
		QSpinBox *m_oWidth;
		QRadioButton *m_oHeightC;
		QSpinBox *m_oHeight;

		int m_iBaseWidth;
		int m_iBaseHeight;

	public slots:
		void slotButtonClicked(int button);
		void radio(bool);
		void widthChanged(int);
		void heightChanged(int);
};

#endif

