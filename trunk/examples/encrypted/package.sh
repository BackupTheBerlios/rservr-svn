#!/bin/sh

cd "$( dirname "$0" )/../.."

tar -cvvjf examples/"$1"-example-encrypted.tar.bz2 --owner root --group root -T - <<END
examples/encrypted/bsd-license
examples/encrypted/package.sh
examples/encrypted/passwords
examples/encrypted/readme
examples/encrypted/relay-setup
examples/encrypted/server-setup
examples/encrypted/tpasswd
examples/encrypted/tpasswd.conf
END
