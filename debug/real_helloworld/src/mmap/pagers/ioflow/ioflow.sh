#! /bin/bash

mount /dev/md9 /root/ada_mnt/repo/io-tracing/debug/real_helloworld/src/mmap/pagers/mount/ufsimg
./ioflow.d -c './main'
rm -rf ../mount/ufsimg/myfile
umount /dev/md9

