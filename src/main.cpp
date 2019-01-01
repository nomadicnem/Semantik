// Thomas Nagy 2007-2019 GPLV3

#include <fstream>
#include <QCoreApplication>
#include <QtGui>
#include <QTranslator>
#include <QCommandLineParser>
#include "con.h"
#include "semantik.h"
 #include "sem_mediator.h"
	#include "sembind.h"
#include <stdlib.h>
#include <iostream>

#include <KDE/KCmdLineOptions>
#include <KDE/K4AboutData>
#include <KDE/KApplication>
#include <KAboutData>

static const char description[] = I18N_NOOP("A mind-mapping tool for KDE\nAvailable exclusively for open systems such as OpenSuse, Gentoo or Fedora.");
static const char version[] = VERSION;

int grid_int(int x) {
	return GRID * (x / GRID);
}

int main(int i_iArgc, char **i_iArgv)
{
	QCommandLineParser l_oParser;

	KCmdLineOptions options;
	options.add("+[url]", ki18n("A file to open on startup"));
	options.add("o <file>", ki18n("Batch export as svg/pdf/png"));
	options.add("d <folder>", ki18n("Batch document creation into the given folder (absolute path)"));
	options.add("t <type>", ki18n("Batch document creation template pdflatex/beamer/html/s5/odp/odt"));
	K4AboutData l_o("semantik", 0, ki18n("Semantik"), version, ki18n(description),
			K4AboutData::License_GPL_V3, notr("© 2007-2019 Thomas Nagy"), KLocalizedString());
	l_o.setBugAddress("https://gitlab.com/ita1024/semantik/issues");
	l_o.addAuthor(notr("Thomas Nagy"), KLocalizedString());

	KCmdLineArgs::init(i_iArgc, i_iArgv, &l_o);
	KCmdLineArgs::addCmdLineOptions(options);

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
	KLocalizedString::setApplicationDomain("semantik");
	KApplication l_oApp;
	semantik_win *l_oMainWin = new semantik_win;

	const KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (!args->getOption("o").isEmpty())
	{
		if (args->count()) {
			if (args->url(0).isEmpty()) {
				qDebug()<<"Image export requires an input document (url)";
				return 1;
			} else {
				QString l_o = args->getOption("o");
				if (!QDir::isAbsolutePath(l_o))
				{
					l_o = QDir::currentPath() + "/" + l_o;
				}
				l_oMainWin->slot_recent(args->url(0));
				QPair<int, int> p;
				return l_oMainWin->print_to_file(l_o, p);
			}
		}
	}
	else if (!args->getOption("d").isEmpty() && !args->getOption("t").isEmpty())
	{
		QUrl l_oInput = args->url(0);
		if (!l_oInput.isValid())
		{
			qDebug()<<"Batch document export requires an input document (url)";
			return 1;
		}
		l_oMainWin->slot_recent(args->url(0));

		QString l_oTemplate = QString("%1/%2.sem.py").arg(bind_node::get_var(notr("template_dir")), args->getOption("t"));
		if (!QFile::exists(l_oTemplate))
		{
			qDebug()<<"Document export requires a readable input template"<<l_oTemplate;
			return 2;
		}

		QDir l_oDir = QDir(args->getOption("d"));
		if (!l_oDir.isAbsolute())
		{
			qDebug()<<"Document export requires an absolute path for the target folder";
			return 3;
		}

		QString l_oTargetName = l_oDir.dirName();
		l_oDir.cdUp();
		QString l_oBaseFolder = l_oDir.absolutePath();

		// "/home/user/" "tc_infra" "Beamer presentation"
		// "/usr/share/semantik/templates/beamer.sem.py" "tc_infra" "/home/user/"

                /*l_oMainWin->m_oMediator->m_sOutDir = l_oGen.m_oLocation->text();
                l_oMainWin->m_oMediator->m_sOutProject = l_oGen.m_oName->text();
                --l_oMainWin->m_oMediator->m_sOutTemplate = l_oGen.m_sShortName;*/

		l_oMainWin->m_oMediator->m_sOutDir = l_oBaseFolder;
		l_oMainWin->m_oMediator->m_sOutProject = l_oTargetName;
		return l_oMainWin->m_oMediator->generate_docs(l_oTemplate, l_oTargetName, l_oBaseFolder);
	}
	else
	{
		l_oMainWin->show();
		if (args->count() && !args->url(0).isEmpty()) {
			l_oMainWin->slot_recent(args->url(0));
		}
	}
	return l_oApp.exec();
}

