#!/usr/bin/env python
# vim: set fileencoding=utf8

import os
import re
import struct
import sys
import subprocess

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

IDA_EXE_PATH = '/Applications/idaq.app/Contents/MacOS/idaq'

def run_bash(command):
  shell_path = '/tmp/dsym_creator_temp_shell.sh'
  output_path = '/tmp/dsym_creator_temp_result.txt'
  open(shell_path, 'w').write('%s > %s' % (command, output_path))
  retcode = subprocess.call(['bash', shell_path])
  if retcode !=0:
    print >> sys.stderr, 'run bash command failed: %s' % command
    sys.exit(1)
  return open(output_path, 'r').read().strip()


def main(binary_path):
  binary_path = os.path.abspath(binary_path)
  binary_file_name = os.path.basename(binary_path)
  binary_dir = os.path.dirname(binary_path)
  output_symbol_path = os.path.join(binary_dir, '%s.symbol' % binary_file_name)
  toolchain_path = os.path.join(THIS_DIR, 'toolchain')

  # first make sure IDA exists
  if not os.path.exists(IDA_EXE_PATH):
    print >> sys.stderr, "IDA Pro is not installed at %s, please install first" % IDA_EXE_PATH
    sys.exit(1)

  # 0. if input binary is a fat file, extract armv7 first
  magic_num = struct.unpack('<L', open(binary_path, 'r').read(4))[0]
  if magic_num == 0xbebafeca:
    # fat file, extract armv7
    fresh_binary_path = '/tmp/dsym_creator_binary_armv7'
    run_bash('lipo -thin armv7 %s -output %s' % (binary_path, fresh_binary_path))
    binary_path = fresh_binary_path
  elif magic_num != 0xfeedface:
    # invalid file
    print >> sys.stderr, 'invalid binary file: %s' % binary_path
    sys.exit(1)

  # 1. extract raw symbol from IDA Pro
  raw_symbol_path = '/tmp/dsym_creator_raw_symbol.txt'
  idc_script_path = os.path.join(toolchain_path, 'IDAScript', 'all.idc')
  run_bash('%s -S"%s \"%s\"" %s' % (IDA_EXE_PATH, idc_script_path, raw_symbol_path, binary_path))

  # 2. extract uuid fram binary file
  macho_dump_path = os.path.join(toolchain_path, 'macho-dump.py')
  uuid = run_bash('%s %s 2>/dev/null | grep uuid | egrep -o \'\w{8}-\w{4}-\w{4}-\w{4}-\w{12}\'' % (macho_dump_path, binary_path))

  # 3. format symbol
  dsym_creator_path = os.path.join(toolchain_path, 'DSYMCreator')
  command = [dsym_creator_path,
             '--uuid', uuid,
             '--raw_ida_symbol', raw_symbol_path,
             '--output', output_symbol_path]
  retcode = subprocess.call(command)
  if retcode !=0:
    print >> sys.stderr, 'DSYMCreator run failed!'
    sys.exit(1)

  return 0


if __name__ == '__main__':
  if len(sys.argv) != 2:
    print >> sys.stderr, "usage: main.py binary"
    sys.exit(1)
  main(sys.argv[1])

# vim: number list tabstop=2 shiftwidth=2 expandtab
