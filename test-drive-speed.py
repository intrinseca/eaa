#!/usr/bin/env python

from subprocess import Popen, PIPE, STDOUT, call

def do_read(start, count, hide=True):
	command = ['dd', 'bs=2k', 'if=/dev/sr0', 'skip={0}'.format(start), 'count={0}'.format(count), 'iflag=direct', 'of=/dev/null']

	dd = Popen(command, stdout=PIPE, stderr=STDOUT)
	output = dd.stdout.read()

	if not hide:
		print(output)
	
	return output

def set_speed(speed):
	call(['eject', '-x', str(speed)])



read_buf = 1000;

for i in [1, 12, 24]:
	print("{0}x:".format(i))

	set_speed(i)
	do_read(1, read_buf)
	do_read(10000, 1)
	do_read(0, 1)

	do_read(1, read_buf, hide=False)
	do_read(10000, 1)
	do_read(0, 1)