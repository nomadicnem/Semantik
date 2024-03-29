#sem:name: Beamer presentation
#sem:tip: Generates a LaTeX presentation using Beamer

# Thomas Nagy, 2007-2019 GPLV3

import getpass, os, shutil, sys, time

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

settings = {
'all_latex': False,
'doc_content':'',
'doc_title':'',
'doc_author': getpass.getuser(),
'doc_author_off':'None',
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

'uncover_stepwise': '',

'each_subsection_off':'None',
'doc_tableofcontents_off':'None',

'use_minted': 0, # optional
}
add_globals(settings)

# do the document in latex ?
def tex_convert(s):
	return s
if not settings.get('all_latex', 0): tex_convert = protect_tex

temp_dir = sembind.get_var('temp_dir')
pics = index_pictures(outdir, pic_prefs='pdf,png,jpg,jpeg,gif')

buf = []
out = buf.append

def print_slide(node, niv):
	if node.get_var('disable_slide', False):
		return
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
def print_figure_slides(node):
	if node.get_val("id") in diagrams_added:
		return
	diagrams_added.add(node.get_val("id"))

	caption = node.get_var('caption')
	if not caption and not node.get_var('disable_caption', False):
		caption = node.get_val('summary')

	typo = node.get_val('type')
	if typo in ['text']:
		body = parse_raw(node.get_val('text')).strip()
		lang = node.get_var('code_lang').strip() or node.get_var('minted_lang').strip()
		if body and not lang:
			sys.stderr.write('For code snippets, set the variable code_lang\n')

		if body and lang:
			settings['use_minted'] = 1
			filename = 'code-%s.minted' % node.get_val("id")
			minted_opts = node.get_var('minted_opts').strip() or 'autogobble,fontsize=\\tiny'
			with open(outdir + '/' + filename, 'w', encoding='utf-8') as f:
				f.write(body)

			title = tex_convert(node.get_val('summary'))
			out('\\begin{frame}[fragile]\n')
			out('\\frametitle{%s}\n\n' % title)

			if node.get_var('minted_raw'):
				# \inputminted[autogobble,fontsize=\Large]{python}{../code-42.minted}
				out('\\inputminted[%s]{%s}{%s}\n' % (minted_opts, lang, '../' + filename))
			else:
				out('\\begin{tcbinputlisting} {')
				minted_caption = node.get_var('caption')
				if minted_caption:
					out('title=%s,\n' % protect_tex(caption))
				out('colback=%s,\n' % node.get_var('minted_colback', 'blue!3'))
				out('colframe=%s,\n' % node.get_var('minted_colframe', 'black'))
				out('boxrule=%s,\n' % node.get_var('minted_boxrule', '0.3pt'))
				out('fonttitle=%s,\n' % node.get_var('minted_fonttitle', '\\bfseries\\tiny'))
				out('left=%s,\n' % node.get_var('minted_left', '5mm' if 'linenos' in minted_opts else '0mm'))
				out('listing engine=minted,\n')
				out('minted language=%s,\n' % lang)
				out('listing file=../%s,\n' % filename)
				out('minted options={%s},\n' % minted_opts)
				out('listing only}\n')
				out('\end{tcbinputlisting}\n')

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

				out('\\begin{table}\n')

				out('\\begin{center}\n')
				out('\\setlength{\\tabcolsep}{2pt}\n')

				out('\\begin{adjustbox}{max width=\\textwidth,max totalheight=\\textheight,keepaspectratio}\n')
				out('\\begin{tabular}{|%s}' % ('l|'*cols))
				out(' \\hline\n')

				disable_row_header = node.get_var('disable_row_header', False)
				disable_col_header = node.get_var('disable_col_header', False)

				for i in range(rows):
					for j in range(cols):
						cell = tex_convert(node.get_cell(i, j)).replace('\n', ' ')
						if (i == 0 and not disable_row_header) or (j == 0 and not disable_col_header):
							out('\\textbf{%s}' % cell)
						else:
							out('%s' % cell)
						if j < cols - 1: out(" & ")
					out(' \\\\ \\hline\n')
				out('\\end{tabular}\n')
				out('\\end{adjustbox}\n')
				out('\\end{center}\n')

				if caption:
					out('\\caption{%s}\n' % tex_convert(caption))
				out('\\end{table}\n')

			out('\n')

		elif typo == 'img' or typo == 'diag':
			the_pic = pics.get(node.get_val('id'))
			if the_pic and not node.get_var('exclude_pic'):
				restrict = node.get_var("picdim")
				#if not restrict:
				#	w = int(node.get_val('pic_w'))
				#	restrict = ""
				#	if (w > 5*72): restrict = "[width=5in]"
				if not restrict:
					rat = '0.75'
					if caption:
						rat = '0.7'
					restrict = "[width=\\textwidth,height=%s\\textheight,keepaspectratio]" % rat

				out('\\begin{figure}[htbp]\n')
				out('  \\begin{center}\n')
				out('    \\includegraphics%s{%s}\n' % (restrict, the_pic))
				if caption:
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

# 0. all nodes result in some text being output
# 1. unless explicitly stated, figures only result
# 2. top-level nodes always make a section
def print_nodes(node, niv):
	num = node.child_count()
	for i in range(num):
		subtree = node.child_num(i)
		title = tex_convert(subtree.get_val('summary'))

		num = subtree.child_count()
		if niv == 0:
			out('%% %s\n' % ('=' * 79))
			out('\\section{%s}\n' % title)

		#elif niv == 1 and num >= 1:
		#	out('\\subsection{%s}\n' % title)

		if subtree.child_count() >= 0:
			if int(subtree.get_val('tree_size')) < 16:
				print_slide(subtree, 0);
				print_figure_slides(subtree)
			elif niv == 0:
				print_nodes(subtree, 1)
				print_figure_slides(subtree)
			elif niv == 1:
				print_nodes(subtree, 2)
				print_figure_slides(subtree)
			else:
				print_figure_slides(subtree)
				sys.stderr.write("transforming this map into slides makes kitten cry")

# the main document
root = Root()
if not settings['doc_title']:
	settings['doc_title'] = root.get_val('summary')
print_nodes(root, 0);
settings['doc_content'] = ''.join(buf)

# now write main.tex
transform("/beamer/main.tex", outdir+'/main.tex', settings)

shutil.copy2(template_dir()+'/beamer/wscript', outdir+'/wscript')
shutil.copy2(template_dir()+'/waf', outdir+'/waf')
os.chmod(outdir+'/waf', 0o755)

if settings['use_minted']:
	with open(outdir + '/use_minted.txt', 'w', encoding='utf-8') as f:
		f.write('')

with open(outdir + '/run.sh', 'w', encoding='utf-8') as f:
	f.write('#! /bin/sh\npython waf configure build --view\n')
os.chmod(outdir + '/run.sh', 0o755)

# load the preview on main.tex
visualize('beamer', outdir+'/main.tex')

