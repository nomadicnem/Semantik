// Thomas Nagy 2007-2017 GPLV3

#include <fstream>
#include <QCoreApplication>
#include <QtGui>
#include <QTranslator>
#include <QCommandLineParser>
#include "con.h"
#include "semantik.h"
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
	K4AboutData l_o("semantik", 0, ki18n("Semantik"), version, ki18n(description),
			K4AboutData::License_GPL_V3, ki18n("(C) 2007-2017 Thomas Nagy"), KLocalizedString());
	l_o.setBugAddress("https://github.com/ita1024/semantik/issues");
	l_o.addAuthor(ki18n("Thomas Nagy"), KLocalizedString());

	KCmdLineArgs::init(i_iArgc, i_iArgv, &l_o);
	KCmdLineArgs::addCmdLineOptions(options);

	QCoreApplication::setAttribute(Qt::AA_Use96Dpi, true);
	KApplication l_oApp;

	semantik_win *l_oMainWin = new semantik_win;

	const KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (!args->getOption("o").isEmpty()) {
		if (args->count()) {
			if (args->url(0).isEmpty()) {
				qDebug()<<"a file requires a url";
				return 1;
			} else {
				QUrl l_oUrl(args->getOption("o"));
				if (!l_oUrl.isValid()) return 2;

				l_oMainWin->slot_recent(args->url(0));
				QPair<int, int> p;
				return l_oMainWin->print_to_file(l_oUrl, p);
			}
		}
	} else {
		l_oMainWin->show();
		if (args->count() && !args->url(0).isEmpty()) {
			l_oMainWin->slot_recent(args->url(0));
		}
	}
	return l_oApp.exec();
}

