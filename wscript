#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2016 (ita)

APPNAME = 'semantik'
VERSION = '1.0.0'

top = '.'

import os, sys, re, time
from waflib import Options, Logs, Configure, Errors, Utils, TaskGen, Task

default_prefix = '/usr'

def build(bld):
	bld.targets = '*' # build all targets by default
	lst = 'src/fig/semd.cpp src/fig/semantik_d_win.cpp src/fig/diagram_document.cpp src/fig/filetree.cpp'

	tg = bld(
		features = 'cxx qt5 cxxshlib',
		source = bld.path.ant_glob('src/fig/*.cpp src/base/*.cpp', excl=lst),
		target = 'semantik',
		moc = '''src/base/flag_scheme.h src/base/sem_mediator.h
src/fig/box_class_properties.h
src/fig/box_link_properties.h
src/fig/export_fig_dialog.h
src/fig/box_view.h
src/fig/matrix_dialog.h
src/fig/box_document_properties.h
''',
		name = 'semlib',
		vnum = VERSION,
		use = 'KDECORE QT5CORE QT5WIDGETS QT5GUI QT5WEBENGINEWIDGETS QT5XML QT5PRINTSUPPORT QT5SVG PYEMBED',
		includes='. src src/fig src/base')

	bld(features='cxx qt5 cxxprogram pyembed',
		source = bld.path.ant_glob('src/*.cpp'),
		moc = '''
src/browser.h
src/canvas_view.h
src/config_dialog.h
src/data_view.h
src/doc_dialog.h
src/export_map_dialog.h
src/generator_dialog.h
src/image_view.h
src/linear_view.h
src/pre_view.h
src/semantik.h
src/table_dialog.h
src/table_view.h
src/text_view.h
src/vars_view.h
''',
		use = 'KDECORE QT5CORE QT5WIDGETS QT5GUI QT5WEBENGINEWIDGETS QT5XML QT5PRINTSUPPORT QT5SVG semlib',
		target = 'src/semantik',
		includes = '. src src/fig src/base')

	bld(features='cxx qt5 cxxprogram pyembed',
		source = lst,
		moc = '''
src/fig/semantik_d_win.h
src/fig/filetree.h
src/fig/diagram_document.h
''',
		use = 'KDECORE QT5CORE QT5WIDGETS QT5GUI QT5XML QT5PRINTSUPPORT QT5SVG semlib',
		target = 'src/semantik-d',
		includes = '. src src/fig src/base')

	bld.install_files('${SEMANTIK_DIR}', 'src/sembind.py')


	rt = 'src/templates/'

	glob = bld.path.ant_glob

	bld.install_files('${SEMANTIK_DIR}/flags', glob('src/flags/*.svg'))
	bld.install_files('${TEMPLATE_DIR}', glob(rt+'*.py'))

	bld.install_as('${TEMPLATE_DIR}/waf', 'waf', chmod=Utils.O755)

	bld.install_files('${TEMPLATE_DIR}/pdflatex', glob(rt+'pdflatex/*'))
	for x in 'html odt odp java'.split():
		bld.install_files('${TEMPLATE_DIR}/' + x, glob(rt+'%s/*' % x))

	for x in "color font inner outer theme".split():
		k = 'beamer/beamermindist/themes/'
		bld.install_files('${TEMPLATE_DIR}/' + k+x, bld.path.ant_glob(rt+k+x+'/*'))

	obj = bld(features='msgfmt', appname = 'semantik', langs=[x.path_from(bld.path).replace('.po', '') for x in bld.path.ant_glob('src/po/*.po')])
	for x in bld.path.ant_glob('src/po/*.po'):
		bld.symlink_as('${LOCALEDIR}/%s/LC_MESSAGES/semantik-d.mo' % x.name.strip('.po'), 'semantik.mo')

	bld.install_files('${TEMPLATE_DIR}/beamer/beamermindist/art/', glob(rt+'beamer/beamermindist/art/*'))
	bld.install_files('${TEMPLATE_DIR}/beamer/beamermindist/', glob(rt+'beamer/beamermindist/*'))
	bld.install_files('${TEMPLATE_DIR}/beamer/', rt+'beamer/main.tex')
	bld.install_files('${TEMPLATE_DIR}/beamer/', rt+'beamer/wscript')

	bld.install_files('${TEMPLATE_DIR}/s5', rt+'s5/index.html')
	bld.install_files('${TEMPLATE_DIR}/s5/ui/default/', glob(rt+'s5/ui/default/*'))
	bld.install_files('${SEMANTIK_DIR}/images', glob('src/images/*.svg'))
	bld.install_files('${FILTER_DIR}/', glob('src/filters/*'))

	bld.install_files('${DESKTOPDIR}/', 'src/data/semantik.desktop src/data/semantik-d.desktop')
	bld.install_files('${MIME_DIR}/', 'src/data/semantik.xml')

	for x in ('', '-d'):
		bld.install_as('${ICONDIR}/oxygen/128x128/apps/semantik%s.png' % x, 'src/data/hi128-app-semantik%s.png'%x)
		bld.install_as('${ICONDIR}/oxygen/64x64/apps/semantik%s.png'% x, 'src/data/hi64-app-semantik%s.png'%x)
		bld.install_as('${ICONDIR}/oxygen/48x48/apps/semantik%s.png'% x, 'src/data/hi48-app-semantik%s.png'%x)
		bld.install_as('${ICONDIR}/oxygen/32x32/apps/semantik%s.png'% x, 'src/data/hi32-app-semantik%s.png'%x)
		bld.install_as('${ICONDIR}/oxygen/22x22/apps/semantik%s.png'% x, 'src/data/hi22-app-semantik%s.png'%x)

		#bld(rule="${GZIP} -c ${SRC} > ${TGT}", source='src/data/semantik%s.svg'%x, target='src/data/semantik%s.svgz'%x)
		bld.install_as('${ICONDIR}/oxygen/scalable/apps/semantik%s.svg'%x, 'src/data/semantik%s.svg'%x)


	bld.install_files('${XMLGUIDIR}/semantik', 'src/data/semantikui.rc')
	bld.install_files('${XMLGUIDIR}/semantik-d', 'src/data/semantik-dui.rc')
	bld.install_files('${DATAROOTDIR}/semantik', 'src/data/tips')

	bld.add_post_fun(post_build)
	# update-mime-database /usr/share/mime ?

