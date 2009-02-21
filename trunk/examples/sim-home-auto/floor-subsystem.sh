#!/bin/bash

# This script creates a server system for each virtual floor with one client for
# each room on the floor.

{ echo "execute @rservrd@-dxr";
  echo "execute @rsv-fsrelay@$1-connect";
  echo "register_all_wait"; } | \
rservr "$1" /dev/null

for room in $*; do
  if [ "$room" = "$1" ]; then
    continue
  fi

  name=$( echo "$room" | cut -d: -f1 )
  lights=$( echo "$room" | cut -d: -f2 )
  fans=$( echo "$room" | cut -d: -f3 )
  vents=$( echo "$room" | cut -d: -f4 )

  rservrd "$1" @execute@separate-room@$name@$lights@$fans@$vents > /dev/null
done

#connect the virtual floor's server system to the main system
rservrd "$1" @local_connect@"$1"-connect@/tmp/system-connect > /dev/null
