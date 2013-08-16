#!/usr/bin/env bash

current="$(readlink -f $(dirname "$0"))"
cd $(dirname "$0")

sed='gsed -r'
# stat='stat -c "%s %n"'
stat='stat -f "%z %N"'

additional=
additional_really=
additional_really_specific=

if [ "$1" = "all" ];then
  additional=",external"
fi

if [ "$2" = "all" ];then
  additional_really="../../hparser/trunk/{include,src}"
  additional_really_specific="echo ../../hparser/trunk/Makefile.am; "
fi

patterns="$additional_really include/{rservr,global$additional} lang libs plugins programs security extensions -name \"*.h\" -o -name \"*.hpp\" -o -name \"*.c\" -o -name \"*.cpp\" -o -name \"*.py\" -o -name \"*.r\" -o -name Makefile.am"
any_line="\".*\""
nonblank_line="."
find_command="( find $patterns -type f; echo param.h; echo attributes.h; $additional_really_specific )"

echo $find_command
echo

echo 'FILES:   ' $(eval $find_command | grep -c .)

echo 'LINES:   ' $(cat `eval $find_command` | eval grep -c $any_line) '(all)'

#'uncomment' is local because I wrote it!
echo 'RELEVANT:' $(cat `eval $find_command` | ./uncomment | grep -c .) '(non-blank/non-comment)'

echo 'SIZE:    ' $(du -chs `eval $find_command` | tail -n1)

echo
echo 'LONGEST 8: (by total lines)'
grep -c ".*" $( eval $find_command ) | \
  $sed 's/^(.+):([0-9]+)$/  \2\t\1/' | \
  $sed '{:start;s/^  ([0-9]{,4})\t/  0\1\t/g;tstart}' | \
  sort -r | head -8 | $sed '{:start;s/(^| )0/\1 /g;tstart}' | grep -n ".*"

echo
echo 'LARGEST 8: (by file size)'
eval $stat $( eval $find_command ) | \
  $sed 's/^([0-9]+) /\1\t/' | \
  $sed '{:start;s/^([0-9]{,6})\t/0\1\t/g;tstart}' | \
  sort -r | head -8 | $sed '{:start;s/(^| )0/\1 /g;tstart}' | grep -n ".*"

echo
echo 'HIGHEST 8 UNUSED LINES: (non-API / minus license header)'
eval $find_command | grep -v "include/rservr/" | while read file; do
  total=$( $sed 'btest; {:remove;N;/\*\//{N;d;b};bremove}; :test;1,/\*\// {/[ \t]*\/\*/bremove}' "$file" | grep -c ".*" )
  used=$( ./uncomment "$file" | grep -c "." )

  if [ "$total" -ne 0 ]; then
    ratio=$( echo "100*($total-$used)/($total)" | bc | $sed '{:start;s/^([0-9]{,2})(\.|$)/0\1\2/g;tstart}' )
  else
    ratio="(empty file)"
  fi
  echo -e "   $ratio%\t$file"
done | \
  sort -r | head -8 | $sed '{:start;s/(^| )0/\1 /g;tstart}' | grep -n ".*"

echo
echo 'HIGHEST 8 AVERAGE LINE LENGTH: (non-Makefile / non-blank/non-comment)'
eval $find_command | grep -v Makefile.am | while read file; do
  used=$( ./uncomment "$file" | grep -c "." )
  characters=$( ./uncomment "$file" | $sed '{:a;s/([^\n])([^\n]+)$/\1\n\2/g;ta}' | grep -c . )

  if [ "$used" -ne 0 ]; then
    ratio=$( echo "scale=1;$characters/$used" | bc | $sed '{:start;s/^([0-9]{,3})(\.|$)/0\1\2/g;tstart}' )
  else
    ratio="(empty file)"
  fi

  echo -e " $ratio\t$file"
done | \
  sort -r | head -8 | $sed '{:start;s/(^| )0/\1 /g;tstart}' | grep -n ".*"
