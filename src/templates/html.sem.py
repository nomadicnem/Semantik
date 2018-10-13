#sem:name: Web document
#sem:tip: Generates a simple html document with a stylesheet

# Thomas Nagy, 2007-2018 GPLV3

# exclude
# caption

import os, shutil, time

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

settings = {
'doc_content':'',
'doc_title':'',
}
add_globals(settings)

try:
	tm = time.strftime('.%d-%b-%y--%H-%M-%S', time.gmtime(os.stat(outdir).st_mtime))
	os.rename(outdir, outdir+tm)
except OSError:
	pass

try:
	os.makedirs(outdir)
except OSError:
	debug("Cannot create folder " + outdir)

temp_dir = sembind.get_var('temp_dir')
pics, imgs = copy_pictures(temp_dir, outdir)

buf = []
out = buf.append

def p(s):
	return sembind.protectHTML(s)

def x(s):
	return sembind.protectXML(s)

def print_nodes(node, niv, lbl_lst):

	lbl = ".".join(lbl_lst)

	typo = node.get_val('type')
	if typo == 'text':
		if niv == 0:
			settings['doc_title'] = node.get_val('summary')
		elif niv in range(5):
			out('<h%d><span class="show_niv">%s</span>%s</h%d>\n' % \
				(niv, lbl, node.get_val('summary'), niv))

		y = node.get_val('text')
		out(p(y))

	elif typo == 'table':
		rows = node.num_rows()
		cols = node.num_cols()
		if rows > 0 and cols > 0:
			caption = node.get_var('caption')
			if not caption: caption = node.get_val('summary')

			out('\n')
			out('<table class="sem_table" cellspacing="0px" cellpadding="0px">\n')
			out('<caption>%s</caption>\n' % x(caption))
			out('<tbody>\n')
			for i in range(rows):
				out('\t<tr>\n')
				for j in range(cols):
					if i>0 and j>0:
						out('\t\t<td>%s</td>\n' % x(node.get_cell(i, j)))
					else:
						out('\t\t<th>%s</th>\n' % x(node.get_cell(i, j)))
				out('\t</tr>\n')

			out('</tbody>\n')
			out('</table>\n')
		out('\n')

	elif typo == 'img' or typo == 'diag':
		if typo == 'img':
			the_pic = imgs.get(node.get_val('pic_id'))
		else:
			the_pic = pics.get(node.get_val('id'))

		if the_pic and not node.get_var('exclude_pic'):
			caption = node.get_var('caption')
			if not caption: caption = node.get_val('summary')

			style = node.get_var('picstyle')
			if not style:
				w = int(node.get_val('pic_w'))
				h = int(node.get_val('pic_h'))
				if w > 800: style = " style='width:800px;'"

			divstyle = node.get_var('picdivstyle')
			#captionstyle = node.get_var('piccaptionstyle')

			out('<p><div class=\"img\" %s><img src=\"%s\" alt=\"%s\" title=\"%s\" %s></div></p>\n'
				% (divstyle, the_pic, x(caption), x(caption), style))

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

# okay, code generation has worked
visualize('html', outdir+'/index.html')

