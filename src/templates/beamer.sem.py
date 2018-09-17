#sem:name: Beamer presentation
#sem:tip: Generates a LaTeX presentation using Beamer

# Thomas Nagy, 2007-2018 GPLV3

import os, shutil, time, sys
import getpass

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

settings = {
'all_latex': False,
'doc_content':'',
'doc_title':'',
'doc_author': getpass.getuser(),
'doc_place': 'Stockholm',
'doc_company': 'World Company',
'doc_code_on': False,

'theme':'Warsaw',
'babel':'english', # frenchb
#'doc_tableofcontents_off':''
'content_title':'Contents',

'doc_title':'',
'doc_title_off':'None',

'doc_date':r'\today',
'doc_date_off':'None',

'doc_author':'',
'doc_author_off':'None',
'uncover_stepwise': '',
'has_minted': 0, # optional

'each_subsection_off':'None',
'doc_tableofcontents_off':'None',
}
add_globals(settings)

# do the document in latex ?
def tex_convert(s):
	return s
if not settings.get('all_latex', 0): tex_convert = protect_tex

try:
	tm = time.strftime('.%d-%b-%y--%H-%M-%S', time.gmtime(os.stat(outdir).st_mtime))
	os.rename(outdir, outdir+tm)
except OSError:
	pass

try: os.makedirs(outdir)
except OSError: debug('Cannot create folder ' + outdir)

# copy the pictures
cwd = os.getcwd()
os.chdir(sembind.get_var('temp_dir'))
pics = {} # map the id to the picture
lst = os.listdir('.')
for x in lst:
	if x.startswith('diag-'):
		key = x.split('.')[0].replace('diag-', '')
		if x.endswith('.pdf') or not key in pics:
			pics[key] = x
		shutil.copy2(x, outdir)
os.chdir(cwd)


buf = []
out = buf.append

def print_slide(node, niv):
	txt = tex_convert(node.get_val('summary'))

	if niv == 0:
		num = node.child_count()

		out('%-------------------------------------------------------------------\n')
		out('\\begin{frame}\n')
		out('\\frametitle{%s}\n\n' % txt)
		num = node.child_count()
		if num:
			out("\\begin{itemize}\n")
			for i in range(num):
				print_slide(node.child_num(i), niv+1)
			out("\\end{itemize}\n")
		out('\\end{frame}\n')
		out('%-------------------------------------------------------------------\n')

	elif niv < 3:
		if txt: out('\\item %s\n' % txt)
		num = node.child_count()
		if num:
			out("\\begin{itemize}\n")
			for i in range(num):
				print_slide(node.child_num(i), niv+1)
			out("\\end{itemize}\n")
	elif niv == 3:
		if txt: out('\\item %s\n' % txt)
		num = node.child_count()
		for i in range(num):
			print_slide(node.child_num(i), niv+1)

	else:
		if txt: out('%s %s\n' % ('%', txt))
		num = node.child_count()
		for i in range(num):
			print_slide(node.child_num(i), niv+1)

