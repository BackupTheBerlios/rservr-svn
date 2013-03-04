#!/usr/bin/env bash

rservr test 'test-server.log' <<-END
  execute_critical rservrd -dxr
  execute_critical rsv-terminal '$( dirname -- "$0" )/terminal'
  execute_critical rsv-echo 'echo'
  execute_critical '$( dirname -- "$0" )/test-client.py' 'hello_client'
  register_all_wait
END
