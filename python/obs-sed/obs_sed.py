#!/usr/bin/env python

import sys
import os
import re
import argparse
import hashlib
import subprocess
import signal
import time
import copy
import abc

from hashlib import pbkdf2_hmac

########################### Usage ###########################
def usage():
    usage_info = '''
Usage:
    {dest} [subcommand] [<options>] [blkdev | all]
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
    {NODE} take_ownership [blkdev | all]
Opts:
    -h, --help  Display this help
    '''
    print(usage_info.format(NODE = sys.argv[0]))
    sys.exit(1)

def node_ro_usage():
    usage_info = '''\
Usage:
    {NODE} release_ownership [blkdev | all]
Opts:
    -h, --help  Display this help
    '''
    print(usage_info.format(NODE = sys.argv[0]))
    sys.exit(1)

def node_lr_usage():
    usage_info = '''\
Usage:
    {NODE} lock_range [blkdev | all]
Opts:
    -h, --help  Display this help
    '''
    print(usage_info.format(NODE = sys.argv[0]))
    sys.exit(1)

def node_ur_usage():
    usage_info = '''\
Usage:
    {NODE} unlock_range [blkdev | all]
Opts:
    -h, --help  Display this help
    '''
    print(usage_info.format(NODE = sys.argv[0]))
    sys.exit(1)

def node_llr_usage():
    usage_info = '''\
Usage:
    {NODE} list_locking_range [blkdev | all]
Opts:
    -h, --help  Display this help
    '''
    print(usage_info.format(NODE = sys.argv[0]))
    sys.exit(1)

########################### Class ###########################
### Node ###
class Node:
    def __init__(self, tpm=None):
        self._info = '123'
        self.__master_key = None
        self.tpm = tpm
        self.sedisks = list()           # all SED on the system
        self.sedisks_pending = list()   # all SED needed to process
        self.sc_opts = {
            #subcmd               #usage              #shortpts       #longopts
            'take_ownership':     [node_to_usage,     'h',            ['help']],
            'release_ownership':  [node_ro_usage,     'h',            ['help']],
            'lock_range':         [node_lr_usage,     'h',            ['help']],
            'unlock_range':       [node_ur_usage,     'h',            ['help']],
            'list_locking_range': [node_llr_usage,    'h',            ['help']]
        }
        self.fetch_sedisks()

    def __str__(self):
        return '{}()'.format(self.__class__.__name__)

    def derive_key(self, seed, salt, iterations = 100000):
        key_bytes = pbkdf2_hmac('sha256', seed.encode(), salt.encode(), iterations, dklen=32)
        return key_bytes.hex()

    def gen_master_key(self):
        seed = self.tpm.getrandom()
        self.__master_key = self.derive_key(seed, self.info)

    def gen_auth_key(self, disk_sn):
        return self.derive_key(self.__master_key, disk_sn)

    def is_valid_sed(self, blkdev):
        cmd = 'sedutil-cli --isValidSED %s' % blkdev
        result = exeshell(cmd)
        match = re.search(r'\bSED\b', result)
        return True if match else False

    def is_in_sedisks(self, sedisk):
        return sedisk.blkdev in [sd.blkdev for sd in self.sedisks]

    def fetch_sedisks(self):
        cmd = "cs_hal list disks"
        result = exeshell(cmd)
        lines = result.splitlines()

        for line in lines:
            if line.startswith('/dev/sg'):
                blkdev = line.split()[1]
                if self.is_valid_sed(blkdev):
                    sn = line.split()[-2]
                    self.sedisks.append(SED(blkdev, sn))
        #print([sd for sd in self.sedisks])

    def fetch_sedisks_pending(self, blkdev):

        if blkdev == 'all':
            self.sedisks_pending = copy.deepcopy(self.sedisks)

        for index, sedisk in enumerate(self.sedisks):
            if blkdev == sedisk.blkdev:
                self.sedisks_pending.append(self.sedisks[index])
                break
        else:
            print(
                f"The value of blkdev({blkdev}) is neither all "
                f"nor this value is in the SED disks({[str(sd) for sd in self.sedisks]})"
            )
            usage()

    def tpm_create_space(self):
        self.tpm.create_space()

    def tpm_destroy_space(self):
        self.tpm.destroy_space()

    def tpm_read_key(self):
        try:
            self.__master_key = self.tpm.read_key()
        except TPMKeyNotFoundError:
            raise TPMKeyNotFoundError()

    def tpm_write_key(self):
        self.tpm.write_key(self.__master_key)

    @property
    def info(self):
        return self._info

    @info.setter
    def info(self, value):
        if not value:
            raise ValueError("node_info cannot be empty")
        self._info = value

