#!/bin/sh

cd "$( dirname "$0" )"

version="$( grep 'VERSION *= *' ../configure.ac | sed -r 's/^.*VERSION *= *([^ ]+).*$/\1/' )"

./package.sh "rservr-core-$version-doc.tar.bz2"
