#!/bin/csh -f
#
# Script to test the RCTM build, applications and execution scripts.
#
# Assumes the rctm_env_vars.csh has been sourced for environment
# variables and all the code has been built and installed.
#
#
# Set the date and times for test mode. It requires two times to run
# the processes to boot-strap the dump_meso/fcst data files.
#
set run_times = ('20150211.150000' '20150211.160000')
#
mkdir -p $RCTM_ROOT_DIR >& /dev/null
#
# Clean up from previous tests...
#
rm -r $RCTM_ROOT_DIR/* >& /dev/null
#
# Set up test data directories and links
#
mkdir $RCTM_ROOT_DIR/log
mkdir $RCTM_ROOT_DIR/lock
mkdir $RCTM_ROOT_DIR/tmp
#
cp -r $cwd/../static_data $RCTM_ROOT_DIR
cp -r $cwd/../test/data/rctm/rdwx_fcst $RCTM_ROOT_DIR
cp -r $cwd/../test/data/rctm/dec_data $RCTM_ROOT_DIR
#
ln -s bc_rec_tmt $RCTM_ROOT_DIR/bc_init
ln -s rc_rec_tmt $RCTM_ROOT_DIR/rc_init
#
#
echo "This test runs RCTM components on sample data. This may take a couple minutes."
set t = 1
while ( $t <= $#run_times )

    setenv TEST_MODE_TIME $run_times[$t]

    echo "Testing RCTM for time $run_times[$t] ..."

    rctm_ep.py ep_dump_meso_rc.py ep_concat_meso_rc.py ep_obs_hist.py ep_rc_no_tmt.py ep_fec_rc_no_tmt.py ep_rc_cur_tmt.py ep_fec_rc_cur_tmt.py ep_rc_rec_tmt.py ep_fec_rc_rec_tmt.py ep_bc_no_tmt.py ep_bc_cur_tmt.py ep_bc_rec_tmt.py ep_merge_no_tmt.py ep_merge_cur_tmt.py ep_merge_rec_tmt.py ep_dump_fcst_rc.py 1800 >& /dev/null

    # remove logs for time 1, since errors are normal then
    if ($t == 1) then
	rm $RCTM_ROOT_DIR/log/*
    endif

    @ t++

end
#
# Check results
#
grep -i error $RCTM_ROOT_DIR/log/* > /dev/null
if ($status == 1) then
    echo "Success! No errors detected."
else
    echo "Test failed: Look in the log files under $RCTM_ROOT_DIR/log."
endif
