#!/bin/sh
# Join 2 devices together
size1=`blockdev --getsz $1`
size2=`blockdev --getsz $2`
echo "0 $size1 linear $1 0
$size1 $size2 linear $2 0" | dmsetup create joined
