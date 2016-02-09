#!/bin/csh -f
#
#
# Builds the RWFS system
#
#
set libs = (ncf ncfc nxncf dmapf grib2c log/src site_list spline tdrp)
set apps = (grib2site temp_interp derive_vars decoded2modfcst)
set script_types = (python perl)

set root_dir = '../'

set target = 'install'
if ($#argv > 0) then
    set target = "-c"
    if ("$argv[1]" != "-clean") then
	echo 'Usage: $0 [-clean]'
	exit
    endif
endif

# Loop over source
foreach lib ($libs)
    scons -C $root_dir/libs/$lib $target
    if ($status != 0) then
	echo "Error building lib $lib, Aborting ... "
	exit
    endif
end

foreach app ($apps)
    scons -C $root_dir/apps/$app $target
    if ($status != 0) then
	echo "Error building app $app, Aborting ... "
	exit
    endif
end

foreach type ($script_types)
    scons -C $root_dir/scripts/$type $target
    if ($status != 0) then
	echo "Error installing script type $type, Aborting ... "
	exit
    endif
end

