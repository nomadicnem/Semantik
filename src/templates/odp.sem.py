#sem:name: OpenDocument Presentation
#sem:tip: Generates presentations for OpenOffice.org Impress

# Thomas Nagy, 2007-2018 GPLV3

mimetype = "application/vnd.oasis.opendocument.presentation"

import os, time, zipfile, sys

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

settings = {
'doc_content':'',
'manif':'',
'oolang':'fr_FR',
}
add_globals(settings)


temp_dir = sembind.get_var('temp_dir')
pics = index_pictures(outdir, pic_prefs='svg,png,jpg,jpeg,gif')

try: os.mkdir(outdir+'/META-INF')
except: raise


buf = []
out = buf.append

def p(s):
	return sembind.protectHTML(s)

def xml(s):
	return sembind.protectXML(s)

def print_slide(node, niv):
	txt = xml(node.get_val('summary'))
	if niv == 0:
		begin = """
      <draw:page draw:name="page1" draw:style-name="dp1" draw:master-page-name="lyt-cool" presentation:presentation-page-layout-name="AL1T1">
        <office:forms form:automatic-focus="false" form:apply-design-mode="false"/>
        <draw:frame presentation:style-name="pr1" draw:text-style-name="P1" draw:layer="layout" svg:width="23.911cm" svg:height="3.507cm" svg:x="2.058cm" svg:y="1.543cm" presentation:class="title" presentation:user-transformed="true">
          <draw:text-box>
            <text:p text:style-name="P1">%s</text:p>
          </draw:text-box>
        </draw:frame>
        <draw:frame presentation:style-name="pr2" draw:text-style-name="P2" draw:layer="layout" svg:width="23.911cm" svg:height="13.23cm" svg:x="2.058cm" svg:y="5.838cm" presentation:class="outline" presentation:user-transformed="true">
	"""

		end ="""
        </draw:frame>
        <presentation:notes draw:style-name="dp2">
          <draw:page-thumbnail draw:layer="layout" svg:width="13.705cm" svg:height="10.279cm" svg:x="3.647cm" svg:y="2.853cm" draw:page-number="1"/>
          <draw:frame presentation:style-name="pr3" draw:text-style-name="P1" draw:layer="layout" svg:width="14.517cm" svg:height="11.41cm" svg:x="3.249cm" svg:y="14.13cm" presentation:class="notes" presentation:placeholder="true" presentation:user-transformed="true">
            <draw:text-box draw:corner-radius="0.001cm"/>
          </draw:frame>
        </presentation:notes>
      </draw:page>
"""
		out(begin % xml(txt))

		num = node.child_count()
		if num:
			out("<draw:text-box>\n")
			out('  <text:list text:style-name="L2">\n')
			for i in range(num):
				print_slide(node.child_num(i), niv+1)
			out("  </text:list>\n")
			out("</draw:text-box>\n")

		out(end)

		other = """
          <draw:text-box>
            <text:list text:style-name="L2">
              <text:list-item>
                <text:p text:style-name="P2">SOCIETE</text:p>
              </text:list-item>
            </text:list>
            <text:list text:style-name="L2">
              <text:list-item>
                <text:p text:style-name="P2"/>
              </text:list-item>
            </text:list>
            <text:list text:style-name="L2">
              <text:list-item>
                <text:p text:style-name="P2">AUTRES_IDEES_A_TRAITER</text:p>
              </text:list-item>
            </text:list>
          </draw:text-box>
		"""

	else:
		if txt:
			out('<text:list-item>\n')
			out('<text:p text:style-name="P2">%s</text:p>\n' % txt)
		num = node.child_count()
		if num:
			out('<text:list text:style-name="L2">\n')
			for i in range(num):
				print_slide(node.child_num(i), niv+1)
			out("</text:list>\n")
		if txt:
			out('</text:list-item>\n')

def print_nodes(node, niv):

	num = node.child_count()
	for i in range(num):
		subtree = node.child_num(i)
		#print_nodes(node.child_num(i), niv+1)

		sm = xml(subtree.get_val('summary'))
		num = subtree.child_count()
		if niv == 0 and num >= 1:
			out('<!-- %s -->\n' % sm)
		elif niv == 1 and num >= 1:
			out('<!-- %s -->\n' % sm)

		if int(subtree.get_val('tree_size')) < 10:
			print_slide(subtree, 0);
		elif niv == 0:
			print_nodes(subtree, 1)
		elif niv == 1:
			print_nodes(subtree, 2)
		else:
			sys.stderr.write("transforming this map into slides makes kitten cry")

# the main content
print_nodes(Root(), 0);
settings['doc_content'] = "".join(buf)

# prepare files
transform("/odp/manifest.xml", outdir+'/META-INF/manifest.xml', settings)
for x in "content.xml settings.xml styles.xml meta.xml".split():
	transform("/odp/"+x, outdir+'/'+x, settings)

# add files to the zip
with zipfile.ZipFile(outdir+'/main.odp', mode='w') as f:
	f.writestr('mimetype', mimetype)
	for x in "content.xml styles.xml meta.xml".split():
		f.write(os.path.join(outdir, x), x)#, compress_type=zipfile.ZIP_DEFLATED)
	f.write(os.path.join(outdir, 'META-INF/manifest.xml'), 'META-INF/manifest.xml')#, compress_type=zipfile.ZIP_DEFLATED)

# and remove the useless stuff
os.popen('cd %s && rm -rf *.xml diag-* META-INF' % outdir)

visualize('odp', outdir+'/main.odp')

