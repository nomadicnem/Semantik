#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2018 GPLV3

from html.parser import HTMLParser
import os, re, sembind, shutil, sys

protectXML = sembind.protectXML
sys.path = [sembind.get_var('template_dir')]+sys.path

if not hasattr(sys, "argv"): sys.argv = ['semantik']
#sys.stderr.write(str(sys.argv))

def subst_vars(doc, map):
	ugh = '-<[{($)}]>-'
	tmp = doc.replace('%', ugh)
	m4_re = re.compile('@(\w+)@', re.M)
	tmp = m4_re.sub(r'%(\1)s', tmp)
	tmp = tmp % map
	tmp = tmp.replace(ugh, '%')
	return tmp

def visualize(type, doc):
	var = 'command_'+type
	if var in settings:
		os.popen(settings[var] % doc).read()
	else:
		sembind.set_var('preview', doc)

def read_properties(code):
	tmp={}
	for x in code.split('\n'):
		if not x: continue
		if x[0]=="#": continue
		lst = x.split("=")
		if len(lst) < 2: continue
		tmp[lst[0]] = "=".join(lst[1:])
	return tmp

class RawProcessor(HTMLParser):
	def reset(self):
		self.pieces = []
		self.state = ""
		self.buf = []
		self.inline_level = []
		HTMLParser.reset(self)

	def handle_starttag(self, tag, attrs):
		if tag == 'ul':
			self.inline_level.append(0)
			if self.inli and self.buf:
				self.pieces.append('  ' * len(self.inline_level))
				self.pieces.append(''.join(self.buf))
				self.pieces.append('\n')
		elif tag == 'li':
			self.inline_level[-1] += 1

	def handle_endtag(self, tag):
		if tag == 'p':
			self.pieces.append(''.join(self.buf))
		elif tag == 'li':
			if self.buf:
				self.pieces.append('  ' * len(self.inline_level))
				self.pieces.append('%s. ' % self.inline_level[-1])
				self.pieces.append(''.join(self.buf))
				self.pieces.append('\n')
		elif tag == 'ul':
			self.inline_num.pop()
		elif tag == 'style':
			pass
		elif tag == 'br':
			self.buf.append('\n')
		else:
			self.pieces.append(''.join(self.buf))

		self.buf = []

	def handle_data(self, text):
		self.buf.append(text)

	def output(self):
		return "".join(self.pieces)

def parse_raw(s):
	parser = RawProcessor()
	parser.feed(s)
	parser.close()
	return parser.output()

class TrucProcessor(HTMLParser):
	def reset(self):
		self.pieces = []
		self.state = ""
		self.buf = []
		self.inli = 0
		HTMLParser.reset(self)

	def handle_starttag(self, tag, attrs):
		if tag == 'ul':
			if self.inli and self.buf:
				self.pieces.append('\\item ')
				self.pieces.append(tex_convert(''.join(self.buf)))
				self.pieces.append('\n')
			self.pieces.append('\\begin{itemize}\n')
		elif tag == 'li':
			self.inli += 1

	def handle_endtag(self, tag):
		if tag == 'p':
			self.pieces.append(tex_convert(''.join(self.buf)))
			self.pieces.append('\n')
		elif tag == 'li':
			if self.buf:
				self.pieces.append('\\item ')
				self.pieces.append(tex_convert(''.join(self.buf)))
				self.pieces.append('\n')
			self.inli -= 1
		elif tag == 'ul':
			self.pieces.append('\\end{itemize}\n')
		elif tag == 'style':
			pass
		else:
			self.pieces.append(tex_convert(''.join(self.buf)))

		self.buf = []

	def handle_data(self, text):
		self.buf.append(text)

	def output(self):
		return "".join(self.pieces)

def parse_string(s):
	parser = TrucProcessor()
	parser.feed(s)
	parser.close()
	return parser.output()

