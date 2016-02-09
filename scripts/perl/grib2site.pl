#!/usr/bin/env perl
#
# This script converts GRIB files into netCDF using grib2site. It uses
# the input model_config.pl file to get the name of the model, the ref-
# erence time and the list of forecast times to process as well as other
# items.
#
# The script loops over the list of forecast times and looks to see
# which GRIB files need to be decoded. If the raw file exists and the
# decoded flag file does not exist, then this time is added to the list
# of files to process. The GRIB files in the list are then decoded into
# netCDF files using grib2site (supports GRIB1 and GRIB2), and flag
# files are created when finished. The data can be decoded directly to
# the final nc file or to a temp file first, then moved. If grib2site
# fails, the final file is removed, or if using a temp file, the temp
# file is left in place and not moved.
#
# The site list(s) taken from the model_config file are concatenated
# and sorted (with duplicates removed) to get a global site list. The
# cdl file from model_config is also updated to have a sufficiently
# large enough value of max_site_num.
#
# An optional date argument can be used, otherwise, this script uses
# today's data.
#
# Jim Cowie/RAL 01/11/06 Adapted from gribtonc.pl
# Jim Cowie/RAL 12/07/06 Removed usage of flag file from raw dirs,
#                        updated script description.
# Jim Cowie/RAL 02/09/10 Changed log file name syntax to match dicast
# Jim Cowie/RAL 10/04/10 Added ability to process raw files with RAPDMG
#                        name syntax
# Jim Cowie/RAL 06/21/12 Added ability to run ens_mean.pl to create an ensemble
#                        mean file. The variables $ensMeanName and $ensMeanCdl
#                        must be specified in the model_config file, and the
#                        input (grib2site) file created must have ensemble
#                        members.
#############################################################################

if ($#ARGV < 1) {
    print "Usage $0 model_config.pl refhour [YYYYMMDD]\n";
    print "Arguments:\n";
    print "model_config.pl\tPath to file defining model-specific elements\n";
    print "refhour\t\tModel reference hour (eg; 00, 12)\n";
    print "\nOptions:\n";
    print "YYYYMMDD\tOptional date to process instead of today\n";
    exit 2;
}

# Make sure the root directory for output is defined
$root_dir = $ENV{'RWFS_ROOT_DIR'};
if (! defined ($root_dir)) {
    print "Error: RWFS_ROOT_DIR environment variable is not set.\n";
    exit 2;
}

$tmp = 1;


$refhour = $ARGV[1];
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$idst) = gmtime();

# Get today's date or use optional date. If we're processing today's
# model data, subtract a day if the model reference hour is greater
# than the current hour.

if ($#ARGV == 1) {
    if ($hour < int($refhour)) {
        ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$idst) = gmtime(time()-24*3600);
    }
    $today = sprintf("%04d%02d%02d", $year+1900, $mon+1, $mday);
}
else {
    $today = $ARGV[2];
}


# Import the variables specific to this model.
die "Could not find file $ARGV[0]: $!\n" if (! -e $ARGV[0]);
require $ARGV[0];

# Set up directory and file paths for things
$lockDir = "$root_dir/lock";
$logDir  = "$root_dir/log";
$tmpDir  = "$root_dir/dec_data/tmp";
$rawDir  = "$root_dir/raw_data/model/$model/$today";
$dcdDir  = "$root_dir/dec_data/model/$dcdname/$today" if (!defined($dcdDir));
$ncName  = "$dcdname.$today.i${refhour}00.nc";

$tmpfile = "$tmpDir/$ncName";
$ncfile  = "$dcdDir/$ncName";

# Set the raw file extension for backward compatibility
$ext = "grb" if (!defined($ext));

# Lock a file to ensure a single instance of this script per model and
# reference time.
`mkdir -p $lockDir` if (! -d $lockDir);
$lockFile = "$lockDir/grib2site_$dcdname$refhour.lock";
open LOCK, ">$lockFile";
exit unless(flock (LOCK, 2|4));  # Non-blocking lock

# Set up output
`mkdir -p $logDir` if (! -d $logDir);
$logFile = "$logDir/grib2site_$dcdname$refhour.$today.asc";
open STDOUT, ">>$logFile";
open STDERR, ">&STDOUT";
select STDERR; $| = 1;  # make unbuffered
select STDOUT; $| = 1;  # make unbuffered

print &ptime, "Start: $0\n" ;

`mkdir -p $tmpDir` if (! -d $tmpDir && $tmp);
`mkdir -p $dcdDir` if (! -d $dcdDir);

# Create site list file. This will be a temp file to be removed later.
$tmpsitelist = "$tmpDir/${dcdname}_core_sites.asc.$$";
`sort -u $sitelist > $tmpsitelist`;

# Create CDL with correct max_site_num. Also a temp file.
chomp($nsites = `wc -l < $tmpsitelist`);
chomp($cdlname = `basename $cdlfile`);
$tmpcdl = "$tmpDir/$cdlname.$$";
`sed 's/max_site_num =.*;/max_site_num = $nsites ;/' $cdlfile > $tmpcdl`;


# Loop while there is more data to decode

$more = 1;

