#!/bin/bash

#set an optional log file by providing a single argument to the script

output="/dev/null"

if [ -n "$1" ]; then
  output="$1"
fi


#start 3 very basic server systems

echo "starting 'node3'..."

rservr node3 "$output" \
<( echo "execute @rservrd@-dxr"
   echo "execute @rsv-fsrelay@relay3in"
   echo "register_all_wait" )

echo "starting 'node2'..."

rservr node2 "$output" \
<( echo "execute @rservrd@-dx"
   echo "execute @rsv-fsrelay@relay2out"
   echo "execute @rsv-fsrelay@relay2in"
   echo "register_all_wait" )

echo "starting 'node1'..."

rservr node1 "$output" \
<( echo "execute @rservrd@-dxr"
   echo "execute @rsv-fsrelay@relay1out"
   echo "register_all_wait" )


#create connections between the systems

echo "connecting..."

rservrd node3 @local_listen@relay3in@/tmp/relay3in
rservrd node2 @local_connect@relay2out@/tmp/relay3in
rservrd node2 @local_listen@relay2in@/tmp/relay2in
rservrd node1 @local_connect@relay1out@/tmp/relay2in


#start an echo client on one end and a terminal on the opposite end

echo "new clients..."

rservrd node3 @execute@rsv-echo@repeater
rservrd node1 @execute@rsv-terminal@/tmp/terminal


echo "terminal..."

#start the terminal emulator (point of user interaction)
#don't press [Ctrl]+C to exit; use [Ctrl]+D

unsterm /tmp/terminal


#shut down the 3 server systems

echo "exiting..."

rservrd 'node[1-3]' @server_term


echo "application complete."
