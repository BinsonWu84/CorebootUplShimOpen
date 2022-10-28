#!/bin/bash
BuildTarget="DEBUG"
while getopts "hb:t:" arg; do
  case $arg in
    h)
      echo "usage:"
      echo "  -h: help"
      echo "  -b: build target, default is DEBUG"
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
echo $WORKSPACE

while [ $# -gt 0 ]; do
  shift
done


cd $WORKSPACE/CorebootUplShimPkg
make cleanall
make
