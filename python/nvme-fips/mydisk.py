import os
import isi.sys.geom as geom
import isi.sys.psi as syspsi

import isi.sys.disk as disk

from icecream import ic

SCRATCH_ALIAS = "s1b"
IFS_ALIAS = "s1e"
NEWFS_WAIT_FILE = "/etc/ifs/isi_newfs_wait"

mesh = geom.Mesh()

def basename(s):
	return os.path.split(s)[-1]

def find_partition(partition_geom, label):
	for p in partition_geom.providers:
		if p.label == label:
			return p
	return None

def make_partition(partition_geom, label, uuid, size, start=-1, zero_partition=False):
	# Create the partition if it doesn't exist.
	ic(partition_geom, label) # PART:da2 isilon-pmp
	ic(partition_geom.providers) # [PART:da2:da2p2, PART:da2:da2p1]

	partition_provider = find_partition(partition_geom, label)
	ic(partition_provider)

	if not partition_provider:
		partition_provider = partition_geom.add_partition(
			uuid, size, label, start=start, zero=zero_partition
		)
	return partition_provider

def prep_disk(diskname):
	return pre_newfs_disk(diskname)

# Split leading /dev/ or trailing s1e.
def pre_newfs_disk(diskname):
	"""Do whatever is necessary before running newfs_efs."""
	diskname = basename(diskname)
	if diskname.endswith(IFS_ALIAS):
		ic(diskname)
		diskname = diskname[: -len(IFS_ALIAS)]
	ic(diskname)
	mesh.refresh()
	dp = mesh.disk_provider(diskname)
	ic(dp)
	ret = pre_newfs_disk_provider(dp)
	return ret

def pre_newfs_disk_provider(dp, force=False):
	supportsSEDs = syspsi.supports_self_encrypting_drives()
	ic(supportsSEDs)

	# ic(disk.is_properly_formatted_storage_drive(dp))
	if not disk.is_properly_formatted_storage_drive(dp) or supportsSEDs or force:

		#ic(disk.is_removable(dp.name)
		if disk.is_removable(dp.name) and not force:
			return 1

		pg = dp.partition_geom
		#ic(type(dp))
		if pg:
			#ic(pg.providers)
			pps = [pp for pp in pg.providers
					if (
						hasattr(pp, "label") and
						(pp.label == "kerneldump" or (pp.label == "journal" and not force))
					)
				]
			ic(pps)
			if len(pps) and not syspsi.bootstrap_supports_distmirror():
				return 0;

		if supportsSEDs:
			ret = sed.release_ownership(dp.name)
			if ret not in [
				sed.SED_RESULT_SUCCESS,
				sed.SED_RESULT_KEY_NOT_FOUND,
				sed.SED_RESULT_DRIVE_UNOWNED,
			]:
				return 1

		if syspsi.bootstrap_supports_distmirror():
			skip_list = [
			"root0",
			"root1",
			"var0",
			"var1",
			"journal-backup",
			"jbackup-peer",
			"mfg",
			"kernelsdump",
			"var-crash",
			"kerneldump",
			"keystore",
			"hw",
			]
			disk.wipe_disk_provider(dp, skip=skip_list)
		else:
			disk.wipe_disk_provider(dp)

		partition_geom = dp.partition_geom or dp.create_partition_geom()
		ic(partition_geom)

		ifs_start, ifs_size = disk.ifs_aligned_start_size(partition_geom)
		ic(ifs_start, ifs_size)

		scratch_start, scratch_size = disk.scratch_aligned_start_size(partition_geom)
		ic(scratch_start, scratch_size)
		ic(dp.name, dp.sectorsize)

		if supportsSEDs:
			band1_start = scratch_start
			band2_start = ifs_start

			band1_size = band2_start - band1_start
			band2_size = ifs_size

			ic(band1_start, band1_size)
			ic(band2_start, band2_size)

			ret = sed.take_ownership(
				dp.name, band1_start, band1_size, band2_start, band2_size
			)

			if ret != sed.SED_RESULT_SUCCESS:
				return 1

			if not os.path.exists(NEWFS_WAIT_FILE):
				ret = sed.probe_band0(dp.name)
			if ret == sed.SED_RESULT_BAND_ERASE_REQUIRED:
			# Isi_sed returns Erase-Required to indicate that Band0 needs erased
				ret = sed.band0_erase(dp.name)
			if ret != sed.SED_RESULT_SUCCESS:
				return 1

		return disk.init_disk_provider(dp, scratch_start, scratch_size, ifs_start, ifs_size)
