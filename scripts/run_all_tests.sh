#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

for test in ${SCRIPT_DIR}/../build/tests/*test; do 
  $test --gtest_brief=1;
done

