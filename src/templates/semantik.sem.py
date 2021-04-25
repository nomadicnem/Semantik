#sem:name: semantik document generator
#sem:tip: used for saving semantik documents

# Thomas Nagy, 2007-2019 GPLV3

import os, tarfile
import io

tar = tarfile.open(sembind.get_var('outfile'), 'w:gz')

doc = sembind.get_var('fulldoc').encode('utf-8')
stuff = io.BytesIO(doc)
tarinfo = tarfile.TarInfo('con.xml')
tarinfo.size = len(doc)
tar.addfile(tarinfo, stuff)

#debug(doc)

temp_dir = sembind.get_var('temp_dir')
lst = os.listdir(temp_dir)

tmp = str(sembind.get_item_ids()).split(",")
for x in tmp:
	if not x:
		continue
	item = sembind.get_item_by_id(int(x))
	if item:
		pid = sembind.get_val(item, "pic_id")
		s = 'img-%s' % pid
		if pid != '0':
			for pic in lst:
				if pic.startswith(s):
					tar.add(os.path.join(temp_dir, pic), pic)
					break
			else:
				debug("Could not find the picture for item %r->%r in %r" % (x, pid, sembind.get_var('temp_dir')))
	else:
		debug("Could not get an object for %r" % x)
tar.close()

