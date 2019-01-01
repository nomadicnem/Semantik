#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2019 GPLV3

import os, tarfile, string

# parsing xml properly is less simple than it seems
def filter(txt):
	out = []
	buf = ""
	replace = ""
	for x in txt:
		if 0: pass
		elif replace == '' and x == ' ': replace = ' '
		elif replace == ' ' and x == 'i': replace = ' i'
		elif replace == ' ' and x == 'p': replace = ' p'
		elif replace == ' ' and x == 'v': replace = ' v'
		elif replace == ' i' and x == 'd': replace = ' id'
		elif replace == ' id' and x == '=': replace = ' id='
		elif replace == ' p' and x == '=': replace = ' p='
		elif replace == ' v' and x == '=': replace = ' v='
		elif replace == ' id=' and x == '"': replace = ' id="'
		elif replace == ' p=' and x == '"': replace = ' p="'
		elif replace == ' v=' and x == '"': replace = ' v="'
		elif (replace == ' id="' or replace == ' p="' or replace == ' v="') and x in string.digits: buf+=x
		elif (replace == ' id="' or replace == ' p="' or replace == ' v="') and x=='"':
			val = int(buf) + 1
			buf = ""
			replace += '%d"'
			out.append(replace % val)

			replace = ''
		else:
			if replace: out.append(replace)
			out.append(x)
			replace = ''

	ret = "".join(out)

	#file = open('/tmp/de.xml', 'w')
	#file.write(ret)
	#file.close()

	return ret

def parse_file(infile, tmpdir):
	tar = tarfile.open(infile)
	for tarinfo in tar:
		tar.extract(tarinfo, path=tmpdir)
	tar.close()

	fname = os.path.join(tmpdir, 'com.xml')
	if not os.path.exists(fname):
		fname = os.path.join(tmpdir, 'con.xml')

	with open(fname, 'r', encoding='utf-8') as f:
		txt = f.read()
	os.remove(fname)
	return txt

