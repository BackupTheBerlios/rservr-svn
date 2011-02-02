#!/bin/bash


echo '!rservr[my_command] {'
echo '	!route {'
echo '		number = !xABCD'
echo '		value  = !-12333'
echo '		string = some_label'
echo '		address = mydata?/tmp/socket'
echo '	}'
echo ''
/bin/echo -En '	single_data = \BA\'; echo #dd if=/dev/urandom bs=10 count=1 2> /dev/null; echo
echo '	a_group = {'
/bin/echo -En '		some_data = \BA\'; echo #dd if=/dev/urandom bs=10 count=1 2> /dev/null; echo
/bin/echo -En '		more_data = \BA\'; echo #dd if=/dev/urandom bs=10 count=1 2> /dev/null; echo
echo '	}'
echo '}'
