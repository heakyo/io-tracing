#!/bin/sh
# Create an identity mapping for a device
echo "0 `blockdev --getsz $1` linear $1 0" | dmsetup create identity
