#!/usr/bin/env perl
#
# Script to ftp model data from an ftp server. This script requires
# (sources) the input config file to provide needed variables specific
# to the type of model data being retreived. When running, the script
# forks subprocesses to ftp files simultaneously, using an adaptive
# incrementing approach. 
#
# A lock file is used to prevent instances of this script from running
# which might try to get the same data.
#
# An optional date argument can be used but note that ftp servers
# generally keep only the most recent day's data on the server.
# Otherwise, this script tries to get today's data. When getting
# today's data (ie; date option is not used), the script will get
# yesterday's model file if the current system hour is less than
# the model hour it is trying to get. (eg; you're getting the 18Z run
# at 02Z.)
#
# Jim Cowie/RAP Circa 2001
#
# Updated 9/26/05 by JCraig to allow multiple files to be downloaded
# and concatenated together.  This happens when the input config files
# ftpDir and ftpFglob are arrays. The files are downloaded to the tmp
# directory and only when all of a forecast time's files are present
# are they moved/concatenated to the final location.
#
# Jim Cowie/RAL 01/04/07 Added file extension specification, fixed some
#               start-up issues, removed flag file usage as this is
#               not necessary anymore and complicates the process.
#
#######################################################################


if ($#ARGV < 1 ) {
    print "Usage $0 model_config.pl refhour [YYYYMMDD]\n";
    print "Arguments:\n";
    print "model_config.pl\tPath to file defining model-specific elements\n";
    print "refhour\t\tModel reference hour (eg; 00, 12)\n";
    print "YYYYMMDD\tOptional date to retreive instead of today\n";
    exit 2;
}

# Make sure the root directory for output is defined
$root_dir = $ENV{'RWFS_ROOT_DIR'};
if (! defined ($root_dir)) {
    print "Error: RWFS_ROOT_DIR environment variable is not set.\n";
    exit 2;
}

# Process refhour first, since it and the date will be used to create
# variables defined in the model_config file.

$refhour = $ARGV[1];
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$idst) = gmtime();

# Get today's date or use optional date. If we're getting today's
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

# Set up directory paths and other constants
$lockDir = "$root_dir/lock";
$logDir  = "$root_dir/log";
$rawDir  = "$root_dir/raw_data/model/$model/$today";
$tmpDir  = "$root_dir/raw_data/tmp";

$lower_max = 4;         # Lower limit of simultaneous child ftp processes
$upper_max = 8;         # Upper limit of simultaneous child ftp processes
$cur_max = $lower_max;  # Current limit of simultaneous child ftp processes
$num_kids = 0;          # Current number of child processes
$num_ftps = 0;          # Number of files successfully ftp'd

# Set default raw grib file extension
$ext = "grb" if (!defined($ext));

# Set up lock file and see if we can lock it. If not, simply exit.
`mkdir -p $lockDir` if (! -d $lockDir);
$lockFile = "$lockDir/ftp_$model.$refhour.lock";
open FTPLOCK, ">$lockFile";
exit unless(flock (FTPLOCK, 2|4));  # Non-blocking lock

# Set up log output
`mkdir -p $logDir` if (! -d $logDir);
$logFile = "$logDir/ftp_$model.$today.$refhour.log";
open STDOUT, ">>$logFile";
open STDERR, ">&STDOUT";
select STDERR; $| = 1;  # make unbuffered
select STDOUT; $| = 1;  # make unbuffered

print &ptime, "Starting.\n" ;

`mkdir -p $tmpDir` if (! -d $tmpDir);
if (! (chdir $tmpDir)) {
    print &ptime, "Error: Can't cd to $tmpDir: $!\n";
    print &ptime, "Ending: exit status = 1.\n";
    exit 1;
}

if (! -d $rawDir) {
    print &ptime, "Executing: mkdir -p $rawDir\n";
    `mkdir -p $rawDir`;
}

# Set to loop while we keep finding additional files

$more = 1;

