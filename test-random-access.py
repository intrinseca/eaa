#!/usr/bin/env python

from subprocess import Popen, PIPE, STDOUT, call
import random, sys

def do_read(start, count, hide=True):
	command = ['dd', 'bs=2k', 'if=/dev/sr0', 'skip={0}'.format(start), 'count={0}'.format(count), 'iflag=direct', 'of=/dev/null']

	dd = Popen(command, stdout=PIPE, stderr=STDOUT)
	output = dd.stdout.read()

	if not hide:
		print(output.decode("utf-8"))
	
	return output

def set_speed(speed):
	call(['eject', '-x', str(speed)])

read_buf = 1
#drive_end = int(339296 / 2)
drive_end = 84824

for i in range(50):
	start = random.randint(0, drive_end - read_buf)
	print(start)

	do_read(start, read_buf, hide=False)