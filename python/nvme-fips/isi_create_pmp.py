#!/usr/bin/env python

import sys

import isi.sys.disk as disk
import isi.sys.geom as geom

import mydisk

mesh = geom.Mesh()

def main():
	devices = ["da2"]
	dev = devices[0]
	d_provider = mesh.disk_provider(dev)
	print(d_provider)

	partition_g = d_provider.partition_geom
	print(partition_g)

	if 1 and (partition_g is None or len(partition_g.providers)) > 0:

		print(partition_g.providers)

		disk.wipe_disk_provider(d_provider)
		disk.fix_gpt(d_provider)
		partition_g = d_provider.partition_geom

	size = 20 * (1024 ** 3)
	#disk.make_partition(partition_g, "isilon-pmp", geom.GPT_ENT_TYPE_ISILON_PMP, size)
	mydisk.make_partition(partition_g, "isilon-pmp", geom.GPT_ENT_TYPE_ISILON_PMP, size)

if __name__ == "__main__":
	sys.exit(main())
