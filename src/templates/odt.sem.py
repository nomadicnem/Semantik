#sem:name: OpenDocument Text
#sem:tip: Generates text documents OpenOffice.org Writer

# Thomas Nagy, 2007-2019 GPLV3

mimetype = "application/vnd.oasis.opendocument.text"

import os, time, zipfile

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

globals = {
'DATE':'<text:date style:data-style-name="N37"></text:date>',
'PAGENUM':'<text:page-number text:select-page="current">1</text:page-number>/<text:page-count>1</text:page-count>',
#'AUTHOR':'<text:initial-creator/>', # <- i wish i could find this one in the gui
#'SUBJECT':'<text:subject/>',
}

settings = {
'doc_content':'',
'manif':'',
'oolang':'fr_FR',
'piclst':[],
'author':'',
'header_l':'@AUTHOR@',
'header_c':'',
'header_r':'@TITLE@',
'footer_l':'@DATE@',
'footer_c':'@PAGENUM@',
'footer_r':'',
}
add_globals(settings)


def p(s):
	return sembind.protectHTML(s)

def xml(s):
	return sembind.protectXML(s)

# substitute @var@, look if there is not a global variable named like that in the globals already
# TODO: use a real parser, but for the moment quote every string
for a in settings.keys():
	val = settings[a]
	if len(val)>3:
		if val[0]=='@' and val[-1]=='@' and not ' ' in val:
			nvar = val[1:-1]
			if nvar in settings: settings[a] = xml(settings[nvar])
			else: settings[a] = globals.get(nvar, '')

# copy the pictures
temp_dir = sembind.get_var('temp_dir')
pics = index_pictures(outdir, pic_prefs='svg,png,jpg,jpeg,gif')

os.mkdir(outdir+'/META-INF')

buf = []
out = buf.append

