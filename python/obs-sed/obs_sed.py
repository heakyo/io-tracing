#!/usr/bin/env python

import sys
import os
import re
import argparse
import hashlib
import subprocess

from hashlib import pbkdf2_hmac

########################### Usage ###########################
def usage():
    usage_info = '''
Usage:
    {dest} [subcommand] [<options>] [device | all]
Subcommand:
    take_ownership          Claim drive for use by node
    release_ownership       Restore drive to factory default state
Options:
    --psid [PSID]           PSID value
    '''
    print(usage_info.format(dest=sys.argv[0]))
    sys.exit(1)

def node_to_usage():
    usage_info = '''\
Usage:
    {NODE} take_ownership [device | all]
Opts:
    -h, --help  Display this help
    '''
    print(usage_info.format(NODE = sys.argv[0]))
    sys.exit(1)

def node_ro_usage():
    usage_info = '''\
Usage:
    {NODE} release_ownership [device | all]
Opts:
    -h, --help  Display this help
    '''
    print(usage_info.format(NODE = sys.argv[0]))
    sys.exit(1)

########################### Class ###########################
class Node:
    def __init__(self):
        self._info = ''
        self.sc_opts = {
            #subcmd               #usage              #shortpts       #longopts
            'take_ownership':     [node_to_usage,     'h',            ['help']],
            'release_ownership':  [node_ro_usage,     'h',            ['help']]
        }

    def __str__(self):
        return '{}()'.format(self.__class__.__name__)

    @property
    def info(self):
        return self._info

    @info.setter
    def info(self, value):
        if not value:
            raise ValueError("node_info cannot be empty")
        self._info = value

class TPM:
    pass

class SED:

    def __init__(self, blkdev):
        self.blkdev = blkdev

    def take_ownership(self, blkdev, auth_key):
        print(f"Taking ownership with auth_key:{auth_key}")
        cmd = "sedutil-cli --initialSetup %s %s" % (auth_key, blkdev)
        print(f"cmd:{cmd}")
        exeshell(cmd)

    def release_ownership(self, blkdev, auth_key):
        print(f"Releasing ownership")
        cmd = "sedutil-cli --revertTPer %s %s" % (auth_key, blkdev)
        print(f"cmd:{cmd}")
        exeshell(cmd)

########################### Utility ###########################
def exeshell(cmd, check=True):
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    s = result.returncode
    o = result.stdout.strip() if result.stdout else result.stderr.strip()

    if check and s != 0:
        raise SystemExit(f"Execute command '{cmd}' error: code={s}, {o}")

    return o

def derive_key(seed: str, salt: str, iterations: int = 100000):
    key_bytes = pbkdf2_hmac('sha256', seed.encode(), salt.encode(), iterations, dklen=32)
    return key_bytes.hex()

def get_master_key(node_info: str):
    tpm_random = exeshell("tpm2_getrandom --hex 32");
    master_key = derive_key(tpm_random, node_info)
    return master_key

def get_auth_key(master_key: str, disk_sn: str):
    auth_key = derive_key(master_key, disk_sn)
    return auth_key

def set_master_key(nvindex: str):
    return

def isValidSED(blkdev: str):
    cmd = 'sedutil-cli --isValidSED %s' % blkdev
    #print("cmd:",cmd);
    result = exeshell(cmd)
    #print("result:", result)
    match = re.search(r'\bSED\b', result)
    return True if match else False

def get_sed_disks():
    cmd = "cs_hal list disks"
    result = exeshell(cmd)
    lines = result.splitlines()
    #print(lines)

    sed_disks = {}
    for line in lines:
        if line.startswith('/dev/sg'):
            parts = line.split()
            #print("parts:", parts)
            block_dev = parts[1]       # e.g. /dev/sdb
            serial_num = parts[-2]     # e.g. X9A0A07TTNTF
            if isValidSED(block_dev):
                sed_disks[block_dev] = serial_num
    return sed_disks

