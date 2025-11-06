#!/usr/bin/env python

import sys
import os
import argparse
import hashlib
import subprocess

from hashlib import pbkdf2_hmac

def usage():
    usage_info = '''
Usage:
    {dest} [subcommand] [<options>] [seed] [salt]
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

def take_ownership(auth_key: str):
    cmd = "sedutil-cli --initialSetup %s /dev/sdc" % auth_key

    print(f"Taking ownership with auth_key:{auth_key}")
    print(f"cmd:{cmd}")
    exeshell(cmd)

def release_ownership():
    print(f"Releasing ownership")

def main():

    #parser = argparse.ArgumentParser(
    #    usage='%(prog)s [subcommand] [<options>] [seed] [salt]',
    #    description='Example SED management tool.'
    #)

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='subcommand', help='Subcommand')

    # take_ownership
    parser_take = subparsers.add_parser('take_ownership', help='Claim drive for use by node')
    parser_take.add_argument('seed', help='Seed value')
    parser_take.add_argument('salt', help='Salt value')

    args = parser.parse_args()
    if not args.subcommand:
        usage()
        sys.exit(1)

    if args.subcommand == 'take_ownership':
        seed = args.seed
        salt = args.salt
        print(f"Seed: {seed} (type: {type(seed)})")

        master_key = derive_key(seed, salt)
        print(f"seed:{seed} salt:{salt} master_key:{master_key}")
        print(f"master_key:{master_key} type:{type(master_key)}")

        drv_sn = "12"
        auth_key = derive_key(master_key, drv_sn)
        print(f"auth_key:{auth_key} type:{type(auth_key)}")
        #take_ownership(auth_key)
    else:
        print(f"Unknown subcommand: {args.subcommand}")
        usage()
        sys.exit(1)

    return 0;

if __name__ == '__main__':
    sys.exit(main())