def print_nodes(node, niv, lbl_lst):

	typo = node.get_val('type')

	txt = xml(node.get_val('summary'))
	#out('<text:h text:style-name="Heading_x_%d" text:outline-level="%d">%s</text:h>\n' % (nv, nv, txt))

	if niv == 0:
		settings['doc_title'] = node.get_val('summary')
	elif niv in range(8) and typo == 'text':

		for xxx in range(niv):
			if xxx == 0:
				out('<text:list text:style-name="L1" text:continue-numbering="true">')
			else:
				out('<text:list text:continue-numbering="true">')
			out('<text:list-item>')

		#out('<text:p text:style-name="P1">%s</text:p>' % txt)
		out('<text:h text:style-name="Heading_x_%d" text:outline-level="%d">%s</text:h>\n' % (niv, niv, txt))

		for xxxx in range(niv):
			out('</text:list-item>')
			out('</text:list>')

		#out('<text:h text:style-name="Heading_x_%d" text:outline-level="%d">%s %s</text:h>\n' % (niv, niv, lbl, txt))

	if typo == 'text':
		y = node.get_val('text')
		if y:
			for line in clear_html(y).split('\n\n'):
				out('<text:p text:style-name="P1">')
				out('<text:span text:style-name="T1">')
				out(xml(line))
				out('</text:span>')
				out('</text:p>')

	elif typo == 'table':
		rows = node.num_rows()
		cols = node.num_cols()
		if rows>0 and cols>0 and not node.get_var('exclude_table'):

			caption = node.get_var('tbl_caption')
			if not caption: caption = '(TODO: set a caption for this table! -> var tbl_caption)'

			out('\n')
			out('<table:table table:name="Tableau1" table:style-name="Tableau1">\n')
			#out('<caption>%s</caption>\n' % xml(caption))
			out('<table:table-column table:style-name="Tableau1.A" table:number-columns-repeated="%d"/>\n' % cols)
			for i in range(rows):
				out('\t<table:table-row>\n')
				for j in range(cols):
					if i>0 and j>0:
						out('\t\t<table:table-cell table:style-name="Tableau1.B2" office:value-type="string">\n')
						out('\t\t\t<text:p text:style-name="P1">%s</text:p>\n' % xml(node.get_cell(i, j)))
						out('\t\t</table:table-cell>\n')
					else:
						out('\t\t<table:table-cell table:style-name="Tableau1.B1" office:value-type="float" office:value="0">\n')
						out('\t\t\t<text:p text:style-name="P1">%s</text:p>\n' % xml(node.get_cell(i, j)))
						out('\t\t</table:table-cell>\n')
				out('\t</table:table-row>\n')
			out('</table:table>\n')

		out('\n')

	elif typo == 'pic' or typo == 'diag':
		the_pic = pics.get(node.get_val('id'))
		if the_pic and not node.get_var('exclude_pic'):
			caption = node.get_var('pic_caption')
			if not caption:
				caption = '(TODO: set a caption for this picture! -> var pic_caption)'

			#out('<draw:frame draw:style-name="fr1" text:anchor-type="paragraph" draw:z-index="0">\n')
			#out('<draw:text-box min-height="3cm">\n')
			#out('<text:p text:style-name="Illustration">\n')

			#out('<draw:frame text:anchor-type="paragraph" style:rel-height="scale" draw:z-index="1" svg:width="5cm">')
			#out('<draw:image xlink:href="Pictures/%s" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>' % pics[id])
			#out('</draw:frame>\n')

			#out('</text:p>\n')
			#out('</draw:text-box>\n')
			#out('</draw:frame>\n')

			#out('<text:p text:style-name="Standard">\n')
			#out('<draw:frame text:anchor-type="paragraph" style:rel-height="scale" draw:z-index="1">\n')
			#out('<draw:image xlink:href="Pictures/pic-6.jpg" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>\n')
			#out('</draw:frame>\n')
			#out('</text:p>\n')

			w = float(node.get_val('widthHint')) / 36; # yuck
			h = float(node.get_val('heightHint')) / 36;

			if w > 15:
				h = (15 * h) / w
				w = 15

			out('<text:p text:style-name="Standard">\n')
			out('<draw:frame draw:style-name="fr1" draw:name="Image1" text:anchor-type="paragraph" ')
			out(' svg:width="%fcm" svg:height="%fcm" ' % (w, h))
			out(' draw:z-index="0">\n')
			out('<draw:image xlink:href="Pictures/%s" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>\n' % the_pic)
			out('</draw:frame>\n')
			out('</text:p>\n')

			settings['manif'] += '<manifest:file-entry manifest:media-type="%s" manifest:full-path="Pictures/%s"/>' % (name_to_mime(the_pic), the_pic)
			settings['piclst'].append(the_pic)

	num = node.child_count()
	for i in range(num):
		print_nodes(node.child_num(i), niv+1, lbl_lst+[str(i+1)])

# the main content
print_nodes(Root(), 0, []);
settings['doc_content'] = "".join(buf)

# prepare files
transform("/odt/manifest.xml", outdir+'/META-INF/manifest.xml', settings)
for x in "content.xml settings.xml styles.xml meta.xml".split():
	transform("/odt/"+x, outdir+'/'+x, settings)

# add files to the zip
with zipfile.ZipFile(outdir+'/main.odt', mode='w') as f:
	f.writestr('mimetype', mimetype)
	for x in "content.xml styles.xml meta.xml".split():
		f.write(os.path.join(outdir, x), x)#, compress_type=zipfile.ZIP_DEFLATED)
	f.write(os.path.join(outdir, 'META-INF/manifest.xml'), 'META-INF/manifest.xml')#, compress_type=zipfile.ZIP_DEFLATED)
	for x in settings['piclst']:
		f.write(os.path.join(outdir, x), 'Pictures/%s' % x)

# and remove the useless stuff
os.popen('cd %s && rm -rf *.xml diag-* META-INF' % outdir)

visualize('odt', outdir+'/main.odt')

