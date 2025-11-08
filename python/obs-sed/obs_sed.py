#!/usr/bin/env python

import sys
import os
import re
import argparse
import hashlib
import subprocess

from hashlib import pbkdf2_hmac

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

def main():

    tpm2_nvindex = 0x1500016
    master_key=''

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='subcommand', help='Subcommand')

    # take_ownership
    parser_take = subparsers.add_parser('take_ownership', help='Claim drive for use by node')
    parser_take.add_argument('device', help='device')

    # release_ownership
    parser_rel = subparsers.add_parser('release_ownership', help='Restore drive to factory default state')
    parser_rel.add_argument('device', help='device')

    # test
    parser_rel = subparsers.add_parser('test', help='Restore drive to factory default state')
    parser_rel.add_argument('device', help='device')

    args = parser.parse_args()
    if not args.subcommand:
        usage()

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
        print(f"tpm cmd:{cmd}")
        master_key = exeshell(cmd)
        print(f"master_key:{master_key} type:{type(master_key)}")

    if args.subcommand == 'release_owership':
        cmd = 'tpm2_nvundefine 0x%x' % tpm2_nvindex
        exeshell(cmd)

    if args.subcommand == 'take_ownership':
        device = args.device
        if not device == 'all' and not device in sed_disks :
            print(
                f"The value of device({device}) is either all,"
                f"or this value is in the SED disks({sed_disks.keys()})"
            )
            usage()

        auth_key = get_auth_key(master_key, sed_disks.get(device))

        print(f"sed_disks.get(device): {sed_disks.get(device)}")
        print(f"auth_key:{auth_key} type:{type(auth_key)}")

        take_ownership(device, auth_key)

    elif args.subcommand == 'release_ownership':
        device = args.device
        if not device == 'all' and not device in sed_disks :
            print(
                f"The value of device({device}) is either all,"
                f"or this value is in the SED disks({sed_disks.keys()})"
            )
            usage()

        auth_key = get_auth_key(master_key, sed_disks.get(device))
        release_ownership(device, auth_key)

        cmd = 'tpm2_nvundefine 0x%x' % tpm2_nvindex
        print(f"tpm cmd:{cmd}")
        exeshell(cmd)
    elif args.subcommand == 'test':
        device = args.device
        auth_key = get_auth_key('40c818669e6b605d0b65af52e59177857bb3f72ed4bcf216495e6bc8a4eb\n8eca', sed_disks.get(device))
        print(f"auth_key:{auth_key} type:{type(auth_key)}")
    else:
        print(f"Unknown subcommand: {args.subcommand}")
        usage()

    return 0;

if __name__ == '__main__':
    sys.exit(main())