while ($more)
{

    undef(@rfiles);
    undef(@dflags);
    $numDcd = 0;

    # Loop over fcst times to decode

    foreach $f (@ftimes)
    {
	# Normalize time to 3 digits
	$ft = sprintf("f%03d", $f);
	
	# Set up names of raw GRIB and flag files 
	$rfile = sprintf("%s.%8d.i%02d00.%s.%s", $model, $today, $refhour, $ft, $ext);
	$rpath = "$rawDir/$rfile";
	$dflag = "$dcdDir/.$rfile.flag";
	
	# If raw file exists and it needs to be decoded, add to list
	if (-e $rpath && ! -e $dflag) 
	{
	    push @rfiles, $rpath;
	    push @dflags, $dflag;
	}
	# Try RAPDMG raw name format 
	elsif (defined($rapdmg_name))
	{
	    $rfile = sprintf("%8d_i%02d_%s_%s.%s", $today, $refhour, $ft, $rapdmg_name, $ext);
	    $rpath = "$rawDir/$rfile";
	    $dflag = "$dcdDir/.$rfile.flag";
	    
	    if (-e $rpath && ! -e $dflag) 
	    {
		push @rfiles, $rpath;
		push @dflags, $dflag;
	    }
	}

	# Keep track of how many are decoded
	$numDcd++ if (-e $dflag) ;
    }
    
    # Decode any un-decoded GRIB files.
    
    $stat = 0;
    if (defined(@rfiles))
    {

	# If using a temp file, copy any partial file to the temp file
	# and decode to that.

        if ($tmp == 1)
	{
  	    if (-e $ncfile) 
	    {
	        print &ptime, "Executing: cp -f $ncfile $tmpfile\n";
	        `cp -f $ncfile $tmpfile`;
	    }
	    elsif (-e $tmpfile) 
	    {
	        print &ptime, "Executing: rm -f $tmpfile\n";
	        `rm -f $tmpfile`;
	    }
	}
	else
	{
	    # No temp file, so set output file to temp file variable
	    $tmpfile = $ncfile;
	}

	# Loop over raw files to process.
	
	foreach $rfile (@rfiles)
	{
	    print &ptime, "Executing: grib2site $tmpcdl $tmpsitelist $tmpfile < $rfile\n";
	    $stat = system "grib2site $tmpcdl $tmpsitelist $tmpfile < $rfile";
	    
	    if ($stat != 0)
	    {
		last;
	    }
	}
	
	# If processing was successful, copy tmp file and touch flags
	
	if ($stat == 0) 
	{
	    if ($tmp == 1)
	    {
	        print &ptime, "Executing: mv -f $tmpfile $ncfile\n";
	        $stat = system "mv -f $tmpfile $ncfile";
	    }    

	    if ($stat == 0)
	    {
		foreach $flag (@dflags)
		{
		    print &ptime, "Executing: touch $flag\n";
		    `touch $flag`;
		}
	    }
	}

	# If processing was not successful, log message and remove the final
	# file if not using a temp file, as it might now be corrupted.

	else
	{
	    print &ptime, "Error: grib2site failed decoding $rfile\n";
	    if ($tmp == 0)
	    {
		print &ptime, "Executing: rm -f $tmpfile\n";
	        $stat = system "rm -f $tmpfile";
	    }

	    last;
	}
    }
    else
    {
	if ($numDcd == @ftimes) 
	{    
	    print &ptime, "Info: All forecast times decoded. ($numDcd)\n";
	}
	else
	{
	    print &ptime, "Info: No new forecast times to decode.\n";
	}
	$more = 0;
    }

}

# Create the ensemble mean file from an ensemble grid, if requested. This
# requires $ensMeanName and $ensMeanCdl defined in the model_config file,
# ens_mean.pl in the path, and all the forecast times must be decoded.

if (defined($ensMeanName) && ($numDcd == @ftimes))
{
    # Create output path and name

    $ensMeanDir = "$root_dir/dec_data/model/$ensMeanName/$today";
    `mkdir -p $ensMeanDir` if (! -d $ensMeanDir);
    $ncName  = "$ensMeanName.$today.i${refhour}00.nc";
    $tmpPath = "$tmpDir/$ncName";
    $ensMeanPath = "$ensMeanDir/$ncName";
    
    # If output file does not exist ...

    if (! -e $ensMeanPath)
    {
	# Create CDL with correct max_site_num.

	if (defined($ensMeanCdl) && -e $ensMeanCdl)
	{
	    chomp($cdlname = `basename $ensMeanCdl`);
	    $enscdl = "$tmpDir/$cdlname.$$";
	    `sed 's/max_site_num =.*;/max_site_num = $nsites ;/' $ensMeanCdl > $enscdl`;
	    
	    # Make sure the ensemble mean script is found

	    $found = system "which ens_mean.pl 1> /dev/null 2>&1";
	    if ($found == 0) 
	    {
		print &ptime, "Executing: ens_mean.pl $ncfile $enscdl $tmpPath\n";
		$stat = system "ens_mean.pl $ncfile $enscdl $tmpPath";
		if ($stat == 0)
		{
		    print &ptime, "Executing: mv -f $tmpPath $ensMeanPath\n";
		    $stat = system "mv -f $tmpPath $ensMeanPath";
		    `rm $enscdl`;
		}
		else
		{
		    print &ptime, "Error: ens_mean.pl failed creating $ensMeanPath.\n";
		}
	    }
	    else
	    {
		print &ptime, "Error: Ensemble mean generation requested but ens_mean.pl script not found.\n";
	    }
	}
	else
	{
	    print &ptime, "Error: Ensemble mean generation requested but \$ensMeanCdl is not defined or file does not exist.\n";
	}
    }
}

# Remove the temp files if there was no problem
if ($stat == 0) {
    `rm $tmpsitelist $tmpcdl`;
}



flock (LOCK, 8); # Release lock
close LOCK;
print &ptime, "Stop: Exit status = $stat.\n";
close STDOUT;
close STDERR;
exit ($stat);

###################################################################

sub ptime {
    $ts = sprintf("%02d:%02d:%02d ", (gmtime())[2],(gmtime())[1],(gmtime())[0]);
    return $ts;
}
