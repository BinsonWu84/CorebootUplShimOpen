#!/bin/bash
ToolChain="GCC5"
BuildTarget="DEBUG"
Alignment=0
while getopts "hb:t:" arg; do
  case $arg in
    h)
      echo "usage:"
      echo "  -h: help"
      echo "  -b: build target, default is DEBUG"
      echo "  -t: tool chain, default is GCC5"
      exit 0
      ;;
    b)
      BuildTarget=$OPTARG
      ;;
    t)
      ToolChain=$OPTARG
      ;;
  esac
done

export WORKSPACE=$(cd `dirname $0`; pwd)
export PACKAGES_PATH="$WORKSPACE:$WORKSPACE/../Edk2"
echo $WORKSPACE
echo $PACKAGES_PATH

while [ $# -gt 0 ]; do
  shift
done
cd $WORKSPACE/../Edk2
source ./edksetup.sh
make -C ./BaseTools
python UefiPayloadPkg/UniversalPayloadBuild.py -t $ToolChain -b $BuildTarget -a IA32
cd $WORKSPACE
echo "export UPL_ALIGMENT=0x10">Build/UefiPayloadInit.sh
python CorebootUplHandle.py -t $ToolChain -b $BuildTarget
