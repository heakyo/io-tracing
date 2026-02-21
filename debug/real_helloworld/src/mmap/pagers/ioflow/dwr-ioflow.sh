#! /bin/bash

set -e

DEV=/dev/ada0p20
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

# Device: /dev/ada0p20
ufs2demo_wr_delayed()
{
	echo "mount $1 $MNT"
	mount $1 $MNT
	#ls $MNT

	rm -rf $FILE
	#ls $MNT

	./main -p
	#ls $FILE

	echo "umount $MNT"
	./dwr-ioflow.d -c "umount $DEV"
}

#tunefs_ufs $DEV

if [ -n "$MP" ]; then
    echo "$DEV is mounted on $MP"
    umount "$DEV"
fi

ufs2demo_wr_delayed $DEV
