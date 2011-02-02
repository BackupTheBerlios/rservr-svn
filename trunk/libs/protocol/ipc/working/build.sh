#!/bin/sh

bison -Wall -b parser -y -d parser.y
flex -o lexer.c lexer.l
