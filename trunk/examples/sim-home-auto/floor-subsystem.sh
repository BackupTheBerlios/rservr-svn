#!/bin/sh

# This script creates a server system for each virtual floor with one client for
# each room on the floor.

output="$1"
server="$2"
shift 2

{ echo "execute_critical rservrd -dxr";
  echo "execute rsv-fsrelay $server-connect";
  echo "register_all_wait"; } | \
rservr "$server" "$output"

for room in $*; do
  name=$( echo "$room" | cut -d: -f1 )
  lights=$( echo "$room" | cut -d: -f2 )
  fans=$( echo "$room" | cut -d: -f3 )
  vents=$( echo "$room" | cut -d: -f4 )

  rservrd "$server" @execute@./separate-room@$name@$lights@$fans@$vents > /dev/null
done

#connect the virtual floor's server system to the main system
rservrd "$server" @local_connect@"$server"-connect@/tmp/system-connect > /dev/null
