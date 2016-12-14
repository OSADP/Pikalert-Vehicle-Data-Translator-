#!/bin/csh

#Updates all the test binaries built using the instructions in ../../build_install/INSTALL
#USAGE: follow the INSTALL instructions to build all executables and then just run this script without any arguments

source ../../build_install/pikalert_env_vars.csh

cp $VDT_ROOT/bin/probe_to_json probe_to_json_test
cp $VDT_ROOT/bin/rwa rwa_test
cp $VDT_ROOT/bin/rwh rwh_test
cp $VDT_ROOT/bin/vdt45 vdt_test
