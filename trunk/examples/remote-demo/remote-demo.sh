#!/bin/sh

#set an optional log file by providing a single argument to the script

output="/dev/null"

if [ -n "$1" ]; then
  output="$1"
fi


clean_up()
{
  rservrd 'node[1-3]' @server_term
  exit
}

trap 'clean_up' 2 15


#start 3 very basic server systems

echo "starting 'node3'..."

{ echo "execute_critical rservrd -dxr";
  echo "execute rsv-fsrelay relay3in";
  echo "register_all_wait"; } | \
rservr node3 "$output"

echo "starting 'node2'..."

{ echo "execute_critical rservrd -dx";
  echo "execute rsv-fsrelay relay2out";
  echo "execute rsv-fsrelay relay2in";
  echo "register_all_wait"; } | \
rservr node2 "$output"

echo "starting 'node1'..."

{ echo "execute_critical rservrd -dxr";
  echo "execute rsv-fsrelay relay1out";
  echo "register_all_wait"; } | \
rservr node1 "$output"


#create connections between the systems

rservrd node3 @local_listen@relay3in@/tmp/relay3in
connect_addr2=`rservrd node2 @local_connect@relay2out@/tmp/relay3in`
rservrd node2 @local_listen@relay2in@/tmp/relay2in
connect_addr1=`rservrd node1 @local_connect@relay1out@/tmp/relay2in`

if [ -z "$connect_addr2" ] || [ -z "$connect_addr1" ]; then
  echo "exiting because of failed connection(s)."
  clean_up
  exit 1
fi


#start an echo client on one end and a terminal on the opposite end

echo "new clients..."

echo_pid=`rservrd node3 @execute@rsv-echo@repeater`
term_pid=`rservrd node1 @execute@rsv-terminal@/tmp/terminal`


if [ -n "$echo_pid" ] && [ -n "$term_pid" ]; then

  echo "terminal..."

  #start the terminal emulator (point of user interaction)

  echo "*** use [Ctrl]+C or [Ctrl]+D to exit terminal ***"
  unsterm /tmp/terminal

fi


#shut down the 3 server systems

echo "exiting..."

clean_up


echo "application complete."