class RichProcessor(HTMLParser):
	def reset(self):
		self.pieces = []
		self.state = ""
		self.buf = []
		self.inli = 0
		HTMLParser.reset(self)

	def handle_starttag(self, tag, attrs):
		if tag == 'ul':
			self.inli += 1

	def handle_endtag(self, tag):
		if tag == 'p':
			self.pieces.extend(self.buf)
			self.pieces.append('\n')
		elif tag == 'li':
			if self.buf:
				self.pieces.append(' ' * self.inli)
				self.pieces.extend(self.buf)
				self.pieces.append('\n')
		elif tag == 'ul':
			self.inli -= 1
		elif tag == 'style':
			pass
		else:
			self.pieces.extend(self.buf)

		self.buf = []

	def convert_entityref(self, name):
		return '&%s;' % name

	def handle_data(self, text):
		self.buf.append(text)

	def output(self):
		return "".join(self.pieces)

def clear_html(s):
	parser = RichProcessor()
	parser.feed(s)
	parser.close()
	return parser.output()


def template_dir():
	return sembind.get_var('template_dir')
#TEMPLATE_DIR = sembind.get_var('template_dir')
#FILTER_DIR = sembind.get_var('filter_dir')
#GLOBAL_VARS = read_properties(sembind.get_var('hints'))

def post_process(txt, defines):
	nested = 0
	nested_lst = list(range(30))
	nested_lst[0]=1

	lst = txt.split('\n')
	out = []

	def evaluate(txt):
		negate = 0
		ret = 0
		if len(txt)>1 and txt[0] == '!':
			txt = txt[1:]
			negate = -1
		try:
			num = int(txt)
			if num: ret=1
		except:
			if txt in defines:
				if defines[txt]:
					ret = 1
		ret = ret+negate
		#if ret<0: ret = 1
		return ret

	for x in lst:
		if x.find('#endif')==0:
			nested -= 1
		elif x.find('#if ')==0:
			nested += 1
			txt = x.replace('#if ', '')
			nested_lst[nested] = evaluate(txt)
		#elif x.find('#elif ')==0:
		#	if nested_lst[nested]:
		#		nested_lst[nested] = "skip"
		#		continue
		#	txt = x.replace('#elif ', '')
		#	nested_lst[nested] = evaluate(txt)
		elif x.find('#ifdef ')==0:
			nested += 1
			txt = x.replace('#ifdef ', '').rstrip()
			nested_lst[nested] = (txt in defines)
		elif x.find('#ifndef ')==0:
			nested += 1
			txt = x.replace('#ifndef ', '').rstrip()
			nested_lst[nested] = not (txt in defines)
		elif x.find('#else')==0:
			if nested_lst[nested]:
				nested_lst[nested]=0
			else:
				nested_lst[nested]=1
		else:
			if nested_lst[nested]:
				out.append(x)
	return "\n".join(out)

def write_to_file(name, content):
	if sys.platform.rfind('32') > -1: return
	with open(name, 'w', encoding='utf-8') as f:
		f.write(content)

def read_file(name):
	with open(name, 'r', encoding='utf-8') as f:
		doc = f.read()
	return doc

def add_globals(table):
	GLOBAL_VARS = read_properties(sembind.get_var('hints'))
	template_name = sembind.get_var('namet').split('/')[-1].replace('.sem.py', '')
	for x in GLOBAL_VARS:
		table[x] = GLOBAL_VARS[x]
		lst = x.split('.')
		# override for sub-commands
		if len(lst)>1 and lst[1] == template_name:
			table[lst[0]] = GLOBAL_VARS[x]

def transform(template, outfile, map):
	doc = read_file(template_dir() + template)
	doc = subst_vars(doc, settings)
	doc = post_process(doc, settings)
	write_to_file(outfile, doc)

def debug(x):
	msg = "%s\n" % x
	sys.stderr.write(msg.encode())

def protect_tex(s):
	lst = []
	for x in s:
		if x == '\\': lst.append(r'$\backslash$')
		elif x == '#': lst.append(r'\#')
		elif x == '$': lst.append(r'\$')
		elif x == '%': lst.append(r'\%')
		elif x == '&': lst.append(r'\&')
		elif x == '_': lst.append(r'\_')
		elif x == '{': lst.append(r'\{')
		elif x == '}': lst.append(r'\}')
		elif x == '^': lst.append(r'\^{}')
		elif x == '~': lst.append(r'\~{}')
		elif x == '<': lst.append('$<$')
		elif x == '>': lst.append('$>$')
		else: lst.append(x)
	return "".join(lst)

