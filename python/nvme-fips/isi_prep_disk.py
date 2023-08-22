#!/usr/bin/env python

import argparse
import ctypes
import logging
import os
import sys

import isi.app.lib.procs as procs
import isi.hw.bay as bay
import isi.sys.disk as disk

from icecream import ic

class IsiPrepDisk(object):

	def __init__(self):
		self.drives = []

	def process_args(self):
		failure = 0;

		parser = argparse.ArgumentParser()
		parser.add_argument("disks", nargs="+")
		parser.set_defaults(disks=[])
		#print(parser)

		args = parser.parse_args()
		#ic(args) # Namespace(disks=['da2'])

		# grab device paths
		for drive_str in args.disks:
			b = bay.any2bay(drive_str)
			#ic(drive_str) # 'da2'
			#ic(b) # 2 -- bay number
			if b < 0:
				self.logger.error(
					"Error: unknown bay for device %s; errno %d",
					drive_str,
					ctypes.get_errno(),
				)
			dev = bay.bay2dev(b) # 'da2'
			#ic(dev)
			if dev:
				dev = dev.strip()
				if not dev.startswith("/dev"):
					dev = "/dev/" + dev
			# ic(dev)

			if not dev or not os.path.exists(dev):
				self.logger.error(
					"Error: device %s (%d:%s) does not exist",
					drive_str,
					b,
					dev,
				)
				sys.exit(1)

			self.drives.append(dev)

	def start_logging(self):
		log_file = "/var/log/isi_drive_d.log"
		hdlr = logging.FileHandler(log_file)
		formatter = logging.Formatter("%(asctime)s %(process)d %(message)s")
		hdlr.setFormatter(formatter)
		self.logger = logging.getLogger(name="isi_prep_disk")
		self.logger.addHandler(hdlr)

		self.logger.setLevel(logging.INFO) # the log can be shown in the /var/log/isi_drive_d.log

	def main(self):
		self.start_logging()
		try:
			self.process_args()

			self.logger.info("Prepping gpt/gmirror drives %s", self.drives)
			for dev in self.drives:
				_, drive = os.path.split(dev)
				#ic(os.path.split(dev)) # ('/dev', 'da2')
				#ic(type(drive), drive) # <class 'str'>, 'da2'

				#ic(disk.uses_disklabel(drive)) # False
				if disk.uses_disklabel(drive):
					cmd = "/usr/sbin/isi_wipe_disk %s" % drive
					(err, out) = procs.get_cmd_output(cmd)
					if err:
						self.logger.error("Error from wipe_disk: %s", err)
						sys.exit(1)

				ret = disk.prep_disk(drive)

			if len(self.drives) == 1:
				sys.exit(ret)
			sys.exit(0)

		except Exception:
			self.logger.exception("")
			raise

if __name__ == "__main__":
	IsiPrepDisk().main()
