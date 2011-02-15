#!/bin/sh

# This script sets up the main server system, then a system for each floor of
# the virtual building based on a list contained in one or more config files.

#the main system uses 'rservrd' for admin control, 'rsv-fsrelay' to connect to
#the servers for each virtual floor, and 'system-status' to update the status of
#the system as a whole.

#set an optional log file by providing a second argument to the script

output="/dev/null"

if [ -n "$2" ]; then
  output="$2"
fi

echo "start server 'system'..." 1>&2

{ echo "execute_critical rservrd -dxr";
  echo "execute rsv-fsrelay system-connect";
  echo "execute ./system-status status";
  echo "register_all_wait"; } | \
rservr system "$output" || exit 1

#set up a local connection point for the other systems to connect to
rservrd system @local_listen@system-connect@/tmp/system-connect > /dev/null || exit 1

cat "$1" | while read line; do
  eval ./floor-subsystem.sh "$output" $line || exit 1
done

echo "request configuration from 'status'..." 1>&2

#execute the configuration-initiation client
#this sends a 'configure' request for system 'rooms' to the 'status' client
rservrd system @execute@./system-control@control@status@rooms > /dev/null || exit 1
