#sem:name: PDF LaTeX
#sem:tip: Generates a LaTeX project for obtaining PDF files

# Thomas Nagy, 2007-2018 GPLV3

import os, time, shutil, re, getpass

# Additional variables:
# exclude   1
# picdim    [width=10cm]

settings = {
'doc_content':'',
'doc_class':'article',

'all_latex':False,
'header_off':'',
'footer_off':'',
'header_l':'',
'header_c':'',
'header_r':'',
'footer_l':'',
'footer_c':r'\thepage/\pageref{LastPage}',
'footer_r':r'\today',

'doc_title':'',
'doc_title_off':'None',

'doc_date':'',
'doc_date_off':'None',

'doc_author':getpass.getuser(),
'doc_author_off':'None',

'babel':'english', # frenchb

'use_minted': 0, # optional
}
add_globals(settings)


outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

try:
	tm = time.strftime('.%d-%b-%y--%H-%M-%S', time.gmtime(os.stat(outdir).st_mtime))
	os.rename(outdir, outdir+tm)
except OSError:
	pass

try:
	os.makedirs(outdir)
except OSError:
	debug("Cannot create folder " + outdir)


# do the document in latex ?
def tex_convert(s):
	return s
if not settings.get('all_latex', 0): tex_convert = protect_tex

# copy the pictures
temp_dir = sembind.get_var('temp_dir')
pics = {} # map the id to the picture
lst = os.listdir(temp_dir)
for x in lst:
	if x.startswith('diag-'):
		key = x.split('.')[0].replace('diag-', '')
		if x.endswith('.pdf') or not key in pics:
			pics[key] = x
		shutil.copy2(os.path.join(temp_dir, x), outdir)
	elif x.startswith('img-'):
		key = x.split('.')[0].replace('img-', '')
		if not key in pics:
			pics[key] = x
		shutil.copy2(os.path.join(temp_dir, x), outdir)

buf = []
out = buf.append

def print_nodes(node, niv):
	sm = tex_convert(node.get_val('summary'))
	if settings['doc_class'] in ['book', 'report']:
		if niv == 1:
			out('\\chapter{%s}\n' % sm)
		elif niv == 2:
			out('\\section{%s}\n' % sm)
		elif niv == 3:
			out('\\subsection{%s}\n' % sm)
		elif niv == 4:
			out('\\subsubsection{%s}\n' % sm)
		elif niv == 5:
			out('\\paragraph{%s}\n' % sm)
	else:
		if niv == 1:
			out('\\section{%s}\n' % sm)
		elif niv == 2:
			out('\\subsection{%s}\n' % sm)
		elif niv == 3:
			out('\\subsubsection{%s}\n' % sm)
		elif niv == 4:
			out('\\paragraph{%s}\n' % sm)

	typo = node.get_val('type')
	if typo == 'text':
		body = parse_raw(node.get_val('text')).strip()
		lang = node.get_var('minted_lang').strip()
		if body and not lang:
			sys.stderr.write('For code snippets, set the variable minted_lang\n')

		if lang:
			settings['use_minted'] = 1
			filename = 'code-%s.minted' % node.get_val("id")
			with open(outdir + '/' + filename, 'w', encoding='utf-8') as f:
				f.write(body)

			title = tex_convert(node.get_val('summary'))
			out('\\begin{figure}[htbp]\n')
			out('  \\begin{center}\n')
			out('     \\begin{tcolorbox}\n')
			out('       \\tiny\n')
			out('       \\ttfamily\n')
			out('       \\inputminted{%s}{%s}\n' % (lang, '../' + filename))
			out('     \\end{tcolorbox}\n')
			out('  \\end{center}\n')
			out('\\end{figure}\n')
		else:
			out(parse_string(node.get_val('text')))

	elif typo == 'table':
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
		id = node.get_val('id' if typo == 'diag' else 'pic_id')
		if id in pics:

			caption = node.get_var('caption')
			if not caption: caption = caption = node.get_val('summary')

			restrict = node.get_var("picdim")
			#if not restrict:
			#	w = int(node.get_val('pic_w'))
			#	restrict = ""
			#	if (w > 5*72): restrict = "[width=5in]"
			if not restrict:
				restrict = "[width=0.95\\textwidth,height=0.95\\textheight,keepaspectratio]"

			out('\\begin{figure}[htbp]\n')
			out('  \\begin{center}\n')
			out('    \\includegraphics%s{%s}\n' % (restrict, pics[id]))
			out('    \\caption{\\footnotesize{%s}}\n' % tex_convert(caption))
			out('%% %s\n' % protect_tex(node.get_val('pic_location')))
			out('%% %s\n' % node.get_val('pic_w'))
			out('%% %s\n' % node.get_val('pic_h'))
			out('    \\end{center}\n')
			out('\\end{figure}\n')

	num = node.child_count()
	for i in range(num):
		print_nodes(node.child_num(i), niv+1)

root = Root()

title = root.get_val('title')
if title:
	settings['doc_title_off']=''
	settings['doc_title']=title

date = r'\today' #root.get_val('date')
if date:
	settings['doc_date_off']='None'
	settings['doc_date']=date

author = root.get_val('pname')
if author:
	settings['doc_author_off']=''
	settings['doc_author']=author

# the main document
print_nodes(root, 0);
settings['doc_content'] = "".join(buf)

# now write main.tex
transform("/pdflatex/main.tex", outdir+'/main.tex', settings)

shutil.copy2(template_dir()+'/pdflatex/wscript', outdir+'/wscript')
shutil.copy2(template_dir()+'/waf', outdir+'/waf')
os.chmod(outdir+'/waf', 0o755)

if settings['use_minted']:
	with open(outdir + '/use_minted.txt', 'w', encoding='utf-8') as f:
		f.write('')

with open(outdir + '/run.sh', 'w', encoding='utf-8') as f:
	f.write('#! /bin/sh\npython waf configure build --view\n')
os.chmod(outdir + '/run.sh', 0o755)

# load the preview on main.tex
visualize('pdflatex', outdir+'/main.tex')

