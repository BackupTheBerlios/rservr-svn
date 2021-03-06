#!/bin/sh

if [ $# -ne 1 ]; then
  echo $0 [package name]
  exit 1
fi

file="$1"

rm -f $(find . -name "*~")

TAR=tar

if [ -x "$(which gtar)" ]; then
  TAR=gtar
fi

$TAR --owner=root --group=`id -gn root` -cvvjf "$file" $( find api/ app user -type f | grep -v '/\.' ) \
  fragment.png index.html rservr-rabbit2.png rservr.css