def configure(conf):
	conf.find_program('msgfmt')
	def test(system):
		return (sys.platform.lower().rfind(system) > -1)

	conf.find_program('gzip')

	err = "Semantik cannot work on %s, please install a Linux system from http://www.opensuse.org"
	if (test('linux')):
		Logs.pprint('GREEN', "You are using Linux, that's good (tm)")
	elif (test('bsd')):
		Logs.pprint('GREEN', "You are using a BSD system, that's good (tm)")
	elif (test('win32') or test('cygwin')):
		conf.fatal(err % "win32")
	elif (test('darwin')):
		conf.fatal(err % "osx")
	elif (test('solaris') or test('sunos')):
		conf.fatal(err % "Sun systems")

	icons = '/usr/share/icons:/usr:/etc/opt/kde3/share/icons/:/opt/kde3/share/icons/:/var/lib/mandriva/kde-profiles/powerpackplus/share/icons/:/usr/share/icons/'
	if Options.options.icons:
		icons = Options.options.icons

	conf.load('compiler_c compiler_cxx qt5')

	conf.check_cxx(fragment='int main() { return 0; }', cxxflags='-std=c++11', uselib_store='cxx')
	if not conf.env.CXX: conf.fatal('Semantik requires g++ (compilation only)')
	if not conf.env.QT_LRELEASE: conf.fatal('Semantik requires the program lrelease (from the Qt linguist package? - compilation only)')
	conf.find_program('python3', var='PYTHON')
	conf.load('python')
	if not conf.env.LIB_QT5SVG: conf.fatal('Could not find Qt5Svg - Semantik requires Qt >= 5')
	if not conf.env.LIB_QT5WEBENGINEWIDGETS: conf.fatal('Could not find Qt5WebEngineWidgets - check `pkg-config --libs Qt5WebEngineWidgets`')
	if not conf.env.PYTHON: conf.fatal('Semantik requires Python >= 3.2 (development package for the compilation)')
	conf.check_python_version((3,2,0))
	conf.check_python_headers()
	#ret = ctx.cmd_output('python-config --cflags').strip().split()

	conf.env.DATAROOTDIR = os.path.join(conf.env.PREFIX, 'share')
	conf.env.DESKTOPDIR = os.path.join(conf.env.DATAROOTDIR, 'applications')
	conf.env.XMLGUIDIR = os.path.join(conf.env.DATAROOTDIR, 'kxmlgui5')
	conf.env.ICONDIR = os.path.join(conf.env.DATAROOTDIR, 'icons')
	conf.env.PICDIR = conf.env.DATAROOTDIR + '/share/pixmaps/'
	conf.env.LOCALEDIR = os.path.join(conf.env.DATAROOTDIR, 'locale/')

	conf.env.DEFINES = 'WAF'
	conf.define('ICONS', icons)
	conf.define('VERSION', VERSION)

	#kdeconfig = conf.find_program('kde4-config')
	prefix = Options.options.prefix # conf.cmd_and_log(Utils.to_list(kdeconfig) + ['--prefix']).strip()
	conf.env.SEMANTIK_DIR = conf.env.DATAROOTDIR + '/semantik'
	conf.env.MIME_DIR = prefix + '/share/mime/packages'
	conf.define('SEMANTIK_DIR', conf.env.SEMANTIK_DIR)

	conf.env.TEMPLATE_DIR = conf.env.DATAROOTDIR + '/semantik/templates/'
	conf.define('TEMPLATE_DIR', conf.env.TEMPLATE_DIR)
	conf.env.FILTER_DIR = conf.env.DATAROOTDIR +'/semantik/filters/'
	conf.define('FILTER_DIR', conf.env.FILTER_DIR)

	path = conf.cmd_and_log(conf.env.QMAKE + ['-query', 'QT_HOST_DATA'], quiet=0, stdout=True)

	path = path.strip()
	if not path:
		raise ValueError('Could not find QT_HOST_DATA')

	specpath = conf.cmd_and_log(conf.env.QMAKE + ['-query', 'QMAKE_SPEC'], quiet=0, stdout=True)
	specpath = os.path.join(path, 'mkspecs', specpath.strip())
	if not os.path.exists(specpath):
		raise ValueError('No spec path, cannot build')

	path = os.path.join(path, 'mkspecs/modules')
	if not os.path.exists(path):
		raise ValueError('Missing path to configuration files? %r' % path)

	conf.env.append_value('INCLUDES_KDECORE', specpath)

	libs = ['KF5KIOCore', 'KF5Auth', 'KF5KIOWidgets',
		'KF5IconThemes', 'KF5ConfigWidgets', 'KF5XmlGui',
		'KF5CoreAddons', 'KF5ConfigGui', 'KF5ConfigCore',
		'KF5WidgetsAddons', 'KF5I18n']

	for lib in libs:
		name = lib[3:]
		if not name.startswith('K'):
			name = 'K' + name

		p = '%s/qt_%s.pri' % (path, name)
		for line in Utils.readf(p).splitlines():
			lst = line.strip().split(' = ')
			if lst[0].endswith('.name'):
				conf.env.append_value('LIB_KDECORE', lst[1])
			elif lst[0].endswith('.includes'):
				conf.env.append_value('INCLUDES_KDECORE', lst[1])
			elif lst[0].endswith('.lib'):
				conf.env.append_value('LIBPATH_KDECORE', lst[1])
			elif lst[0].endswith('.defines'):
				conf.env.append_value('DEFINES_KDECORE', lst[1])

	conf.env.append_value('INCLUDES_KDECORE', '/usr/include/KF5/KDELibs4Support')
	conf.env.append_value('LIB_KDECORE', 'KF5KDELibs4Support')
	conf.check(fragment='#include <kio/job.h>\n#include <KDE/KDialog>\n', features='cxx qt5',
		msg='Checking for kdelibs4support', use='KDECORE QT5CORE QT5GUI QT5WIDGETS')

	conf.define('cmd_add_item', 0)
	conf.define('cmd_update_item', 1)
	conf.define('data_root', os.sep)
	conf.define('cmd_remove_item', 2)
	conf.define('cmd_link', 3)
	conf.define('cmd_unlink', 4)
	conf.define('data_outdir', 'tmp')
	conf.define('cmd_select_item', 5)
	conf.define('cmd_sort_item', 6)
	conf.define('cmd_save_data', 7)
	conf.define('cmd_update_colors', 8)
	conf.define('cmd_update_flags', 9)
	conf.define('cmd_open_map', 10)
	conf.define('cmd_pre_view', 11)
	conf.define('cmd_change_data', 12)
	conf.define('cmd_export_item', 13)

	lst = [x for x in conf.path.find_node('src/styles.txt').read().splitlines()]
	buf = []
	for x in lst:
		if not x: continue
		t = x.split('\t')
		buf.append("""xxp(x,%d,"%s");yyp(x,%d,trUtf8("%s"));""" % (len(buf), t[0], len(buf), t[1].replace('"', '\\"')))
	conf.define('fillglo(x)', ''.join(buf), quote=False)

	lst = [x for x in conf.path.find_node('src/styles_local.txt').read().splitlines()]
	buf = []
	for x in lst:
		if not x: continue
		t = x.split('\t')
		buf.append("""xxp(x,%d,"%s");yyp(x,%d,trUtf8("%s"));""" % (len(buf), t[0], len(buf), t[1].replace('"', '\\"')))
	conf.define('fillloc(x)', ''.join(buf), quote=False)


	conf.env.CXXFLAGS_PYEMBED = [x for x in conf.env.CXXFLAGS_PYEMBED if x != '-g']

	if not 'CXXFLAGS' in os.environ:
		conf.env.CXXFLAGS = '-O2 -pipe -Wno-deprecated-declarations -Wall'.split()# -DDEBUG=1 -g'
		#conf.env.CXXFLAGS = ['-g', '-Wall']
	conf.write_config_header('aux.h')

	# the Debian packagers compile with --prefix=/usr and set /etc/ld.so.conf accordingly
	# the rpath is for end users installing into /usr/local/lib only
	sur = '/usr/local'
	if Options.options.prefix[:len(sur)] == sur:
		conf.env.RPATH_NABLAH = '/usr/local/lib'

	conf.define('GAP', 50, quote=False)