### TPM ###
class TPMKeyNotFoundError(Exception):
   """ Raised when the key is not found in TPM """
   pass

class TPM:
    def __init__(self, nvindex = 0x1500016):
        self.nvindex = nvindex

    def create_space(self):
        ''' Define a 32-byte NV index '''
        cmd = 'tpm2_nvdefine 0x%x -C o -s 32 -a "ownerread|ownerwrite"' % self.nvindex
        exeshell(cmd)

    def destroy_space(self):
        cmd = 'tpm2_nvundefine 0x%x' % self.nvindex
        exeshell(cmd)

    def read_key(self):
        ''' Read the key to TPM '''
        cmd = 'tpm2_nvread 0x%x -C o -s 32 | xxd -p -c 64' % self.nvindex
        o = exeshell(cmd)
        if re.search(r'\bError\b', o):
            raise TPMKeyNotFoundError(f"Key is not found in the TPM")
        return o

    def write_key(self, key):
        ''' Write the key to TPM '''
        cmd = 'echo -n %s | xxd -r -p | tpm2_nvwrite 0x%x -C o -i -' % (key, self.nvindex)
        exeshell(cmd)

    def getrandom(self):
        ''' Get a random number generated by TPM '''
        return exeshell("tpm2_getrandom --hex 32");

### abc SEDCLI ###
class SEDCLI(metaclass=abc.ABCMeta):
    def __init__(self):
        pass

    @abc.abstractmethod
    def take_ownership(self, auth_key, blkdev):
        pass

    @abc.abstractmethod
    def release_ownership(self, auth_key, blkdev):
        pass

    @abc.abstractmethod
    def lock_range(self, auth_key, blkdev):
        pass

    @abc.abstractmethod
    def unlock_range(self, auth_key, blkdev):
        pass

    @abc.abstractmethod
    def list_locking_range(self, auth_key, blkdev):
        pass

### SEDUtil-Cli ###
class SEDUtil(SEDCLI):

    def __init__(self):
        super().__init__()

    def take_ownership(self, auth_key, blkdev):
        cmd = "sedutil-cli --initialSetup %s %s" % (auth_key, blkdev)
        exeshell(cmd)

    def release_ownership(self, auth_key, blkdev):
        cmd = "sedutil-cli --revertTPer %s %s" % (auth_key, blkdev)
        exeshell(cmd)

    def lock_range(self, auth_key, blkdev):
        cmd = "sedutil-cli --setLockingRange 0 LK %s %s" % (auth_key, blkdev)
        exeshell(cmd)

    def unlock_range(self, auth_key, blkdev):
        cmd = "sedutil-cli --setLockingRange 0 RW %s %s" % (auth_key, blkdev)
        exeshell(cmd)

    def list_locking_range(self, auth_key, blkdev):
        cmd = "sedutil-cli --listLockingRange 0 %s %s" % (auth_key, blkdev)
        return exeshell(cmd)

### SEDLib ###
class SEDLib(SEDCLI):
    pass

