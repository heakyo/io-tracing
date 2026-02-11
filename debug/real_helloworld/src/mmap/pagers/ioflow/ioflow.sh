#! /bin/bash

tunefs -p /dev/md9p1 2>&1 | grep "soft updates"
tunefs -n disable /dev/md9p1
tunefs -p /dev/md9p1 2>&1 | grep "soft updates"
#tunefs -p /dev/md11 2>&1 | grep "soft updates"

ufs2demo_rm()
{
	#mount /dev/md11 ufs2demo_mntdir
	mount /dev/md9p1 ufs2demo_mntdir
	./main
	ls -l ufs2demo_mntdir
	./ioflow.d -c 'rm -rf ufs2demo_mntdir/myfirstfile'
	ls -l ufs2demo_mntdir
	umount ufs2demo_mntdir
}

ufs2demo_wr()
{
	mount /dev/md9p1 ufs2demo_mntdir
	ls -l ufs2demo_mntdir
	./wrioflow.d -c './main'
	ls -l ufs2demo_mntdir
	umount ufs2demo_mntdir
}

ufs2demo_rd()
{
	mount /dev/md9p1 ufs2demo_mntdir
	./main
	#./rdioflow.d -c './main'
	umount ufs2demo_mntdir
}

ufs2demo_rd
#ufs2demo_wr
#ufs2demo_rm