def compute_hints(x):
	item = sembind.get_item_by_id(int(x))
	if item:
		m = read_properties(sembind.get_val(item, "hints"))
		sembind.set_result("diagram_width", m.get('diagram_width', '0'))
		sembind.set_result("diagram_height", m.get('diagram_height', '0'))

re_img = re.compile('^(diag|img)-(0|[1-9][0-9]*).([^.]+)$', re.I)
MIMES = {'svg': 'image/svg', 'png': 'image/png', 'jpg': 'image/jpeg', 'jpeg':'image/jpeg', 'gif': 'image/gif', 'pdf': 'application/pdf'}

def name_to_mime(filename):
	default = 'application/octet-stream'
	m = re_img.match(filename)
	if m:
		return MIMES.get(m.group(3), default)
	return default

def copy_pictures(temp_dir, out_dir, pic_prefs='svg,png,jpg,jpeg,gif'):
	vals = {}
	for i, x in enumerate(reversed(pic_prefs.split(','))):
		vals[x] = i

	def ext_to_value_key(filename):
		ext = filename[-3:].lower()
		return vals.get(ext, -1)

	diags = {}
	imgs = {}
	lst = sorted(os.listdir(temp_dir))
	lst = filter(lambda x: ext_to_value_key(x) >= 0, lst)
	lst = sorted(lst, key=ext_to_value_key)
	for x in lst:
		m = re_img.match(x)
		if m:
			kind, key, ext = m.groups()
			if kind == 'diag':
				diags[key] = x
			elif kind == 'img':
				imgs[key] = x
			shutil.copy2(os.path.join(temp_dir, x), out_dir)
	return diags, imgs

def pygmentize(lang, code):
	try:
		from pygments import highlight
		from pygments.lexers import get_lexer_by_name
		from pygments.formatters import HtmlFormatter
	except ImportError:
		debug('Could not format code, please install the pygments package!')
		return ''
	else:
		lexer = get_lexer_by_name(lang, stripall=True)
		formatter = HtmlFormatter(linenos=False, cssclass="pygments_code")
		result = highlight(code, lexer, formatter)
		return result

def pygmentize_css():
	try:
		from pygments.formatters import HtmlFormatter
	except ImportError:
		debug('Could not format code, please install the pygments package!')
		return ''
	else:
		return HtmlFormatter().get_style_defs('.pygments_code')

class Node(object):
	def __init__(self, bind):
		if not bind:
			debug("error!! node takes a parameter")
			sys.exit(1)
		self.ptr = bind
		self.vars = {}

	def get_val(self, s):
		#debug("get_val called %s" % s)
		if s == "type":
			ret = sembind.get_val(self.ptr, s)
			if ret == '3': return 'text'
			if ret == '4': return 'img'
			if ret == '5': return 'diag'
			if ret == '6': return 'table'
			return ret
		return sembind.get_val(self.ptr, s)

	def get_var(self, s, default=''):
		if not self.vars:
			hints = sembind.get_val(self.ptr, "hints")
			self.vars = read_properties(hints)
		return self.vars.get(s, default)

	def child_count(self):
		#debug(str(self.ptr))
		return sembind.child_count(self.ptr)

	def child_num(self, num):
		v = sembind.child_num(self.ptr, num)
		return Node(v)

	def num_rows(self):
		return sembind.num_rows(self.ptr)

	def num_cols(self):
		return sembind.num_cols(self.ptr)

	def get_cell(self, row, col):
		return sembind.get_cell(self.ptr, row, col)

	def link_count(self):
		# FIXME
		return 0

	def link_caption(self, idx):
		# FIXME
		return "SEMANTIK: link_caption is missing"

	def link_location(self, idx):
		# FIXME
		return "SEMANTIK: link_location is missing"

def Root():
	return Node(sembind.instance())

