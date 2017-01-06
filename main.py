#!/usr/bin/env python
# vim: set fileencoding=utf8

import os
import re
import struct
import sys
import subprocess
import argparse

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
TOOLCHAIN_DIR = os.path.join(THIS_DIR, 'toolchain')

IDA_EXE_PATH = '/Applications/idaq.app/Contents/MacOS/idaq'

def run_bash(command):
  shell_path = '/tmp/dsym_creator_temp_shell.sh'
  open(shell_path, 'w').write(command)
  return subprocess.check_output(['bash', shell_path]).strip()

def extract_raw_symbol_from_objc_symbols(binary_path, is_arm64):
  objc_symbols_bin_path = os.path.join(TOOLCHAIN_DIR, 'objc-symbols')
  arch = 'arm64' if is_arm64 else 'armv7'
  raw_symbols = run_bash('%s --arch %s %s | sort' % (objc_symbols_bin_path, arch, binary_path))
  
  # format to 'base	end	name'
  symbols = []
  for l in raw_symbols.split('\n'):
    start, name = l.split(' ', 1)
    start = int(start, 16)  # source start is a hex string
    start -= 1      # result of objc-symbol has an offset, correct it
    if len(symbols) > 0:
      symbols[-1][1] = start    # fill last symbol's end address
    symbols.append([start, 0, name])
  symbols[-1][1] = symbols[-1][0] + 0x1000  # fake end address

  formated_raw_symbols = '\n'.join(['%s	%s	%s' % (hex(x[0]), hex(x[1]), x[2]) for x in symbols])
  raw_symbol_path = '/tmp/dsym_creator_raw_symbol.txt'
  open(raw_symbol_path, 'w').write(formated_raw_symbols)
  return raw_symbol_path

def make_sure_ida_exists():
  if not os.path.exists(IDA_EXE_PATH):
    print >> sys.stderr, "IDA Pro is not installed at %s, please install first" % IDA_EXE_PATH
    sys.exit(1)

def extract_thin_if_binary_is_fat(binary_path, is_arm64):
  magic_num = struct.unpack('<L', open(binary_path, 'r').read(4))[0]
  if magic_num == 0xbebafeca:
    # fat file, extract armv7
    arch = 'arm64' if is_arm64 else 'armv7'
    fresh_binary_path = '/tmp/dsym_creator_binary_' + arch
    run_bash('lipo -thin %s %s -output %s' % (arch, binary_path, fresh_binary_path))
    binary_path = fresh_binary_path
  elif magic_num == 0xfeedface and not is_arm64:
    pass
  elif magic_num == 0xfeedfacf and is_arm64:
    pass
  else:
    # invalid file
    print >> sys.stderr, 'invalid binary file: %s' % binary_path
    sys.exit(1)
  return binary_path

def extract_raw_symbol_from_ida(binary_path):
  raw_symbol_path = '/tmp/dsym_creator_raw_symbol.txt'
  idc_script_path = os.path.join(TOOLCHAIN_DIR, 'IDAScript', 'all.idc')
  run_bash('%s -S"%s \"%s\"" %s' % (IDA_EXE_PATH, idc_script_path, raw_symbol_path, binary_path))
  return raw_symbol_path

def extract_uuid_from_binary(binary_path):
  macho_dump_path = os.path.join(TOOLCHAIN_DIR, 'macho-dump.py')
  uuid = run_bash('%s %s 2>/dev/null | grep uuid | egrep -o \'\w{8}-\w{4}-\w{4}-\w{4}-\w{12}\'' % (macho_dump_path, binary_path))
  return uuid

def calculate_dwarf_sections_min_file_offset_from_binary(binary_path):
  macho_dump_path = os.path.join(TOOLCHAIN_DIR, 'macho-dump.py')
  dwarf_min_offset = 0x1000
  result = run_bash('%s %s 2>/dev/null | egrep "vm_addr|vm_size" | egrep -o "[0-9]+" | paste - -' % (macho_dump_path, binary_path))
  for line in result.split('\n'):
    vmaddr, vmsize = line.split()
    offset = int(vmaddr) + int(vmsize)
    if dwarf_min_offset < offset:
      dwarf_min_offset = offset
  return dwarf_min_offset

def dsymcreator_format_symbol(uuid, raw_ida_symbol, dwarf_section_offset, output, is_arm64):
  dsym_creator_path = os.path.join(TOOLCHAIN_DIR, 'DSYMCreator')
  command = [dsym_creator_path,
             '--uuid', uuid,
             '--raw_ida_symbol', raw_ida_symbol,
             '--dwarf_section_vmbase', hex(dwarf_section_offset),    # DSYMCreator need a hex string
             '--output', output]
  if is_arm64:
    command.append('--arm64')
  retcode = subprocess.call(command)
  if retcode !=0:
    print >> sys.stderr, 'DSYMCreator run failed!'
    sys.exit(1)

def main(options):
  binary_path = os.path.abspath(options.binary_path)
  binary_dir, binary_file_name = os.path.split(binary_path)
  output_symbol_path = os.path.join(binary_dir, '%s.symbol' % binary_file_name)

  raw_symbol_path = None

  # if input binary is a fat file, extract armv7
  binary_path = extract_thin_if_binary_is_fat(binary_path, options.arm64)

  if options.only_objc:
    # only objc mode, use objc-symbols output symbol
    # extract symbols by objc-symbols 
    raw_symbol_path = extract_raw_symbol_from_objc_symbols(binary_path, options.arm64)
  else:
    # ida pro mode
    # first make sure IDA exists
    make_sure_ida_exists()
    # extract raw symbol from IDA Pro
    raw_symbol_path = extract_raw_symbol_from_ida(binary_path)

  # extract uuid fram binary file
  uuid = extract_uuid_from_binary(binary_path)
  # extract already used address ranges from binary file, then calculate the minimum offset for dsym section
  dwarf_sections_min_offset = calculate_dwarf_sections_min_file_offset_from_binary(binary_path)
  # format symbol
  dsymcreator_format_symbol(uuid, raw_symbol_path, dwarf_sections_min_offset, output_symbol_path, options.arm64)

  return 0


if __name__ == '__main__':
  description = '''create a symbol file from a decrypted iOS binary. 
If you want to create the symbol file by IDA Pro (free version is enough!), then you can execute command like this.
    {self_name} --input /path/to/binary

If you don't want to use IDA Pro, then you can use only-objc mode (function count will be less than default mode), like this.
    {self_name} --only-objc --input /path/to/binary

Then a file named `binary.symbol` will be in /path/to directory.'''.format(self_name=sys.argv[0])

  parser = argparse.ArgumentParser(description=description, formatter_class=argparse.RawDescriptionHelpFormatter,)
  parser.add_argument('--only-objc', action='store_true', help='only dump objc functions, if you don\'t have IDA Pro, you can use this switch')
  parser.add_argument('--arm64', action='store_true', help='binary is arm64 version, or you want to extract arm64 part from universal file, note that since IDA Pro demo version doesn\'t support arm64, so you need use --only-objc with this flag')
  parser.add_argument('--input', action='store', required=True, dest='binary_path', help='path of the binary file, which must be decrypted first')
  args = parser.parse_args()

  sys.exit(main(args))

# vim: number list tabstop=2 shiftwidth=2 expandtab
