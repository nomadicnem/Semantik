#sem:name: Web document
#sem:tip: Generates a simple html document with a stylesheet

# Thomas Nagy, 2007-2019 GPLV3

# exclude
# caption

import getpass, shutil

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')
pygments_css_data = ''

settings = {
'doc_content':'',
'doc_title':'',
'doc_author': getpass.getuser(),
}
add_globals(settings)

temp_dir = sembind.get_var('temp_dir')
pics = index_pictures(outdir)

buf = []
out = buf.append

def p(s):
	return sembind.protectHTML(s)

def xml(s):
	return sembind.protectXML(s)

def print_nodes(node, niv, lbl_lst):

	lbl = ".".join(lbl_lst)

	typo = node.get_val('type')
	if typo in ['text']:
		if niv == 0:
			settings['doc_title'] = node.get_val('summary')
		elif niv in range(5):
			out('<h%d><span class="show_niv">%s</span>%s</h%d>\n' % \
				(niv, lbl, node.get_val('summary'), niv))

		body = parse_raw(node.get_val('text')).strip()
		lang = node.get_var('code_lang').strip()
		if body:
			if lang:
				html_code = pygmentize(lang, body)
				global pygments_css_data
				if not pygments_css_data:
					pygments_css_data = pygmentize_css()
				out(html_code)
			else:
				debug('For code snippets, set the variable code_lang\n')
				out(truncate_html(node.get_val('text')))
				out('\n\n')

	elif typo == 'table':
		rows = node.num_rows()
		cols = node.num_cols()
		if rows > 0 and cols > 0:
			caption = node.get_var('caption')
			if not caption and node.get_var('nocaption') != 'true'
				caption = node.get_val('summary')

			out('\n')
			out('<table class="sem_table">\n')
			out('<caption>%s</caption>\n' % xml(caption))
			out('<tbody>\n')
			for i in range(rows):
				out('\t<tr>\n')
				for j in range(cols):
					cell = xml(node.get_cell(i, j)).replace('\n', '<br/>')
					if i>0 and j>0:
						out('\t\t<td>%s</td>\n' % cell)
					else:
						out('\t\t<th>%s</th>\n' % cell)
				out('\t</tr>\n')

			out('</tbody>\n')
			out('</table>\n')
		out('\n')

	elif typo == 'img' or typo == 'diag':
		the_pic = pics.get(node.get_val('id'))
		if the_pic and not node.get_var('exclude_pic'):
			caption = node.get_var('caption')
			if not caption: caption = node.get_val('summary')

			style = node.get_var('picstyle')
			out('<img src=\"%s\" alt=\"%s\" title=\"%s\" %s class=\"imgcenter\">\n'
				% (the_pic, xml(caption), xml(caption), style))

	num = node.child_count()
	for i in range(num):
		print_nodes(node.child_num(i), niv+1, lbl_lst+[str(i+1)])

# the main document
print_nodes(Root(), 0, []);
settings['doc_content'] = "".join(buf)

# now write the document
transform("/html/index.html", outdir+'/index.html', settings)

# css file
shutil.copy2(template_dir()+'/html/def.css', outdir)
shutil.copy2(template_dir()+'/html/doc_icon.png', outdir+'/doc_icon.png')

with open(outdir + '/pygments.css', 'w') as f:
	f.write(pygments_css_data)

# okay, code generation has worked
visualize('html', outdir+'/index.html')

