#!/bin/sh

cd "$( dirname "$0" )"

bison -Wall -b config-parser -y -d config-parser.y #&& \
#   { mv -v config-parser.tab.c config-parser.tab.cpp;
#     mv -v config-parser.tab.h ../../include/protocol/ipc;
#     sed -i 's/config-parser\.tab\.c/config-parser.tab.cpp/g' config-parser.tab.cpp; }

flex -o config-lexer.c config-lexer.l
