#!/bin/sh

# This script sets up the main server system, then a system for each floor of
# the virtual building based on a list contained in one or more config files.

#the main system uses 'rservrd' for admin control, 'rsv-fsrelay' to connect to
#the servers for each virtual floor, and 'system-status' to update the status of
#the system as a whole.
{ echo "execute_critical @rservrd@-dxr";
  echo "execute @rsv-fsrelay@system-connect";
  echo "execute @./system-status@status";
  echo "register_all_wait"; } | \
rservr system /dev/null

#set up a local connection point for the other systems to connect to
rservrd system @local_listen@system-connect@/tmp/system-connect > /dev/null

cat $* | while read line; do
  eval ./floor-subsystem.sh $line
done

#execute the configuration-initiation client
rservrd system @execute@./system-control@control@status@rooms > /dev/null
