#! /bin/bash

mount /dev/md9 /root/ada_mnt/repo/io-tracing/debug/real_helloworld/src/mmap/pagers/mount/ufsimg
./main
hexdump -n 32 -C ../mount/ufsimg/myfile
ls -l ../mount/ufsimg
./ioflow.d -c 'rm ../mount/ufsimg/myfile'
#rm ../mount/ufsimg/myfile
ls -l ../mount/ufsimg
umount /dev/md9

