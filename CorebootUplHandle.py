## @file
# This file contains the script to build UniversalPayload
#
# Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import argparse
import subprocess
import os
import shutil
import sys
from   ctypes import *
import struct

sys.dont_write_bytecode = True
class EFI_GUID(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ('Data1',         c_uint32),
        ('Data2',         c_uint16),
        ('Data3',         c_uint16),
        ('Data4',         ARRAY(c_uint8, 8)),
        ]

class BlockMap(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ('NumBlocks',      c_uint32),
        ('Length',         c_uint32),
        ]

class EFI_FIRMWARE_VOLUME_HEADER(LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ('ZeroVector',           ARRAY(c_uint8, 16)),
        ('FileSystemGuid',       EFI_GUID),
        ('FvLength',             c_uint64),
        ('Signature',            c_uint32),
        ('Attributes',           c_uint32),
        ('HeaderLength',         c_uint16),
        ('Checksum',             c_uint16),
        ('ExtHeaderOffset',      c_uint16),
        ('Reserved',             ARRAY(c_uint8, 1)),
        ('Revision',             c_uint8),
        ('BlockMap',             ARRAY(BlockMap, 1)),
        ]

def RunCommand(cmd):
    print(cmd)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,cwd=os.environ['WORKSPACE'])
    while True:
        line = p.stdout.readline()
        if not line:
            break
        print(line.strip().decode(errors='ignore'))

    p.communicate()
    if p.returncode != 0:
        print("- Failed - error happened when run command: %s"%cmd)
        raise Exception("ERROR: when run command: %s"%cmd)

def CalCheckSum(Number, Length):
    CheckSumChange = 0
    for i in range(Length):
        CheckSumChange += Number%65536
        Number = int(Number/65536)
    return(CheckSumChange)

def ChangeHeaderCheckSum(FvHeader, OldFvHeader):
    PreChecksum = CalCheckSum(FvHeader.FvLength, 4) + CalCheckSum(FvHeader.BlockMap[0].NumBlocks, 2)
    CurChecksum = CalCheckSum(OldFvHeader.FvLength, 4) + CalCheckSum(OldFvHeader.BlockMap[0].NumBlocks, 2)
    FvHeader.Checksum += (CurChecksum - PreChecksum)%65536

def AddVideoDriverToUpl(Args):
    BuildTarget  = Args.Target
    ToolChain    = Args.ToolChain

    os.environ['WORKSPACE'] = os.path.dirname(os.path.abspath(__file__))
    VideoFfsDir = os.path.join(os.environ['WORKSPACE'], os.path.normpath("CorebootUplBin/QemuVideoDriver.ffs"))
    UplBuildDir = os.path.join(os.environ['WORKSPACE'], os.path.normpath("Build/UefiPayloadPkgX64"))
    FvInputDir  = os.path.join(UplBuildDir, "{}_{}".format (BuildTarget, ToolChain), os.path.normpath("FV/DXEFV.Fv"))
    FvOutputDir = FvInputDir

    #
    # Expand the Fv size to add extra ffs
    #
    FvSize = 0
    FvHeader = EFI_FIRMWARE_VOLUME_HEADER()
    with open(FvInputDir, "rb") as fd:
        DxeFvBuffer = fd.read()
        OldFvHeaderBuffer = EFI_FIRMWARE_VOLUME_HEADER.from_buffer_copy(DxeFvBuffer)
        FvHeader = EFI_FIRMWARE_VOLUME_HEADER.from_buffer_copy(DxeFvBuffer)
        FfsBlock = int(os.path.getsize(VideoFfsDir)/FvHeader.BlockMap[0].Length) + 1
        ExtraFvLength = FfsBlock * FvHeader.BlockMap[0].Length
        FvHeader.BlockMap[0].NumBlocks += FfsBlock
        FvHeader.FvLength += ExtraFvLength
        ChangeHeaderCheckSum(FvHeader, OldFvHeaderBuffer)

    FvSize = FvHeader.FvLength
    fp = open(FvInputDir, 'rb+')
    fp.write(bytearray(FvHeader))
    fp.close()

    fp = open(FvInputDir, 'ab+')
    if FvSize > len(DxeFvBuffer):
        a = struct.pack('B', 255)
        fp.write(a * ( FvSize - len(DxeFvBuffer)))
    fp.close()

    #
    # Add extra video ffs into DXEFV
    #
    if (sys.platform == 'win32'):
        FmmtPath = os.path.join(os.environ['WORKSPACE'], os.path.normpath("CorebootUplBin/FMMT.exe"))
    elif (sys.platform.find ('linux') != -1):
        FmmtPath = os.path.join(os.environ['WORKSPACE'], os.path.normpath("CorebootUplBin/FMMT.elf"))
    else:
        print ('Currently, %s is unsupported' % (sys.platform) )
        exit(1)
    ToolCommand = "{} -a {} FV0 {} {}".format (FmmtPath, FvInputDir, VideoFfsDir, FvOutputDir)
    RunCommand(ToolCommand)

    if "CLANG_BIN" in os.environ:
        LlvmObjcopyPath = os.path.join(os.environ["CLANG_BIN"], "llvm-objcopy")
    else:
        LlvmObjcopyPath = "llvm-objcopy"
    try:
        RunCommand('"%s" --version'%LlvmObjcopyPath)
    except:
        print("- Failed - Please check if LLVM is installed or if CLANG_BIN is set correctly")
        sys.exit(1)
    #
    # Update uefi_fv in UPL
    #
    UplDir         = os.path.join(UplBuildDir, 'UniversalPayload.elf')
    ObjCopyFlag    = "elf32-i386"
    remove_section = '"{}" -I {} -O {} --remove-section .upld.uefi_fv {}'.format (LlvmObjcopyPath, ObjCopyFlag, ObjCopyFlag, UplDir)
    add_section    = '"{}" -I {} -O {} --add-section .upld.uefi_fv={} {}'.format (LlvmObjcopyPath, ObjCopyFlag, ObjCopyFlag, FvOutputDir, UplDir)
    set_section    = '"{}" -I {} -O {} --set-section-alignment .upld.uefi_fv=16 {}'.format (LlvmObjcopyPath, ObjCopyFlag, ObjCopyFlag, UplDir)
    RunCommand(remove_section)
    RunCommand(add_section)
    RunCommand(set_section)

    shutil.copy (UplDir, os.path.join(os.environ['WORKSPACE'], 'Build/UefiUniversalPayload.elf'))

def main():
    parser = argparse.ArgumentParser(description='For adding QemuVideoDriver into UPL')
    parser.add_argument('-t', '--ToolChain', default='GCC5')
    parser.add_argument('-b', '--Target', default='DEBUG')
    args = parser.parse_args()
    AddVideoDriverToUpl(args)
    print ("Successfully modify Universal Payload")

if __name__ == '__main__':
    main()
