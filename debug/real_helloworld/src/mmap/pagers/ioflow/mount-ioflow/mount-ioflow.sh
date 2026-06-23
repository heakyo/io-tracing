#! /usr/local/bin/bash

set -e

DEV=/dev/md0
MNT=ufs2demo_mntdir
FILE=$MNT/myfirstfile
MP=$(mount | awk -v dev="$DEV" '$1 == dev {print $3}')
echo "MP:$MP"

tunefs_ufs()
{
	tunefs -p $1 2>&1 | grep "soft updates"
	tunefs -n disable $1
	tunefs -p $1 2>&1 | grep "soft updates"
}

show_fsid()
{
	dumpfs $1 | grep -w id | sed 's/.*\(id.*\]\).*/\1/'
}

# Device: /dev/ada0p20
ufs2demo_mount()
{
	echo "mount $1 $MNT"
	./mount-ioflow.d -c "mount $1 $MNT"

	echo "show FSID"
	show_fsid $MNT

	echo "umount $1"
	umount $1
}

#tunefs_ufs $DEV

if [ -n "$MP" ]; then
    echo "$DEV is mounted on $MP"
    umount "$DEV"
fi

ufs2demo_mount $DEV
