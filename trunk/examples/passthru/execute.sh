#!/bin/sh


rm -f server1.log server2.log

rservr server1 server1.log server1.conf
rservr server2 server2.log server2.conf

rservrd server1 @execute@./receiver@receiver
rservrd server2 @execute@./sender@sender@forward2@`pwd`/controller-socket@receiver
