#!/bin/sh

cd "$( dirname "$0" )/../.."

tar -cvvjf examples/"$1"-example-passthru.tar.bz2 --owner root --group root -T - <<END
examples/passthru/examples/passthru/bsd-license
examples/passthru/controller1.conf
examples/passthru/controller2.conf
examples/passthru/execute.sh
examples/passthru/Makefile
examples/passthru/package.sh
examples/passthru/readme
examples/passthru/receiver.c
examples/passthru/sender.c
examples/passthru/server1.conf
examples/passthru/server2.conf
END