### SED ###
class SED:

    def __init__(self, blkdev=str(), sn=str(), sedcli=SEDUtil()):
        self.blkdev = blkdev
        self.sn = sn
        self.sedcli = sedcli

    def __str__(self):
        return '{}:{}'.format(self.blkdev, self.sn)

    def take_ownership(self, auth_key):
        self.sedcli.take_ownership(auth_key, self.blkdev)

    def release_ownership(self, auth_key):
        self.sedcli.release_ownership(auth_key, self.blkdev)

    def lock_range(self, auth_key):
        self.sedcli.lock_range(auth_key, self.blkdev)

    def unlock_range(self, auth_key):
        self.sedcli.unlock_range(auth_key, self.blkdev)

    def list_locking_range(self, auth_key):
        return self.sedcli.list_locking_range(auth_key, self.blkdev)

########################### Utility ###########################
def handler(signum, frame):
    print("\nCtrl-C detected, but ignored. Do not exit until the program completes")

def exeshell(cmd, check=True):

    # Save old SIGINT handler
    old_handler = signal.getsignal(signal.SIGINT)

    try:
        # Ignore Ctrl-C while subprocess starts
        signal.signal(signal.SIGINT, handler)

        # Run subprocess in a new session (isolates signal group)
        result = subprocess.run(cmd, shell=True,
                    capture_output=True, text=True, start_new_session=True)
    finally:
        # Restore normal Ctrl-C handling
        signal.signal(signal.SIGINT, old_handler)

    # Collect output and return code
    s = result.returncode
    o = result.stdout.strip() if result.stdout else result.stderr.strip()

    # Raise exception if check is True and command failed
    if check and s:
        raise SystemExit(f"Execute command '{cmd}' error: code={s}, {o}")

    return o

########################### Subcommand ###########################
def set_args(parser: argparse.ArgumentParser, node: Node):

    subparsers = parser.add_subparsers(dest='subcommand', help='Subcommand')
    for item in node.sc_opts.items():
        subcmd, subfunc = item[0], item[1][0]
        subparser = subparsers.add_parser(subcmd, help=subfunc)
        subparser.add_argument('blkdev', help='blkdev')

    # Just for test
    parser_rel = subparsers.add_parser('test', help='Restore drive to factory default state')
    parser_rel.add_argument('blkdev', help='blkdev')

def chk_args(parser: argparse.ArgumentParser, node: Node):
    args = parser.parse_args()
    if not args.subcommand:
        usage()

def system_chker():
    # Check if the system has TPM
    try:
        exeshell('ls /sys/class/tpm/')
    except SystemExit as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
        usage()

def do_subcmd(subcmd, node):

    for sedisk in node.sedisks_pending:
        auth_key = node.gen_auth_key(sedisk.sn)
        print(f"SED:{sedisk} auth_key:{auth_key}")

        match subcmd:
            case 'take_ownership':
                sedisk.take_ownership(auth_key)
            case 'release_ownership':
                sedisk.release_ownership(auth_key)
                node.tpm.destroy_space()
            case 'lock_range':
                sedisk.lock_range(auth_key)
            case 'unlock_range':
                sedisk.unlock_range(auth_key)
            case 'list_locking_range':
                print(sedisk.list_locking_range(auth_key))
            case 'test': # Just for test
                print(f"Test")
            case _:
                print(f"Unknown subcommand: {args.subcommand}")
                usage()

########################### Main Entry ###########################
def main():

    system_chker()

    node = Node(TPM())

    parser = argparse.ArgumentParser()
    set_args(parser, node)
    chk_args(parser, node)

    blkdev = parser.parse_args().blkdev
    node.fetch_sedisks_pending(blkdev)

    try:
        node.tpm_read_key();
    except:
        print("Generate master key......")
        node.gen_master_key()
        node.tpm_create_space()
        node.tpm_write_key()
    finally:
        subcmd = parser.parse_args().subcommand
        do_subcmd(subcmd, node)

    return 0;

if __name__ == '__main__':
    sys.exit(main())
