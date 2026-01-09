#! /bin/bash

tunefs -p /dev/md10 2>&1 | grep "soft updates"
tunefs -n disable /dev/md10
tunefs -p /dev/md10 2>&1 | grep "soft updates"

mount /dev/md10 ufs2demo_mntdir
ls -l ufs2demo_mntdir
./ioflow.d -c './main'
ls -l ufs2demo_mntdir
rm -rf ufs2demo_mntdir/myfirstfile
umount /dev/md10

