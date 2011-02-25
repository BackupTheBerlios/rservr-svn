#!/bin/sh


cleanup()
{
  rservrd '^server[12]$' @server_term
}


rm -f server1.log server2.log intercept.log

#stop a currently-running instance if necessary
if rservrd '^server[12]$' 2> /dev/null; then
  cleanup
fi

#start the two servers ('server1' must be started first)
rservr server1 server1.log server1.conf || { cleanup; exit 1; }
rservr server2 server2.log server2.conf || { cleanup; exit 1; }