########################### Subcommand ###########################
def take_ownership(device: str, auth_key: str):
    print(f"Taking ownership with auth_key:{auth_key}")

    cmd = "sedutil-cli --initialSetup %s %s" % (auth_key, device)
    print(f"cmd:{cmd}")
    exeshell(cmd)

def release_ownership(device: str, auth_key: str):
    print(f"Releasing ownership")

    cmd = "sedutil-cli --revertTPer %s %s" % (auth_key, device)
    print(f"cmd:{cmd}")
    exeshell(cmd)

def set_args(parser: argparse.ArgumentParser, node: Node):

    subparsers = parser.add_subparsers(dest='subcommand', help='Subcommand')
    for item in node.sc_opts.items():
        subcmd, subfunc = item[0], item[1][0]
        print(subcmd, subfunc)
        subparser = subparsers.add_parser(subcmd, help=subfunc)
        subparser.add_argument('device', help='device')

    # test
    parser_rel = subparsers.add_parser('test', help='Restore drive to factory default state')
    parser_rel.add_argument('device', help='device')

def system_check():
    # Check if the system has TPM
    try:
        exeshell('ls /sys/class/tpm/')
    except SystemExit as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
        usage()

def main():
    node = Node()

    tpm2_nvindex = 0x1500016
    master_key=''

    parser = argparse.ArgumentParser()
    set_args(parser, node)

    args = parser.parse_args()
    if not args.subcommand:
        usage()

    system_check()
    sed_disks = get_sed_disks()
    print("sed_disks:", sed_disks, "disks_list:", sed_disks.keys())

    if args.subcommand == 'take_ownership':
        # Define a 32-byte NV index
        cmd = 'tpm2_nvdefine 0x%x -C o -s 32 -a "ownerread|ownerwrite"' % tpm2_nvindex
        print(f"tpm cmd:{cmd}")
        exeshell(cmd)

        # Generate master key
        master_key = get_master_key('123')
        print(f"master_key:{master_key} type:{type(master_key)}")

        # Write the key to TPM
        cmd = 'echo -n %s | xxd -r -p | tpm2_nvwrite 0x%x -C o -i -' % (master_key, tpm2_nvindex)
        print(f"tpm cmd:{cmd}")
        exeshell(cmd)

    else:
        # Read the key from TPM
        cmd = 'tpm2_nvread 0x%x -C o -s 32 | xxd -p -c 64' % tpm2_nvindex
        #print(f"tpm cmd:{cmd}")
        try:
            master_key = exeshell(cmd)
        except SystemExit as e:
            print(f"Unexpected error: {type(e).__name__}: {e}")
            #print(f"master_key:{master_key} type:{type(master_key)}")

    if args.subcommand == 'release_owership':
        cmd = 'tpm2_nvundefine 0x%x' % tpm2_nvindex
        exeshell(cmd)

    sedisk = SED(args.device)
    if not sedisk.blkdev == 'all' and not sedisk.blkdev in sed_disks :
        print(
            f"The value of device({sedisk.blkdev}) is either all,"
            f"or this value is in the SED disks({sed_disks.keys()})"
        )
        usage()

    auth_key = get_auth_key(master_key, sed_disks.get(sedisk.blkdev))

    if args.subcommand == 'take_ownership':

        print(f"sed_disks.get(device): {sed_disks.get(sedisk.blkdev)}")
        print(f"auth_key:{auth_key} type:{type(auth_key)}")

        take_ownership(sedisk.blkdev, auth_key)

    elif args.subcommand == 'release_ownership':
        release_ownership(sedisk.blkdev, auth_key)

        cmd = 'tpm2_nvundefine 0x%x' % tpm2_nvindex
        print(f"tpm cmd:{cmd}")
        exeshell(cmd)

    elif args.subcommand == 'test':
        node.info = '123'
        print(f"Node:{node.info}")
    else:
        print(f"Unknown subcommand: {args.subcommand}")
        usage()

    return 0;

if __name__ == '__main__':
    sys.exit(main())
