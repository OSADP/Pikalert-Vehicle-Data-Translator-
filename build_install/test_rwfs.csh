#!/bin/csh -f
#
# Script to test the RWFS build, applications and execution scripts.
#
# Assumes the rwfs_env_vars.csh has been sourced for environment
# variables and all the code has been built and installed.
#
#
# Set the date and times for test mode.
#
setenv TEST_MODE_TIME '20150211.120000'
#
mkdir -p $RWFS_ROOT_DIR >& /dev/null
#
# Clean up from previous tests...
#
rm -r $RWFS_ROOT_DIR/* >& /dev/null
#
# Set up test data directories and links
#
mkdir $RWFS_ROOT_DIR/log
mkdir $RWFS_ROOT_DIR/lock
mkdir $RWFS_ROOT_DIR/tmp
#
cp -r $cwd/../static_data $RWFS_ROOT_DIR
cp -r $cwd/../test/data/rwfs/dec_data $RWFS_ROOT_DIR
#
#
echo "This test runs RWFS components on sample data. This may take a minute ..."
#
# Get model data and de-grib (real-time mode only)
#
#ftp_model_data.pl $HOME/scripts/perl/nam_218.pl 12
#grib2site.pl $HOME/scripts/perl/nam_218.pl 12
#
# Run RWFS chain
#
rwfs_ep.py ep_mod_fcst.py nam ep_temporal_interp.py ep_derive_rdwx.py 600 
#
# Check results
#
grep -i error $RWFS_ROOT_DIR/log/* > /dev/null
if ($status == 1) then
    echo "Success! No errors detected."
else
    echo "Test failed: Look in the log files under $RWFS_ROOT_DIR/log."
endif
