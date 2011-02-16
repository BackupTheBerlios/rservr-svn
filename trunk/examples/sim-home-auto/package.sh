#!/bin/sh

cd "$( dirname "$0" )/../.."

tar -cvvjf examples/"$1"-example-sim-home-auto.tar.bz2 --owner root --group root -T - <<END
examples/sim-home-auto/bsd-license
examples/sim-home-auto/floor-subsystem.sh
examples/sim-home-auto/main-system.sh
examples/sim-home-auto/Makefile
examples/sim-home-auto/readme
examples/sim-home-auto/subsystems
examples/sim-home-auto/src/separate-room.cpp
examples/sim-home-auto/src/system-control.c
examples/sim-home-auto/src/system-status.cpp
END