def options(opt):
	opt.load('compiler_c compiler_cxx')
	opt.load('qt5')
	opt.load('python ')
	opt.add_option('--exe', action='store_true', default=False, help='execute semantik after the compilation (developers)')
	opt.add_option('--ddd', action='store_true', default=False, help='execute semantik-d after the compilation (developers)')
	opt.add_option('--icons', action='store', default='', help='icon dirs where to look for kde icons (configuration)')
	opt.add_option('--nomimes', action='store_true', default=False, help='do not run update-mime-database during installation')
	opt.add_option('--noldconfig', action='store_true', default=False, help='do not run lconfig during installation')

def post_build(bld):
	if bld.cmd == 'install':
		if not Options.options.noldconfig:
			try: bld.exec_command('/sbin/ldconfig 2> /dev/null')
			except Exception: pass
		if not Options.options.nomimes:
			try: bld.exec_command('update-mime-database %s' % os.path.split(bld.env.MIME_DIR)[0])
			except Exception: pass

	if Options.options.exe:
		bld.exec_command('LD_LIBRARY_PATH=build/:$LD_LIBRARY_PATH build/src/semantik', stdout=None, stderr=None)
	if Options.options.ddd:
		bld.exec_command('LD_LIBRARY_PATH=build/:$LD_LIBRARY_PATH build/src/semantik-d', stdout=None, stderr=None)


@TaskGen.feature('msgfmt')
def apply_msgfmt(self):
	for lang in self.to_list(self.langs):
		node = self.path.find_resource(lang+'.po')
		task = self.create_task('msgfmt', node, node.change_ext('.mo'))

		langname = lang.split('/')
		langname = langname[-1]

		inst = getattr(self, 'install_path', '${LOCALEDIR}')

		self.add_install_as(
			install_to = os.path.join(inst, langname, 'LC_MESSAGES', getattr(self, 'appname', 'set_your_appname') + '.mo'),
			install_from = task.outputs[0],
			chmod = getattr(self, 'chmod', Utils.O644))

class msgfmt(Task.Task):
	color   = 'BLUE'
	run_str = '${MSGFMT} ${SRC} -o ${TGT}'