diagrams_added = set([]) # prevent accidents
def print_figure_slides(node, recurse=False):
	if node.get_val("id") in diagrams_added:
		return
	diagrams_added.add(node.get_val("id"))

	typo = node.get_val('type')
	if typo in ['text']:
		body = parse_raw(node.get_val('text')).strip()
		lang = node.get_var('minted_lang').strip()
		if body and not lang:
			sys.stderr.write('For code snippets, set the variable minted_lang\n')

		if body and lang:
			settings['has_minted'] = 1
			filename = 'code-%s.minted' % node.get_val("id")
			with open(outdir + '/' + filename, 'w', encoding='utf-8') as f:
				f.write(body)

			title = tex_convert(node.get_val('summary'))
			out('\\begin{frame}[fragile]\n')
			out('\\frametitle{%s}\n\n' % title)
			out('\\begin{tcolorbox}\n')
			out('\\tiny\n')
			out('\\ttfamily\n')
			out('\\inputminted{%s}{%s}\n' % (lang, '../' + filename))
			out('\\end{tcolorbox}\n')
			out('\\end{frame}\n')
	elif typo in ['table', 'diag', 'img']:
		txt = tex_convert(node.get_val('summary'))
		out('%-------------------------------------------------------------------\n')
		out('\\begin{frame}\n')
		out('\\frametitle{%s}\n\n' % txt)

		if typo == 'table':
			rows = node.num_rows()
			cols = node.num_cols()
			if rows>0 and cols>0:

				caption = node.get_var('caption')
				if not caption: caption = caption = node.get_val('summary')

				out('\\begin{table}\n')

				out('\\begin{center}\n')
				out('\\begin{tabular}{|%s}' % ('c|'*cols))
				out(' \\hline\n')
				for i in range(rows):
					for j in range(cols):
						if i == 0 or j == 0:
							out('\\textbf{%s}' % tex_convert(node.get_cell(i, j)))
						else:
							out('%s' % tex_convert(node.get_cell(i, j)))
						if j < cols - 1: out(" & ")
					out(' \\\\ \\hline\n')
				out('\\end{tabular}\n')
				out('\\end{center}\n')

				out('\\caption{%s}\n' % tex_convert(caption))
				out('\\end{table}\n')

			out('\n')

		elif typo == 'img' or typo == 'diag':
			id = node.get_val('id')
			if id in pics:

				caption = node.get_var('caption')
				if not caption: caption = caption = node.get_val('summary')

				restrict = node.get_var("picdim")
				if not restrict:
					w = int(node.get_val('pic_w'))
					restrict = ""
					if (w > 5*72): restrict = "[width=5in]"
				if not restrict:
					restrict = "[width=0.8\\textwidth,height=0.7\\textheight,keepaspectratio]"

				out('\\begin{figure}[htbp]\n')
				out('  \\begin{center}\n')
				out('    \\includegraphics%s{%s}\n' % (restrict, pics[id]))
				out('    \\caption{\\footnotesize{%s}}\n' % tex_convert(caption))
				out('%% %s\n' % protect_tex(node.get_val('pic_location')))
				out('%% %s\n' % node.get_val('pic_w'))
				out('%% %s\n' % node.get_val('pic_h'))
				out('    \\end{center}\n')
				out('\\end{figure}\n')

		out('\\end{frame}\n')
		out('%-------------------------------------------------------------------\n')

	num = node.child_count()
	for i in range(num):
		print_figure_slides(node.child_num(i))
	#print_figure_slides(root, recurse=False)

def print_nodes(node, niv):

	num = node.child_count()
	for i in range(num):
		subtree = node.child_num(i)
		#print_nodes(node.child_num(i), niv+1)

		sm = tex_convert(subtree.get_val('summary'))
		num = subtree.child_count()
		if niv == 0 and num >= 1:
			out('\\section{%s}\n' % sm)
		elif niv == 1 and num >= 1:
			out('\\subsection{%s}\n' % sm)

		if subtree.child_count() > 0:
			if int(subtree.get_val('tree_size')) < 16:
				print_slide(subtree, 0);
				print_figure_slides(subtree, True)
			elif niv == 0:
				print_nodes(subtree, 1)
				print_figure_slides(subtree)
			elif niv == 1:
				print_nodes(subtree, 2)
				print_figure_slides(subtree)
			else:
				print_figure_slides(subtree, True)
				sys.stderr.write("transforming this map into slides makes kitten cry")

# the main document
root = Root()
if not settings['doc_title']:
	settings['doc_title'] = root.get_val('summary')
print_nodes(root, 0);
settings['doc_content'] = ''.join(buf)

# now write main.tex
transform("/beamer/main.tex", outdir+'/main.tex', settings)

# data files
os.popen('cp -Rf %s %s' % (template_dir()+'/beamer/beamermindist/', outdir)).read()

with open(template_dir()+'/beamer/wscript', encoding='utf-8') as f:
	wscript_code = f.read()
if settings['has_minted']:
	wscript_code = wscript_code.replace('#minted: ', '')
with open(outdir+'/wscript', 'w', encoding='utf-8') as f:
	f.write(wscript_code)

shutil.copy2(template_dir()+'/waf', outdir+'/waf')
os.chmod(outdir+'/waf', 0o755)

with open(outdir + '/run.sh', 'w', encoding='utf-8') as f:
	f.write('#! /bin/sh\npython waf configure build --view\n')
os.chmod(outdir + '/run.sh', 0o755)

# load the preview on main.tex
visualize('beamer', outdir+'/main.tex')