while ($more) {

  if($#ftpDir == -1) {
    @ftpDir = ($ftpDir);
    @ftpFglob = ($ftpFglob);
  }

  # loop over fcst times to get

  foreach $thing (@ftimes){
  
    for ($i=0; $i <= $#ftpDir; $i++) {
      $dir = $ftpDir[$i];
      $glob = $ftpFglob[$i];

      # Set up remote, local and flag filenames
      $rfile = $glob;
      $rfile =~ s/\*/$thing/;

      # If "thing" is an analysis, use 00.
      if ($thing eq "anal" || $thing eq "anl") {
	  $ft = "f000";
      }
      # Normalize to fNNN format
      elsif ($thing =~ /^\d{2,}/) {
	  $ft = sprintf("f%03d", $thing);
      }
      elsif ($thing =~ /^f(\d{2,})/) {
	  $ft = sprintf("f%03d", $1);
      }
      else {
	  $ft = sprintf("%s", $thing);
      }

      # $lfile is the filename locally, $tfile is the temp name which
      # is similar to $lfile but also contains a number to handle multiple
      # input files per forecast time. $rpath is the final destination
      # of the file.

      $lfile = sprintf("%s.%8d.i%02d00.%s.%s", $model, $today, $refhour, $ft, $ext);
      $tfile = sprintf("%s.%8d.i%02d00.%s.%d.%s", $model, $today, $refhour, $ft, $i, $ext);
      $rpath = "$rawDir/$lfile";
      
      #print "dir/rfile = $dir/$rfile\n";
      
      # Get the file if we don't already have it
      if ((! -e "$rpath") && (! -e $tfile)) {

        # Fork a child process
	if ($pid = fork) {
	    $num_kids++;

            # If we've reached the max processes, wait until one finishes
	    if ($num_kids >= $cur_max) {
		wait;
		$num_kids--;
	    
                # If child process fails, decrement max ftp's and exit loop
	        if (($? >> 8) != 0) {
                    $cur_max-- if ($cur_max > $lower_max);
	            last;
	        }

		# Increment number of ftp's and the max if successful
                $num_ftps++;
                $cur_max++ if ($cur_max < $upper_max);
	    }
	}
	elsif (defined($pid)) {

            # Use one of two forms of the ncftp command to get the file. The
            # first form is when login information is supplied in an external
            # file. The name of this file is contained in the $login variable
            # and is defined in the required model config file. The second
            # form uses the ncftp URL syntax, and logs in anonymously.
            # See ncftpget man page for a description of other options.

	    #print "path: ftp://$ftpHost/$dir/$rfile\n";

            if (defined($login)) {
                $ftpstat = system "ncftpget -f $login -V -F -Z . $dir/$rfile > /dev/null 2>&1";
            }
            else {
                $ftpstat = system "ncftpget -V -F -Z ftp://$ftpHost/$dir/$rfile > /dev/null 2>&1";
            }

	    # Successful ftp.
	    if ( $ftpstat == 0 ) {

		print &ptime, "Info: Got $rfile\n";

		# Move the file to the local temp name
		print &ptime, "Executing: mv $rfile $tfile\n";
		`mv $rfile $tfile`;

		# Get the list of files for this forecast time.
		$listStr = sprintf("%s.%8d.i%02d00.%s", $model, $today, $refhour, $ft);
		@list = `find . -name "$listStr.*.$ext"`;

                # Create the final file if we have everything
		if($#list == $#ftpDir)
		{
		    # This is a safe-guard to make sure that another forked
		    # process does not do this exact same thing

		    open(GFILE, "<$listStr.0.$ext") || exit 0;
		    if (flock(GFILE, 2|4))
		    {
			print &ptime, "Executing: mv $listStr.0.$ext $lfile\n";
			`mv $listStr.0.$ext $lfile`;
			for ($j=1; $j <= $#list; $j++) {
			    print &ptime, "Executing: cat $listStr.$j.$ext >> $lfile\n";
			    `cat $listStr.$j.$ext >> $lfile`;
			    print &ptime, "Executing: rm $listStr.$j.$ext\n";
			    `rm $listStr.$j.$ext`;
			}
			print &ptime, "Executing: mv $lfile $rpath\n";
			`mv $lfile $rpath`;
		    }
	        }
      		exit 0;
	    }
	    else {
#		print &ptime, "Info: Could not get $rfile, $ftpstat\n";
		exit 1;
	    }
	}
	else {
	    print &ptime, "Error: Could not fork\n";
	    exit 1;
	}
      }
    }   
  }

  # Allow child processes to finish, count successful ftp's
  while ($num_kids != 0)
  {
      wait;
      $num_ftps++ if (($? >> 8) == 0);
      $num_kids--;
  }

  # Continue the loop if we successfully got a file. If continuing, reset
  # the ftp counter.
  if ($num_ftps == 0) {
      $more = 0;
  }
  else {
      $num_ftps = 0;
  }

}

# Count files
@flist = `find $rawDir -name "$model.$today.i${refhour}00.*.$ext"`;

print &ptime, "Info: Fetched ", $#flist+1, " of ", $#ftimes+1, " $model fcst times for $today.$refhour\n";

flock (FTPLOCK, 8); # Release lock
close FTPLOCK;
print &ptime, "Ending: exit status = 0.\n";
close STDOUT;
close STDERR;

###################################################################

sub ptime {
    $ts = sprintf("%02d:%02d:%02d ", (gmtime())[2],(gmtime())[1],(gmtime())[0]);
    return $ts;
}


