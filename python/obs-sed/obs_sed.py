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

########################### Usage Info ###########################
def usage():
    argparse.ArgumentParser()
    sys.exit(1)

def node_to_usage():
    usage_info = 'Claim drive for use by node'
    return usage_info

def node_ro_usage():
    usage_info = 'Restore drive to factory default state'
    return usage_info

def node_lr_usage():
    usage_info = 'Lock drive ranges, preventing access'
    return usage_info

def node_ur_usage():
    usage_info = 'Unlock drive ranges using authentication'
    return usage_info

def node_llr_usage():
    usage_info = 'List drive ranges'
    return usage_info

def node_rfd_usage():
    usage_info = '*Revert to factory default state --- TODO'
    return usage_info

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
            #subcmd                 #usage              #option
            'take_ownership':       [node_to_usage(),   {
                                                          'blkdev': ['block device (e.g. /dev/sda)', False]
                                                        }],
            'release_ownership':    [node_ro_usage(),   {
                                                          'blkdev': ['block device (e.g. /dev/sda)', False]
                                                        }],
            'lock_range':           [node_lr_usage(),   {
                                                          'blkdev': ['block device (e.g. /dev/sda)', False]
                                                        }],
            'unlock_range':         [node_ur_usage(),   {
                                                          'blkdev': ['block device (e.g. /dev/sda)', False]
                                                        }],
            'list_locking_range':   [node_llr_usage(),  {
                                                          'blkdev': ['block device (e.g. /dev/sda)', False]
                                                        }],
            'revert_default':       [node_rfd_usage(),  {
                                                          'blkdev': ['block device (e.g. /dev/sda)', False],
                                                          '--psid': ['PSID value',                   True]
                                                        }]
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

    # TPM Specific
    def tpm_create_space(self):
        self.tpm.create_space()

    def tpm_destroy_space(self):
        self.tpm.destroy_space()

    def tpm_read_key(self):
        try:
            self.__master_key = self.tpm.read_key()
        except TPMKeyNotFoundError:
            raise TPMKeyNotFoundError(f"Key is not found in the TPM")

    def tpm_write_key(self):
        self.tpm.write_key(self.__master_key)

    # SED Specific
    def take_ownership(self, sedisk):
        sedisk.sedcli.take_ownership(sedisk.get_auth_key(), sedisk.blkdev)

    def release_ownership(self, sedisk):
        sedisk.sedcli.release_ownership(sedisk.get_auth_key(), sedisk.blkdev)

    def lock_range(self, sedisk):
        sedisk.sedcli.lock_range(sedisk.get_auth_key(), sedisk.blkdev)

    def unlock_range(self, sedisk):
        sedisk.sedcli.unlock_range(sedisk.get_auth_key(), sedisk.blkdev)

    def list_locking_range(self, sedisk):
        sedisk.sedcli.list_locking_range(sedisk.get_auth_key(), sedisk.blkdev)

    @property
    def info(self):
        return self._info

    @info.setter
    def info(self, value):
        if not value:
            raise ValueError("node_info cannot be empty")
        self._info = value

    def do_subcmd(self, subcmd):
        # SED Specific
        for sedisk in self.sedisks_pending:
            sedisk.set_auth_key(self.gen_auth_key(sedisk.sn))
            print(f"SED:{sedisk} auth_key:{sedisk.get_auth_key()}")
            getattr(self, subcmd)(sedisk)

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
        print(exeshell(cmd))

### SEDLib -- TODO ###
class SEDLib(SEDCLI):
    pass

### SED ###
class SED:

    def __init__(self, blkdev=str(), sn=str(), sedcli=SEDUtil()):
        self.blkdev = blkdev
        self.sn = sn
        self.sedcli = sedcli
        self.__auth_key = str()

    def __str__(self):
        return '{}:{}'.format(self.blkdev, self.sn)

    def set_auth_key(self, auth_key):
        self.__auth_key = auth_key

    def get_auth_key(self):
        return self.__auth_key

    def take_ownership(self):
        self.sedcli.take_ownership(self.__auth_key, self.blkdev)

    def release_ownership(self):
        self.sedcli.release_ownership(self.__auth_key, self.blkdev)

    def lock_range(self):
        self.sedcli.lock_range(self.__auth_key, self.blkdev)

    def unlock_range(self):
        self.sedcli.unlock_range(self.__auth_key, self.blkdev)

    def list_locking_range(self):
        self.sedcli.list_locking_range(self.__auth_key, self.blkdev)

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
    # SED specific
    subparsers = parser.add_subparsers(dest='subcommand', help='Available subcommands')
    for subcmd, subhelp in node.sc_opts.items():
        subparser = subparsers.add_parser(subcmd, help=subhelp[0])
        for k,v in subhelp[1].items():
            subparser.add_argument(k, help=v[0])

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

########################### Main Entry ###########################
def main():
    system_chker()

    node = Node(TPM())

    parser = argparse.ArgumentParser()
    set_args(parser, node)
    chk_args(parser, node)

    subcmd = parser.parse_args().subcommand
    blkdev = parser.parse_args().blkdev
    node.fetch_sedisks_pending(blkdev)

    try:
        node.tpm_read_key();
    except TPMKeyNotFoundError as e:
        print(f"Generate master key because {e}")
        node.gen_master_key()
        node.tpm_create_space()
        node.tpm_write_key()
    except:
        print(f"Unexpected error: {type(e).__name__}: {e}");
        usage()
    finally:
        node.do_subcmd(subcmd)

    return 0;

if __name__ == '__main__':
    sys.exit(main())
