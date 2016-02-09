#!/bin/bash

set -x

scons

OUT=$?

if [ $OUT -ne 0 ]; then
    exit 1
fi

cp vdt45 ~/component_tests/vdt_test/
cp vdt45 ~/component_tests/vdt_test2/
cd ~/component_tests/vdt_test/
./run.csh
