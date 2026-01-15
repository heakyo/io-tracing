#! /bin/bash

tunefs -p /dev/md10 2>&1 | grep "soft updates"
tunefs -n disable /dev/md10
tunefs -p /dev/md10 2>&1 | grep "soft updates"
#tunefs -p /dev/md11 2>&1 | grep "soft updates"

ufs2demo_rm()
{
	#mount /dev/md11 ufs2demo_mntdir
	mount /dev/md10 ufs2demo_mntdir
	./main
	ls -l ufs2demo_mntdir
	./ioflow.d -c 'rm -rf ufs2demo_mntdir/myfirstfile'
	ls -l ufs2demo_mntdir
	umount ufs2demo_mntdir
}

ufs2demo_rm
