#!/bin/bash

rservr test 'test-server.log' <(
echo execute_critical rservrd -dxr
echo execute_critical rsv-terminal $( dirname "$0" )/terminal
echo execute_critical rsv-echo 'echo'
echo execute_critical $( dirname "$0" )/test-client.py 'hello_client'
echo register_all_wait
)
