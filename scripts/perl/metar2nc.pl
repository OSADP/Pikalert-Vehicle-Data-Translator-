#!/usr/bin/env perl
#
# Decodes METAR reports and optionally creates netCDF file of decoded reports
# and or writes decoded reports to stdout.
#

use Fcntl;
use NetCDF ;
use File::Basename ;

#use Time::Local ;
use Time::Local 'timegm_nocheck'; # Doesn't exit on erroneous dates

no encoding; # Prevents unicode problems with input binary data (Perl 5.8.*)


$timeout = 1200 ;       # default timeout - no data for 20 minutes
$fileOpenTimeOut = 300;
$rtmax_age = 2 * 3600;  # Oldest allowable report age in seconds (real-time)
$tempdir = "/tmp";


sub usage
{
      print "Usage: $0 [-h] [-c cdlfile ] [-d outdir | -f outfile] [-l logdir] [-s stnfile] [-t timeout] [-T tempdir] [-v] [-y yymm] < STDIN\n\n";
      print "Options:\n";
      print "-h           This message\n";
      print "-c cdlfile   CDL file for decoding to netCDF\n";
      print "-d outdir    Directory to write netCDF files (can't be used with -f)\n";
      print "-f outfile   Netcdf file to place all output (can't be used with -d)\n";
      print "-l logdir    Directory to place log file\n";
      print "-s stnfile   Decode only sites in this file (default = all sites)\n";
      print "-t timeout   Timeout value in seconds\n";
      print "-T tempdir   Directory for temp files\n";
      print "-v           Write decoded reports to stdout\n";
      print "-y yymm      Year, month for historical data\n";
      print "\nIf the -c option is used, the decoder will output reports to netCDF file(s)\n";
      print "under 'outdir' (-d option), or in outfile (-f option). If -c is not used,\n";
      print "the decoder writes reports to stdout if the -v flag is present, otherwise\n";
      print "it simply parses the reports and exits. The -s option can be used to limit\n";
      print "decoding to only sites found in stnfile. It is also used to provide location\n";
      print "information for the output netcdf file. Current date information is obtained\n";
      print "from the system clock, unless the -y option is used. Informational messages\n";
      print "and errors are written to stderr.\n";
}


# process command line switches
while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;
    if (/^(-h)/) {
      usage();
      exit;
    }
    /^(-l)/ && ( $logdir = shift ) ;  # logdir
    /^(-c)/ && ( $cdlfile = shift ) ; # cdlfile for netcdf output
    /^(-d)/ && ( $datadir = shift ) ; # data target directory
    /^(-f)/ && ( $outfile = shift ) ; # single output file
    /^(-s)/ && ( $sfile = shift ) ;   # site file
    /^(-t)/ && ( $timeout = shift ) ; # timeout value
    /^(-T)/ && ( $tempdir = shift ) ; # temp dir
    /^(-y)/ && ( $yymm = shift ) ;    # year, month time context
    /^(-v)/ && $verbose++ ;           # turn verbose logging on
}

if (defined($cdlfile)) {
    if ((! defined($datadir) && ! defined($outfile)) ||
	(defined($datadir) && defined($outfile))) {
	print "\nError: Must use one of -d or -f argument with -c option\n\n";
	usage();
	exit;
    }
}

# File used to retain site decoding state between invocations (ie, which
# sites have already been decoded.)
$metar_list = "$tempdir/metar.lst";


# Split up the script name for later use
($fname, $fpath, $fsuffix) = fileparse($0, '\.pl');
$progname = "$fname$fsuffix";
$logname = $fname;

logmsg("Starting $progname\n") ;


# set interrupt handler
$SIG{ 'INT' }  = 'atexit' ;
$SIG{ 'KILL' }  = 'atexit' ;
$SIG{ 'TERM' }  = 'atexit' ;
$SIG{ 'QUIT' }  = 'atexit' ;

# set defaults

$F = -99999 ;
$A = \$F ;
$S1 = "\0" ;
$AS1 = \$S1 ;
$S2 = "\0\0" ;
$AS2 = \$S2 ;
$S3 = "\0\0\0" ;
$AS3 = \$S3 ;
$S4 = "\0\0\0\0" ;
$AS4 = \$S4 ;
$S8 = "\0" x 8 ;
$AS8 = \$S8 ;
$S10 = "\0" x 10 ;
$AS10 = \$S10 ;
$S15 = "\0" x 15 ;
$AS15 = \$S15 ;
$S32 = "\0" x 32 ;
$AS32 = \$S32 ;
$S128 = "\0" x 128 ;
$AS128 = \$S128 ;

# Handle netCDF-specific items
if (defined($cdlfile))
{
    # set error handling to verbose only
    $result = NetCDF::opts( VERBOSE ) ;

    # We need ncgen binary.
    open( NCGEN, "which ncgen |" ) ;
    $ncgen = <NCGEN> ;
    close( NCGEN ) ;

    if( $ncgen =~ /no ncgen/ )
    {
	die "Can't find NetCDF utility 'ncgen' in PATH : $!\n" ;
    }
    else
    {
	$ncgen = "ncgen" ;
    }
  
    %CDL = (
	    "rep_type", 0, "stn_name", 1, "wmo_id", 2, "lat", 3, "lon", 4, "elev", 5,
	    "ob_hour", 6, "ob_min", 7, "ob_day", 8, "time_obs", 9, "time_nominal", 10, 
	    "AUTO", 11, "UNITS", 12, "DIR", 13, "SPD", 14, "GUST", 15, "VRB", 16, 
	    "DIRmin", 17, "DIRmax", 18, "prevail_VIS_SM", 19, "prevail_VIS_KM", 20, 
	    "plus_VIS_SM", 21, "plus_VIS_KM", 22, "prevail_VIS_M", 23, "VIS_dir", 24, 
	    "CAVOK", 25, "RVRNO", 26, "RV_designator", 27, "RV_above_max", 28, 
	    "RV_below_min", 29, "RV_vrbl", 30, "RV_min", 31, "RV_max", 32, 
	    "RV_visRange", 33, "WX", 34, "vert_VIS", 35, "cloud_type", 36, "cloud_hgt", 37,
	    "cloud_meters", 38, "cloud_phenom", 39, "T", 40, "TD", 41, 
	    "hectoPasc_ALTIM", 42, "inches_ALTIM", 43, "NOSIG", 44, "TornadicType", 45, 
	    "TornadicLOC", 46, "TornadicDIR", 47, "BTornadic_hh", 48, "BTornadic_mm", 49,
	    "ETornadic_hh", 50, "ETornadic_mm", 51, "AUTOindicator", 52,
	    "PKWND_dir", 53, "PKWND_spd", 54, "PKWND_hh", 55, "PKWND_mm", 56,
	    "WshfTime_hh", 57, "WshfTime_mm", 58, "Wshft_FROPA", 59, "VIS_TWR", 60,
	    "VIS_SFC", 61, "VISmin", 62, "VISmax", 63, "VIS_2ndSite", 64,
	    "VIS_2ndSite_LOC", 65, "LTG_OCNL", 66, "LTG_FRQ", 67, "LTG_CNS", 68,
	    "LTG_CG", 69, "LTG_IC", 70, "LTG_CC", 71, "LTG_CA", 72, "LTG_DSNT", 73,
	    "LTG_AP", 74, "LTG_VcyStn", 75, "LTG_DIR", 76, "Recent_WX", 77,
	    "Recent_WX_Bhh", 78, "Recent_WX_Bmm", 79, "Recent_WX_Ehh", 80,
	    "Recent_WX_Emm", 81, "Ceiling_min", 82, "Ceiling_max", 83,
	    "CIG_2ndSite_meters", 84, "CIG_2ndSite_LOC", 85, "PRESFR", 86, "PRESRR", 87,
	    "SLPNO", 88, "SLP", 89, "SectorVIS_DIR", 90, "SectorVIS", 91, "GR", 92,
	    "GRsize", 93, "VIRGA", 94, "VIRGAdir", 95, "SfcObscuration", 96,
	    "OctsSkyObscured", 97, "CIGNO", 98, "Ceiling_est", 99, "Ceiling", 100,
	    "VrbSkyBelow", 101, "VrbSkyLayerHgt", 102, "VrbSkyAbove", 103, 
	    "Sign_cloud", 104, "Sign_dist", 105, "Sign_dir", 106, "ObscurAloft", 107,
	    "ObscurAloftSkyCond", 108, "ObscurAloftHgt", 109, "ACFTMSHP", 110,
	    "NOSPECI", 111, "FIRST", 112, "LAST", 113, "Cloud_low", 114, 
	    "Cloud_medium", 115, "Cloud_high", 116, "SNINCR", 117, "SNINCR_TotalDepth", 118,
	    "SN_depth", 119, "SN_waterequiv", 120, "SunSensorOut", 121, "SunShineDur", 122,
	    "PRECIP_hourly", 123, "PRECIP_amt", 124, "PRECIP_24_amt", 125, "T_tenths", 126,
	    "TD_tenths", 127, "Tmax", 128, "Tmin", 129, "Tmax24", 130, "Tmin24", 131, 
	    "char_Ptend", 132, "Ptend", 133, "PNO", 134, "PWINO", 135, "FZRANO", 136, "TSNO", 137, 
	    "maintIndicator", 138, "PlainText", 139, "report", 140, "remarks", 141 ) ;

    # default netCDF record structure, contains all vars for the METAR reports
    @defaultrec = ( $A, $A, $A, $A, $A, $A, $A, $A, $A, $A, $A, $A, $AS3, $A, $A,
		    $A, $A, $A, $A, $A, $A, $A, $A, $A, $AS2, $A, $A, [( $S3, $S3, $S3, $S3 )],
		    [( $F, $F, $F, $F )], [( $F, $F, $F, $F )], [( $F, $F, $F, $F )], 
		    [( $F, $F, $F, $F )], [( $F, $F, $F, $F )], [( $F, $F, $F, $F )], $AS32, $A,
		    [( $S4, $S4, $S4, $S4, $S4, $S4 )], [( $F, $F, $F, $F, $F, $F )],
		    [( $F, $F, $F, $F, $F, $F )], [( $S4, $S4, $S4, $S4, $S4, $S4 )],
		    $A, $A, $A, $A, $A, $AS15, $AS10, $AS2, $A, $A, $A, $A, $AS4, $A, $A,
		    $A, $A, $A, $A, $A, $A, $A, $A, $A, $A, $AS10, $A, $A, $A, $A, $A,
		    $A, $A, $A, $A, $A, $AS2, [( $S8, $S8, $S8 )], [( $F, $F, $F )],
		    [( $F, $F, $F )], [( $F, $F, $F )], [( $F, $F, $F )], $A, $A, $A, $A,
		    $A, $A, $A, $A, $AS2, $A, $A, $A, $A, $AS2, $AS8, $A, $A, $A, $A,
		    $AS3, $A, $AS3, $AS10, $AS10, $AS10, $AS8, $AS3, $A, $A, $A, $A, $A,
		    $AS1, $AS1, $AS1, $A, $A, $A, $A, $A, $A, $A, $A, $A, $A,
		    $A, $A, $A, $A, $A, $A, $A, $A, $A, $A, $A, $A, $AS128, $AS128, $AS128 ) ;

    # two fold purpose array, if entry > 0, then var is requested and it's value
    # is the position in the record, except first entry
    @W = ( 0 ) x ( $#defaultrec +1 ) ;
    $W[ 0 ] = -1 ;

    # open cdl and create record structure according to variables
    open( CDL, "$cdlfile" ) || die "could not open $cdlfile: $!\n" ;
    $i = 0 ;
    while( <CDL> ) {
	if( s#^\s*(char|int|long|double|float) (\w{1,25})## ) {
	    ( $number ) = $CDL{ $2 } ;
	    push( @rec, $defaultrec[ $number ] ) ;
	    $W[ $number ] = $i++ ;
	}
    }
    close CDL ;
    undef( @defaultrec ) ;
    undef( %CDL ) ;

    
    # Read in list of already processed reports if it exists.
    # NOTE: do this only in real-time mode. NOTE2: don't do this at all!
    #if ( ! $yymm ) {
	#if( -e "$metar_list" ) {
	#    open( LST, "$metar_list" ) || 
	#	die "Could not open $metar_list: $!\n" ;
	#    while( <LST> ) {
	#	( $stn, $rtptime, $record, $hr ) = split ;
	#	$reportslist{ "$stn $rtptime" }  = "$record $hr" ;
	#    }
	#    close LST ;
	#}
    #}
}


# Read in optional station data
if( defined($sfile) ) {

    open( STATION, "$sfile" ) || die "Could not open $sfile: $!\n" ;

    while( <STATION> ) {
	chomp;
	($id, $wmo_id, $name, $state, $country, $lat, $lon, $elev) = split /\t+/ ;
	$lat = sprintf( "%7.2f", $lat / 100 ) ;
	$lon = sprintf( "%7.2f", $lon / 100) ;
	$STATIONS{ "$id" } = "$wmo_id $lat $lon $elev" ;
    }
    close STATION ;    
}

# Open STDIN, set the end of input character to CTRL-C.
open( STDIN, '-' ) ;
$/ = "\cC" ;

# Loop forever until we time-out or hit eof.
while( 1 ) {

        vec($rin,fileno(STDIN),1) = 1;
	$nfound = select( $rout = $rin, undef, undef, $timeout );

	atexit("timeout") if( ! $nfound ) ;

	atexit( "eof" ) if( eof( STDIN ) ) ;

	$_ = <STDIN>;
    
	# Process year and month
	if( ! $yymm ) {
		$reference_year = (gmtime())[ 5 ] ;
		$reference_month = (gmtime())[ 4 ] ;
		$reference_month++ ;
	} else {
		$reference_year = substr( $yymm, 0, 2 ) ;
		$reference_month = substr( $yymm, 2 ) ;
		# Y2K fix - years are since 1900, so adjust.
		if ( $reference_year < 70 ) {
	    		$reference_year += 100;
		}
	}
	$theyear = $reference_year;
	$themonth = $reference_month;

	# Discard short bulletins. This size (40 bytes) is the size of a "null"
	# bulletin - just enough for the control characters, a header but no
	# body.
	
	if (length($_) < 40) {
	    logmsg("Bulletin too short.\n") ;
	    next ;
	}

	# Remove certain control characters
	s#\cC## ;
	s#\cM##g ;
	s#\cA\n## ;
	s#\c^##g ;
 
	s#\d\d\d\s{0,1}\n## ;
	s#(\w{4}\d{0,2}) (\w{4})\s*(\d{2})(\d{2})(\d{2})?.*\n## ;
	$tday = $3 ;
	$thour = $4 ;
	$tmin = $5 ;
	$tmin = 0 unless( $tmin ) ;
	next unless ( $tday && defined( $thour ) ) ;
	if ($tday < 1 || $tday > 31 || $thour > 23 || $tmin > 59) {
	    logmsg("Bad bulletin header: $1 $2 $3$4$5\n") ;
	    next ;
	}

	#$time_trans = thetime( "trans" ) ;
	if( s#^(METAR|SPECI) \d{4,6}Z?\n## ) {
		$rep_type = $1 ;
	} elsif( s#^(METAR|SPECI)\n## ) {
		$rep_type = $1 ;
	} elsif( s#^(M E T A R)\s*\n## ) {
		$rep_type = "METAR" ;
	} elsif( s#^(MTR\w{3})\n## ) {
		$rep_type = "METAR" ;
	} else {
		$rep_type = "METAR" ;
	}
	# Separate bulletins into reports, most common to least
	if( /=\n/ ) {
		s#=\s+\n#=\n#g ;
		@reports = split( /=\n/ ) ;
	} elsif (/METAR /) {
	        @reports = split( /METAR / ) ;
	} elsif (/SPECI /) {
	        @reports = split( /SPECI / ) ;
		$rep_type = "SPECI" ;
	} elsif (/TESTM /) {
	        @reports = split( /TESTM / ) ;
		$rep_type = "TESTM" ;
	} elsif (/TESTS /) {
	        @reports = split( /TESTS / ) ;
		$rep_type = "TESTS" ;
	} else {
	        @reports = split( /\n/ ) ;
	}
	
	for ( @reports ) { # Process each report in the bulletin
	
		# undefine vars
		undefinevars() ;

	        $theyear = $reference_year;
		$themonth = $reference_month;

		undef( $report ) ;
		undef( $remarks ) ;
		undef( $stn_name ) ;
		undef( $ob_day ) ;
                undef( $cor );

		next if( /^\n/ ) ;
		s#\n# #g ;
		# save original report and remarks
		if( s#(.*)( RMK| REMARKS)(.*)?#$1# ) {
			$report = $1 ;
			$remarks =  $3 unless( ! $3 || $3 =~ /NIL/ ) ;
		} else {
			$report = $_ ;
		}
		$rep_type = $1 if( s#^(METAR|SPECI|TESTM|TESTS)\s+## ) ;
		$stn_name = $1 if( s#^([A-Z]\w{3})\s+## ); #

		next unless( $stn_name ) ;

		# skip TEST messages
		next if ($rep_type eq "TESTM" or $rep_type eq "TESTS");

		# skip NIL reports
		next if( s#NIL\s*## ) ;


		# Identify site from site file, as needed
		
		if( defined($sfile) )
		{
		    $stn_found = 0;

		    if ( $STATIONS{ "$stn_name" } )
                    {
                        ( $wmo_id, $lat, $lon, $elev ) =
			    split( " ", $STATIONS{ "$stn_name" } ) ;
			$stn_found = 1;
		    }
		    elsif( $stn_name =~ /^K/ )
		    {
                        $tmp = substr( $stn_name, 1 ) ;
                        if( $STATIONS{ "$tmp" } )
			{
			    ( $wmo_id, $lat, $lon, $elev ) =
				split( " ", $STATIONS{ "$tmp" } ) ;
			    $stn_found = 1;
			}
		    }
		    if (! $stn_found)
		    {
			# This happens a lot.
			#&logmsg ("Ignoring site: $report\n");
			next;
		    }
		}

		# get day, hour, minute (supposed to be 6 digits)
		next unless( s#^(\d{4,6})Z\s+## );
		if( length( $1 ) == 4 ) {
			$ob_day = $tday ;
			$ob_hour = substr( $1, 0, 2 ) ;
			$ob_min = substr( $1, 2, 2 ) ;
		} elsif( length( $1 ) == 5 ) {
			$ob_day = sprintf( "%02d", substr( $1, 0, 1 ) ) ;
			$ob_hour = substr( $1, 1, 2 ) ;
			$ob_min = substr( $1, 3, 2 ) ;
		} else {
			$ob_day = substr( $1, 0, 2 ) ;
			$ob_hour = substr( $1, 2, 2 ) ;
			$ob_min = substr( $1, 4, 2 ) ;
		}
		$rpt_time = "$ob_day$ob_hour$ob_min";
		if( $ob_min > 59 || $ob_hour > 23 || $ob_day < 1 || $ob_day > 31) {
			logmsg("Bad ddhhmmZ format: $rpt_time\n") ;
			next ;
		}

		# Check if a correction
		$cor = 1 if (s#^COR\s+##) ;

		# Skip duplicates which are not corrections and have no remarks
		if( defined( $reportslist{ "$stn_name $rpt_time" } ) &&
		    ! defined($cor) && ! defined($remarks)) {
		    next;
		}
	       

 		# Use the appropriate reference day to determine the date
		if (! $yymm) {  # Real-time
		  $theday = (gmtime())[ 3 ] ;
                }
		else { # Non real-time; use the bulletin day
		  $theday = $tday ;
		}
		
		# Adjust the month (and year) if necessary
		if (($theday + 15) < $ob_day) {
		  $themonth--;
		  if ($themonth == 0) {
		    $themonth = 12;
		    $theyear--;
		  }
		}
		elsif ($theday > ($ob_day + 15)) {
		  $themonth++;
		  if ($themonth == 13) {
		    $themonth = 1;
		    $theyear++;
		  }
		}

		$theday = $ob_day;		            
		$yyyymmdd = sprintf("%04d%02d%02d", $theyear+1900, $themonth, $theday) ;

		
		# Adjust report to nearest hour

		#if( $ob_min >= 30 ) {
		#	$hour = sprintf( "%02d", ( $ob_hour + 1 ) % 24 );
		#	$yyyymmdd = thetime( "tomorrow" )
		#			if( $hour eq "00" ) ;
		#}
		#else
		#{

			$hour = sprintf( "%02d", $ob_hour ) ;

		#}

		$time_obs = thetime( "obs" ) ;
		$time_nominal = thetime( "nominal" ) ;

		# check on sanity of obs time.
		if ( ! $yymm ) { # realtime mode
    
    			($cyear, $cmon, $cday, $chour, $cmin) = (gmtime())[5,4,3,2,1] ;
    			$ctime = timegm_nocheck ( 0, $cmin, $chour, $cday, $cmon, $cyear );
    			#$tdiff = $ctime - $time_nominal;
    			$tdiff = $ctime - $time_obs;
    			if ($tdiff > $rtmax_age) {
				logmsg("Report too old: $stn_name $rpt_time ($tdiff seconds)\n");
    				next;
    			}
    			elsif ($tdiff < -7200) {
				logmsg("Report in the future: $stn_name $rpt_time\n");
    				next;
    			}
		}


		#  signal new decoded report
	        print "PARSING METAR REPORT:\n" if ($verbose);
		print "report=$report\n"   if ($verbose) ;
	        print "remarks=$remarks\n" if ($verbose) ;

		$AUTO = 1 if( s#AUTO\s*## ) ;
		# get wind direction and speed
		if( s#(E|W|N|S)?(\d{3}|VRB)(\d{2,3})(G)?(\d{2,3})?(KMH|KT|MPS|MPH)\s*## ) {
			if( $2 eq "VRB" ) {
				$VRB = 1 ;
			} else {
				$DIR = $2 ;
			}
			$SPD = $3 ;
			$GUST = $5 if( $4 eq "G" ) ;
			$UNITS = $6 ;
			$UNITS = padstr( $UNITS, 3 ) ;
		}
		# get min|max wind direction (Changed to look for single space
                # after field so that it does not confuse this with RVR below)
		if( s#(\d{3})V(\d{3}) ## ) {
			$DIRmin = $1 ;
			$DIRmax = $2 ;
		}
		# get visibility
		$prevail_VIS_SM = 0.0 if( s#M1/4SM\s*|<1/4SM\s*## ) ;
		$prevail_VIS_KM = 0.0 if( s#M1/4KM\s*|<1/4KM\s*## ) ;
		$plus_VIS_SM = 1 if( s#P(\d{1,3})SM\s*#$1SM # ) ;
		$plus_VIS_KM = 1 if( s#P(\d{1,3})KM\s*#$1KM # ) ;

		if( s#(\d{1,4}) (\d{1,3})/(\d{1,3})(SM|KM)\s*## ) {
			$prevail_VIS_SM = $1 + ( $2 / $3 ) if( $4 eq "SM" && $3 > 0) ;
			$prevail_VIS_KM = $1 + ( $2 / $3 ) if( $4 eq "KM" && $3 > 0) ;

		} elsif( s#(\d{1,3})/(\d{1,3})(KM|SM)\s*## ) {
			$prevail_VIS_SM = $1 / $2  if( $3 eq "SM" && $2 > 0) ;
			$prevail_VIS_KM = $1 / $2  if( $3 eq "KM" && $2 > 0) ;

		} elsif( s#(\d{1,4})(KM|SM)\s*## ) {
			$prevail_VIS_SM = $1 if( $2 eq "SM" ) ;
			$prevail_VIS_KM = $1 if( $2 eq "KM" ) ;
		# Some report in meters (MMMM) where 9999 is presumably unlimited
		} elsif( s#^9999\s*## ) {
			$prevail_VIS_KM = 16.09 ; # 10mi * 1.609km/mi
		} elsif( s#(^\d{4})\s*## ) {
			$prevail_VIS_KM = $1/1000. ;
		} elsif( s#(\d{4})((NE)|(NW)|(SE)|(SW)|(N)|(S)|(E)|(W))\s*## ) {
			$prevail_VIS_M = $1 ;
			$VIS_dir = $2 ;
			$VIS_dir = padstr( $VIS_dir, 2 ) ;
		}
		# Ceiling And Visibility OK
		$CAVOK = 1 if( s#CAVOK\s*## || s#C A V O K\s*##) ;
		# runway decoding here
		$RVRNO = 1 if( s#RVRNO\s*## ) ;
		for( $i = 0; $i < 4; $i++ ) {
			if( s#R(\d{2})(R|L|C)?/(M|P)?(\d{4})V?(M|P)?(\d{4})?(FT|N|D)?\s*## ){
				if( $i == 0 ) {
					@RV_designator = ( $S3, $S3, $S3, $S3 );
					@RV_above_max = ( $F, $F, $F, $F ) ;
					@RV_below_min = ( $F, $F, $F, $F ) ;
					@RV_vrbl = ( $F, $F, $F, $F ) ;
					@RV_min = ( $F, $F, $F, $F ) ;
					@RV_max = ( $F, $F, $F, $F ) ;
					@RV_visRange = ( $F, $F, $F, $F ) ;
				}
				$RV_designator[ $i ] = "$1$2" ;
				$RV_designator[ $i ] = 
					padstr( $RV_designator[ $i ], 3 ) ;
				$RV_above_max[ $i ] = 1 
					if( $3 eq "P" || $5 eq "P" ) ;
				$RV_below_min[ $i ] = 1 
					if( $3 eq "M" || $5 eq "M" ) ;
				$RV_vrbl[ $i ] = 1 if( $6 ne "" ) ;
				if( $RV_vrbl[ $i ] ) {
					$RV_min[ $i ] = $4 * 1;
					$RV_max[ $i ] = $6 * 1;
				} else {
					$RV_visRange[ $i ] = $4 * 1;
				}
				$i++ ;
			} else {
				last ;
			}
		}

		# Get weather conditions
		for( $i = 0; $i < 4; $i++ ) {
			# code table 4678
			if( s#(\+|-|VC|PR)?(MI|BC|DR|BL|SH|TS|FZ)?(DZ|RA|SN|SG|IC|PE|PL|GR|GS|UP)?(BR|FG|FU|VA|DU|SA|HZ|PY)?(PO|SQ|FC|SS|DS)?\s*## ) {
				last unless "$1$2$3$4$5" ;
				$WX .= "$1$2$3$4$5 " ;
			} else {
				last ;
			}
		}
		$WX = padstr( $WX, 32 ) 
			if( defined( $WX ) ) ;

		# Interpret cloud conditions

                
		# Look for CLR or SKC
                @cloud_type = ( "$1\0", $S4, $S4, $S4, $S4, $S4 ) if( s#(CLR|SKC)\s*## ) ;

		# Turn Vertical visibility (VV) into OVC
		s#VV(\d{3}\s*)#OVC$1# ;

		for( $i = 0; $i < 6; $i++ ) {
			# layers up to 6
			if( s#(\+|-)?(OVC|SCT|FEW|BKN)(\d{3})(\w{1,3})?\s*## ){
				if( $i == 0 ) {
					@cloud_type =
						( $S4,$S4,$S4,$S4,$S4,$S4 ) ;
					@cloud_hgt = ( $F,$F,$F,$F,$F,$F ) ;
					@cloud_meters = ( $F,$F,$F,$F,$F,$F ) ;
					@cloud_phenom =
						( $S4,$S4,$S4,$S4,$S4,$S4 ) ;
				}
#				$cloud_type[ $i ] = padstr( "$1$2", 4 ) ;
				$cloud_type[ $i ] = padstr( "$2", 4 ) ;
				$cloud_hgt[ $i ] = $3 * 100 ;
				$cloud_meters[ $i ] = cloud_hgt2_meters( $3 ) ;
				$cloud_phenom[ $i ] = padstr( $4, 4 ) if( $4 ) ;
			} else {
				last ;
			}
		}
		# get temperature and dew point
		if( s# (M)?(\d{2})/(M)?(\d{2})?\s*## || s#^(M)?(\d{2})/(M)?(\d{2})?\s*## ) {
			$T = $2 ;
			$T *= -1 if( $1 ) ;
			$TD = $4 if( $4 ) ;
			$TD *= -1 if( $3 ) ;
		}
		# get Altimeter settings
		if( s#(A|Q)(\d{4}\.?\d?)\s*## ) {
			if( $1 eq "A" ) {
				$inches_ALTIM = $2 * 0.01 ;
			} else {
				$hectoPasc_ALTIM = $2 ;
			}
		}
		$NOSIG = 1 if( s#NOSIG## ) ;
		# get remarks
		if( defined( $remarks ) ) {
			doRemarks( "$remarks " ) ;
		} 

		# debug output
		printvars() if( $verbose ) ;

		# we have a legal report, open a Netcdf file
		if (defined($cdlfile)) {
		
                  $report = padstr( $report, 128 ) ;
                  $remarks =  padstr( $remarks, 128 ) if( defined( $remarks ) ) ;
		  
		  $result = doNet( $hour ) ;

		  if( $result ) { # OK to write data

			# If this is a replacement, get old record, then set variables
			if (defined($reportslist{ "$stn_name $rpt_time" })) {
	    			( $record, $ntime ) =
				    split( " ", $reportslist{ "$stn_name $rpt_time" }) ;
				$result = NetCDF::recget( $ncid, $record, \@oldrec);
				setvars(@oldrec) ;
			} else { # new report, use next record number, set variables
				$record = $recnum ;
				$reportslist{ "$stn_name $rpt_time" } = "$record $time_nominal" ;
				$recnum++ ;
				setvars(@rec) ;
			}
			# enter report data into NetCDF file
			if ( defined($record)) {
			    $result = NetCDF::recput( $ncid, $record, [ @dataref ]);
			    if( $result ) {
				logmsg("NetCDF::recput result = $result\n") ;
				chop $report 
					while( index( $report, "\0" ) != -1 ) ;
				chop $remarks 
					while( index( $remarks, "\0" ) != -1 ) ;
				logmsg("$ncfile\n$report\n$remarks\n") ;
			    }
			}
                  }
		}
		
	} # end foreach report

	# Sync output files after each bulletin is processed
	if (defined($cdlfile)) {
	  syncFiles();
	}
	
	
} # end while( 1 )

exit( 0 ) ; #should never get here


# Interpet Remarks
sub doRemarks {

( $_ ) = @_ ;

# get Tornado data if present
if( s#(TORNADO\w{0,2}|WATERSPOUTS*|FUNNEL CLOUDS*)\s+## ) { 
	$TornadicType = padstr( $1, 15 ) ;
	if( s#(B|E)(\d\d)(\d\d)?\s+## ) {
		if( $1 eq "B" ) {
			$BTornadic_hh = $2 ;
			$BTornadic_mm = $3 if( defined( $3 ) ) ;
		} else {
			$ETornadic_hh = $2 ;
			$ETornadic_mm = $3 if( defined( $3 ) ) ;
		}
	}
	$TornadicLOC = padstr( $1, 10 )
		if( s#^(DSNT|VCY STN|VC STN|VCY|VC)\s+## ) ;
	$TornadicDIR = padstr( $1, 2 )
		if( s#^(NE|NW|SE|SW|N|S|E|W)\s+## ) ;
}
# get Automated reports
$AUTOindicator = padstr( $1, 4 )
	 if( s#(A01|A01A|A02|A02A|AO1|AO1A|AO2|AO2A|AOA)\s+## ) ;

# get Peak winds
if( s#PK WND (\d{3})(\d{1,3})/(\d\d)?(\d\d)\s+## ) {
	$PKWND_dir = $1 ;
	$PKWND_spd = $2 ;
	$PKWND_hh = $3 if( defined( $3 ) ) ;
	$PKWND_mm = $4 ;
}
# get Wind shift
if( s#WSHFT (\d\d)?(\d\d)\s+## ) {
	$WshfTime_hh = $1 if( defined( $1 ) );
	$WshfTime_mm = $2 ;
}
# get FROPO ( wind shift because of frontal passage )
$Wshft_FROPA = 1 if( s#FROPA\s+## ) ;

# Tower visibility
if( s#TWR (VIS|VSBY) (\d{1,3}) (\d{1,2})/(\d{1,2})\s+## ) {
	$VIS_TWR = $2 + ( $3 / $4 ) ;
} elsif( s#TWR (VIS|VSBY) (\d{1,2})/(\d{1,2})\s+## ) {
	$VIS_TWR = ( $2 / $3 ) ;
} elsif( s#TWR (VIS|VSBY) (\d{1,3})\s+## ) {
	$VIS_TWR = $2 ;
}
# Surface visibility
if( s#SFC (VIS|VSBY) (\d{1,3}) (\d{1,2})/(\d{1,2})\s+## ) {
	$VIS_SFC = $2 + ( $3 / $4 ) ;
} elsif( s#SFC (VIS|VSBY) (\d{1,2})/(\d{1,2})\s+## ) {
	$VIS_SFC = ( $2 / $3 ) ;
} elsif( s#SFC (VIS|VSBY) (\d{1,3})\s+## ) {
	$VIS_SFC = $2 ;
}
# Variable visibility
if( s#(VIS|VSBY) (\d{1,3}) (\d{1,2})/(\d{1,2})V(\d{1,3}) (\d{1,2})/(\d{1,2})\s+## ) {
	$VISmin = $2 + ( $3 / $4 ) ;
	$VISmax = $5 + ( $6 / $7 ) ;
} elsif( s#(VIS|VSBY) (\d{1,3})V(\d{1,3}) (\d{1,2})/(\d{1,2})\s+## ) {
	$VISmin = $2 ;
	$VISmax = $3 + ( $4 / $5 ) ;
} elsif( s#(VIS|VSBY) (\d{1,2})/(\d{1,2})V(\d{1,3}) (\d{1,2})/(\d{1,2})\s+## ) {
	$VISmin = ( $2 / $3 ) ;
	$VISmax = $4 + ( $5 / $6 ) ;
} elsif( s#(VIS|VSBY) (\d{1,3}) (\d{1,2})/(\d{1,2})V(\d{1,3})\s+## ) {
	$VISmin = $2 + ( $3 / $4 ) ;
	$VISmax = $5 ;
} elsif( s#(VIS|VSBY) (\d{1,3})V(\d{1,3})\s+## ) {
	$VISmin = $2 ;
	$VISmax = $3 ;
} elsif( s#(VIS|VSBY) (\d{1,2})/(\d{1,2})V(\d{1,3})\s+## ) {
	$VISmin = ( $2 / $3 ) ;
	$VISmax = $4 ;
}
# Second site visiblity
if( s#(VIS|VSBY) (\d{1,3}) (\d{1,2})/(\d{1,2}) (RY\d{1,2})\s+## ) {
	$VIS_2ndSite = $2 + ( $3 / $4 ) ;
	$VIS_2ndSite_LOC = padstr( $5, 10 ) ;
} elsif( s#(VIS|VSBY) (\d{1,3}) (RY\d{1,2})\s+## ) {
	$VIS_2ndSite = $2 ;
	$VIS_2ndSite_LOC = padstr( $3, 10 ) ;
} elsif( s#(VIS|VSBY) (\d{1,2})/(\d{1,2}) (RY\d{1,2})\s+## ) {
	$VIS_2ndSite = ( $2 / $3 ) ;
	$VIS_2ndSite_LOC = padstr( $4, 10 ) ;
}
# Lightning data ( Occasional,Frequent,Continuous) and
#	(Cloud-Ground,In-Cloud,Cloud-Cloud,Cloud-Air)
if( s#(OCNL|FRQ|CNS) LTG\s?(CG|IC|CC|CA)\s?(DSNT|AP|VCY STN|VCNTY STN)?\s?(NE|NW|SE|SW|N|S|E|W)?\s+## ) {
	$LTG_OCNL = 1 if( $1 eq "OCNL" ) ;
	$LTG_FRQ = 1 if( $1 eq "FRQ" ) ;
	$LTG_CNS = 1 if( $1 eq "CNS" ) ;
	$LTG_CG = 1 if( $2 eq "CG" ) ;
	$LTG_IC = 1 if( $2 eq "IC" ) ;
	$LTG_CC = 1 if( $2 eq "CC" ) ;
	$LTG_CA = 1 if( $2 eq "CA" ) ;

	$LTG_DSNT = 1 if( $3 eq "DSNT" ) ;
	$LTG_AP = 1 if( $3 eq "AP" ) ;
	$LTG_VcyStn = 1 if( $3 eq "VCY STN" || $3 eq "VCNTY STN" ) ;
	$LTG_DIR = padstr( $4, 2 ) if( defined( $4 ) ) ;
}
# get min/max for Variable Ceiling
if( s#CIG (\d{1,4})V(\d{1,4})\s+## ) {
	$Ceiling_min = $1 ;
	$Ceiling_max = $2 ;
}
# ? about SKY condition at 2nd location
# get 2nd site ceiling and location
if( s#CIG (\d{3}) (RY\d{1,2})\s+## ) {
	$CIG_2ndSite_meters = $1 * 10 ;
	$CIG_2ndSite_LOC = $2 ;
}
# Presure falling rapidly
$PRESFR = 1 if( s#PRESFR/?\s+## ) ;

# Presure rising rapidly
$PRESRR = 1 if( s#PRESRR/?\s+## ) ;

# Sea-Level presure
$SLPNO = 1 if( s#SLPNO\s+## ) ;
if( s#SLP\s?(\d{3})\s+## ) {
	if( $1 >= 550 ) {
		$SLP = $1 / 10. + 900. ;
	} else {
		$SLP =  $1 / 10. + 1000. ;
	}
}
# Sector visibility
if( s#(VIS|VSBY) (NE|NW|SE|SW|N|S|E|W)(\d{1,3}) (\d{1,2})/(\d{1,2})\s+## ) {
	$SectorVIS_DIR = padstr( $2, 2 ) ;
	$SectorVIS = $3 + ( $4 / $5 ) ;
} elsif( s#(VIS|VSBY) (NE|NW|SE|SW|N|S|E|W) (\d{1,2})/(\d{1,2})\s+## ) {
	$SectorVIS_DIR = padstr( $2, 2 ) ;
	$SectorVIS = ( $3 / $4 ) ;
} elsif( s#(VIS|VSBY) (NE|NW|SE|SW|N|S|E|W)(\d{1,3})\s+## ) {
	$SectorVIS_DIR = padstr( $2, 2 ) ;
	$SectorVIS = $3 ;
}
# Hailstone activity and size
$GR = 1 if( s#GS\s+## ) ;
if( s#GR M1/4\s+## ) {
	$GR = 1 ;
	$GRsize = 1 / 8 ;
} elsif( s#GR (\d{1,3}) (\d{1,2})/(\d{1,2})\s+## ) {
	$GR = 1 ;
	$GRsize = $1 + ( $2 / $3 ) ;
} elsif( s#GR (\d{1,2})/(\d{1,2})\s+## ) {
	$GR = 1 ;
	$GRsize = ( $1 / $2 ) ;
} elsif( s#GR (\d{1,3})\s+## ) {
	$GR = 1 ;
	$GRsize = $1 ;
}
# VIRGA activity
if( s#VIRGA (DSNT )?(NE|NW|SE|SW|N|S|E|W)?\s+## ) {
	$VIRGA = 1 ;
	$VIRGAdir = padstr( $2, 2 ) if( $2 ) ;
}
# Surface-based Obscuring Phenomena  SfcObscuration weather conditions
# code table 4678
if( s#-X(VC|PR)?(MI|BC|DR|BL|SH|TS|FZ)?(DZ|RA|SN|SG|IC|PE|PL|GR|GS|UP)?(BR|FG|FU|VA|DU|SA|HZ|PY)?(PO|SQ|FC|SS|DS)?(\d)\s+## ) {
		$SfcObscuration = padstr( "$1$2$3$4$5", 8 ) ;
		$OctsSkyObscured = $6 ;
}
# get Ceiling_est or Ceiling height
$CIGNO = 1 if( s#CIGNO\s+## ) ;
if( s#CIG(E)?(\d{3})\s+## ) {
	if( $1 eq "E" ) {
		$Ceiling_est = $2 * 100 ;
	} else {
		$Ceiling = $2 * 100 ;
	}
}
# Variable Sky conditions
if( s#(FEW|SCT|BKN|OVC)(\d{3})? V (FEW|SCT|BKN|OVC)\s+## ) {
	$VrbSkyBelow = $1 ;
	$VrbSkyLayerHgt = $2 * 100 if( defined( $2 ) ) ;
	$VrbSkyAbove = $3 ;
}
# Significant Cloud Types
if( s#(CB|CBMAM|TCU|ACC|SCSL|ACSL|ROTOR CLD|ROPE|ROPE CLD)\s+## ) {
	$Sign_cloud = padstr( $1, 10 ) ;
	$Sign_dist = padstr( $1, 10 ) 
	if( s#^(VCNTY STN|VCY STN|VC STN|VCY|VC|DSNT|OMT)\s+## ) ;
	$Sign_dir = padstr( "$1$2$3", 10 ) 
	if( s#^(NE|NW|SE|SW|N|S|E|W)(\-| MOV )?(NE|NW|SE|SW|N|S|E|W)?/?\s+## ) ;
}
# Obscuring Phenomena Aloft
if( s#(VC|PR)?(MI|BC|DR|BL|SH|TS|FZ)?(DZ|RA|SN|SG|IC|PE|PL|GR|GS|UP)?(BR|FG|FU|VA|DU|SA|HZ|PY)?(PO|SQ|FC|SS|DS)? (FEW|SCT|BKN|OVC)(\d{3})\s+## ) {
	$ObscurAloft = padstr( "$1$2$3$4$5", 8 ) ;
	$ObscurAloftSkyCond = $6 ;
	$ObscurAloftHgt = $7 * 100 ;
}
# Air craft mishap  ACFTMSHP
$ACFTMSHP = 1 if( s#\(?ACFT\s?MSHP\)?\s+## ) ;

# No changes in weather conditions until next report  NOSPECI
$NOSPECI = 1 if( s#NOSPECI\s+## ) ;

# This is first report of the day  FIRST
$FIRST = 1 if( s#FIRST\s+## ) ;

# This is last report in observation coverage  LAST
$LAST = 1 if( s#LAST\s+## ) ;

# Remove any 5-digit coded remarks from Mexican stations. They don't use
# them the same as the METAR documentation states
if ($stn_name =~ /^MM/) {
	s# (\d{5})\s+# # ;
}

# Cloud Types
if( s# 8/(\d|/)(\d|/)(\d|/)/?\s+# # ) {
	$Cloud_low = $1 ;
	$Cloud_medium = $2 ;
	$Cloud_high = $3 ;
}
# Snow Increasing Rapidly   SNINCR
if( s#SNINCR (\d{1,3})/(\d{1,3})\s+## ) {
	$SNINCR = $1 ;
	$SNINCR_TotalDepth = $2 ;
}
# Snow depth on ground
if( s# 4/(\d{1,3})\s+# # ) {
	$SN_depth = $1 ;
}
# Water equivalent of snow on ground
$SN_waterequiv = $1 / 10 if( s# 933(\d{3})\s+# # ) ;

# Duration of sunshine
if( s# 98(\d{1,3}|///)\s+# # ) {
	if( $1 eq "///" ) {
		$SunSensorOut = 1 ;
	} else {
		$SunShineDur = $1 ;
	}
}
# Hourly precipitation amount
$PRECIP_hourly = $1 / 100 if( s# P(\d{1,5})\s+## ) ;

# Precipitation amount
if( s# 6(\d{4}|////)\s+# # ) {
	$PRECIP_amt = $1 / 100 if( $1 ne "////" ) ;
}
# 24 Hour Precipitation amount
if( s# 7(\d{4}|////)\s+# # ) {
	$PRECIP_24_amt = $1 / 100 if( $1 ne "////" ) ;
}
# Temperature and Dew Point
if( s#T(0|1)(\d{3})(0|1)?(\d{3})?\s+## ) {
	if( $1 == 0 ) {
		$T_tenths = 0.1 * $2 ;
	} else {
		$T_tenths = -0.1 * $2 ;
	}
	if( defined( $3 ) && $3 == 0 ) {
		$TD_tenths = 0.1 * $4 ;
	} elsif( defined( $3 ) && $3 == 1 ) {
		$TD_tenths = -0.1 * $4 ;
	}
}
# else {
#	$T_tenths = $T if( defined( $T ) ) ;
#	$TD_tenths = $TD if( defined( $TD ) ) ;
#}
# Maximum Temperature
if( s# 1(0|1|/)(\d{3}|///)\s+# # ) {
	$Tmax = $2 / 10 if( $2 ne "///" ) ;
	$Tmax *= -1.0 if( $1 == 1 ) ;
}
# Minimum Temperature
if( s# 2(0|1|/)(\d{3}|///)\s+# # ) {
	$Tmin = $2 / 10 if( $2 ne "///" ) ;
	$Tmin *= -1.0 if( $1 == 1 ) ;
}
# 24-Hour Maximum and Minimum Temperature
if( s# 4(0|1|/)(\d{3}|///)(0|1|/)(\d{3}|///)\s+# # ) {
	$Tmax24 = $2 / 10 if( $2 ne "///" ) ;
	$Tmax24 *= -1.0 if( $1 == 1 ) ;
	$Tmin24 = $4 / 10 if( $4 ne "///" ) ;
	$Tmin24 *= -1.0 if( $3 == 1 ) ;
}

# Presure Tendency
if( s# 5(0|1|2|3|4|5|6|7|8)(\d{3}/?|///)\s+# # ) {
	$char_Ptend = $1 ;
	$Ptend = $2 / 10 if( $2 ne "///" ) ;
}
# Sensor Status Indicators
# precipitation amount not available  PNO
$PNO = 1 if( s#PNO\s+## ) ;

# Present weather indicator not working  PWINO
$PWINO = 1 if( s#PWINO\s+## ) ;

# Freezing Rain sensor not working  FZRANO
$FZRANO = 1 if( s#FZRANO\s+## ) ;

# Lightning detection sensor not working  TSNO
$TSNO = 1 if( s#TSNO\s+## ) ;

# Maintenance is needed on system Indicator
$maintIndicator = 1 if( s#\$\s+## ) ;

# Get Recent weather conditions with Beginning and Ending times, moved here
# because they may match other stuff we want.
# code table 4678

for( $i = 0; $i < 3; $i++ ) {
	if( s#(\+|-|VC|PR)?(MI|BC|DR|BL|SH|TS|FZ)?(DZ|RA|SN|SG|IC|PE|PL|GR|GS|UP)?(BR|FG|FU|VA|DU|SA|HZ|PY)?(PO|SQ|FC|SS|DS)?(B\d{2,4})(E\d{2,4})(B\d{2,4})?## ) {
		initReWX() if( $i == 0 ) ;
		$Recent_WX[ $i ] = padstr( "$1$2$3$4$5", 8 ) ;
		if( length( $6 ) == 5 ) {
			$Recent_WX_Bhh[ $i ] = substr( $6, 1, 2 ) * 1 ;
			$Recent_WX_Bmm[ $i ] = substr( $6, 3, 2 ) * 1 ;
		} elsif( length( $6 ) == 3 ) {
			$Recent_WX_Bmm[ $i ] = substr( $6, 1, 2 ) * 1 ;
		}
		if( length( $7 ) == 5 ) {
			$Recent_WX_Ehh[ $i ] = substr( $7, 1, 2 ) * 1 ;
			$Recent_WX_Emm[ $i ] = substr( $7, 3, 2 ) * 1 ;
		} elsif( length( $7 ) == 3 ) {
			$Recent_WX_Emm[ $i ] = substr( $7, 1, 2 ) * 1 ;
		}
	} elsif( s#(\+|-|VC|PR)?(MI|BC|DR|BL|SH|TS|FZ)?(DZ|RA|SN|SG|IC|PE|PL|GR|GS|UP)?(BR|FG|FU|VA|DU|SA|HZ|PY)?(PO|SQ|FC|SS|DS)?(E\d{2,4})(B\d{2,4})(E\d{2,4})?## ) {
		initReWX() if( $i == 0 ) ;
		$Recent_WX[ $i ] = padstr( "$1$2$3$4$5", 8 ) ;
		if( length( $6 ) == 5 ) {
			$Recent_WX_Ehh[ $i ] = substr( $6, 1, 2 ) * 1 ;
			$Recent_WX_Emm[ $i ] = substr( $6, 3, 2 ) * 1 ;
		} elsif( length( $6 ) == 3 ) {
			$Recent_WX_Emm[ $i ] = substr( $6, 1, 2 ) * 1 ;
		}
		if( length( $7 ) == 5 ) {
			$Recent_WX_Bhh[ $i ] = substr( $7, 1, 2 ) * 1 ;
			$Recent_WX_Bmm[ $i ] = substr( $7, 3, 2 ) * 1 ;
		} elsif( length( $7 ) == 3 ) {
			$Recent_WX_Bmm[ $i ] = substr( $7, 1, 2 ) * 1 ;
		}
	} elsif( s#(\+|-|VC|PR)?(MI|BC|DR|BL|SH|TS|FZ)?(DZ|RA|SN|SG|IC|PE|PL|GR|GS|UP)?(BR|FG|FU|VA|DU|SA|HZ|PY)?(PO|SQ|FC|SS|DS)?(B|E)(\d{2,4})## ) {
		initReWX() if( $i == 0 ) ;
		$Recent_WX[ $i ] = padstr( "$1$2$3$4$5", 8 ) ;
		if( $6 eq "B" && ( length( $7 ) == 4 )) {
			$Recent_WX_Bhh[ $i ] = substr( $7, 0, 2 ) * 1 ;
			$Recent_WX_Bmm[ $i ] = substr( $7, 2, 2 ) * 1 ;
		} elsif( $6 eq "B" && ( length( $7 ) == 2 )) {
			$Recent_WX_Bmm[ $i ] = substr( $7, 0, 2 ) * 1 ;
		} elsif( $6 eq "E" && ( length( $7 ) == 4 )) {
			$Recent_WX_Ehh[ $i ] = substr( $7, 0, 2 ) * 1 ;
			$Recent_WX_Emm[ $i ] = substr( $7, 2, 2 ) * 1 ;
		} elsif( $6 eq "E" && ( length( $7 ) == 2 )) {
			$Recent_WX_Emm[ $i ] = substr( $7, 0, 2 ) * 1 ;
		}
	} else {
		last ;
	}
}

# Plain Language remarks includes Volcanic eruptions
#$PlainText = padstr( $1, 128 ) if( s#(\w.*)## ) ;

#print "Not decoded. $_  \n\n" if( $_ ) ;
} # end doRemarks

# convert cloud height to  meters
sub cloud_hgt2_meters {

my( $value, $meters ) ;
( $value ) = @_ ;

if( $value == 999 ) {
	$meters = 30000 ;
} else {
	$meters = 30 * $value ;
}
} # end cloud_hgt2_meters

# undefine all report vars
sub undefinevars
{
# report vars
undef( $wmo_id ) ;
undef( $lat ) ;
undef( $lon ) ;
undef( $elev ) ;
undef( $ob_hour ) ;
undef( $ob_min ) ;
undef( $ob_day ) ;
undef( $time_obs ) ;
undef( $time_nominal ) ;
undef( $AUTO ) ;
undef( $GUST ) ;
undef( $VRB ) ;
undef( $DIR ) ;
undef( $SPD ) ;
undef( $UNITS ) ;
undef( $DIRmin ) ;
undef( $DIRmax ) ;
undef( $prevail_VIS_SM ) ;
undef( $prevail_VIS_KM ) ;
undef( $plus_VIS_SM ) ;
undef( $plus_VIS_KM ) ;
undef( $prevail_VIS_M ) ;
undef( $VIS_dir ) ;
undef( $CAVOK ) ;
undef( $RVRNO ) ;
undef( @RV_designator ) ;
undef( @RV_above_max ) ;
undef( @RV_below_min ) ;
undef( @RV_vrbl ) ;
undef( @RV_min ) ;
undef( @RV_max ) ;
undef( @RV_visRange ) ;
undef( $WX ) ;
undef( $vert_VIS ) ;
undef( @cloud_type ) ;
undef( @cloud_hgt ) ;
undef( @cloud_meters ) ;
undef( @cloud_phenom ) ;
undef( $T ) ;
undef( $TD ) ;
undef( $inches_ALTIM ) ;
undef( $hectoPasc_ALTIM ) ;
undef( $NOSIG ) ;

# remarks vars

undef( $TornadicType ) ;
undef( $TornadicLOC ) ;
undef( $TornadicDIR ) ;
undef( $BTornadic_hh ) ;
undef( $BTornadic_mm ) ;
undef( $ETornadic_hh ) ;
undef( $ETornadic_mm ) ;
undef( $AUTOindicator ) ;
undef( $PKWND_dir ) ;
undef( $PKWND_spd ) ;
undef( $PKWND_hh ) ;
undef( $PKWND_mm ) ;
undef( $WshfTime_hh ) ;
undef( $WshfTime_mm ) ;
undef( $Wshft_FROPA ) ;
undef( $VIS_TWR ) ;
undef( $VIS_SFC ) ;
undef( $VISmin ) ;
undef( $VISmax ) ;
undef( $VIS_2ndSite ) ;
undef( $VIS_2ndSite_LOC ) ;
undef( $LTG_OCNL ) ;
undef( $LTG_FRQ ) ;
undef( $LTG_CNS ) ;
undef( $LTG_CG ) ;
undef( $LTG_IC ) ;
undef( $LTG_CC ) ;
undef( $LTG_CA ) ;
undef( $LTG_DSNT ) ;
undef( $LTG_AP ) ;
undef( $LTG_VcyStn ) ;
undef( $LTG_DIR ) ;

# Recent_WX ReWX[3];
undef( @Recent_WX ) ;
undef( @Recent_WX_Bhh ) ;
undef( @Recent_WX_Bmm ) ;
undef( @Recent_WX_Ehh ) ;
undef( @Recent_WX_Emm ) ;

undef( $Ceiling_min ) ;
undef( $Ceiling_max ) ;

undef( $CIG_2ndSite_meters ) ;
undef( $CIG_2ndSite_LOC ) ;
undef( $PRESFR ) ;
undef( $PRESRR ) ;
undef( $SLPNO ) ;
undef( $SLP ) ;
undef( $SectorVIS_DIR ) ;
undef( $SectorVIS ) ;
undef( $GR ) ;
undef( $GRsize ) ;
undef( $VIRGA ) ;
undef( $VIRGAdir ) ;
undef( $SfcObscuration ) ;
undef( $OctsSkyObscured ) ;
undef( $CIGNO ) ;
undef( $Ceiling_est ) ;
undef( $Ceiling ) ;
undef( $VrbSkyBelow ) ;
undef( $VrbSkyLayerHgt ) ;
undef( $VrbSkyAbove ) ;
undef( $Sign_cloud ) ;
undef( $Sign_dist ) ;
undef( $Sign_dir ) ;
undef( $ObscurAloft ) ;
undef( $ObscurAloftSkyCond ) ;
undef( $ObscurAloftHgt ) ;
undef( $ACFTMSHP ) ;
undef( $NOSPECI ) ;
undef( $FIRST ) ;
undef( $LAST ) ;
undef( $Cloud_low ) ;
undef( $Cloud_medium ) ;
undef( $Cloud_high ) ;
undef( $SNINCR ) ;
undef( $SNINCR_TotalDepth ) ;
undef( $SN_depth ) ;
undef( $SN_waterequiv ) ;
undef( $SunSensorOut ) ;
undef( $SunShineDur ) ;
undef( $PRECIP_hourly ) ;
undef( $PRECIP_amt ) ;
undef( $PRECIP_24_amt ) ;
undef( $T_tenths ) ;
undef( $TD_tenths ) ;
undef( $Tmax ) ;
undef( $Tmin ) ;
undef( $Tmax24 ) ;
undef( $Tmin24 ) ;
undef( $char_Ptend ) ;
undef( $Ptend ) ;
undef( $PNO ) ;
undef( $PWINO ) ;
undef( $FZRANO ) ;
undef( $TSNO ) ;
undef( $maintIndicator ) ;
undef( $PlainText ) ;

} # end of undefinevARS

# setvars into record
sub setvars {

undef( @dataref ) ;

@dataref = @_ ;

# report vars
$dataref[ $W[ 0 ] ] = \$rep_type if( $W[ 0 ] != -1 ) ;
$dataref[ $W[ 1 ] ] = \$stn_name ;
$dataref[ $W[ 2 ] ] = \$wmo_id if( $W[ 2 ] && defined( $wmo_id ) ) ;
$dataref[ $W[ 3 ] ] = \$lat if( $W[ 3 ] && defined( $lat ) ) ;
$dataref[ $W[ 4 ] ] = \$lon if( $W[ 4 ] && defined( $lon ) ) ;
$dataref[ $W[ 5 ] ] = \$elev if( $W[ 5 ] && defined( $elev ) ) ;
$dataref[ $W[ 6 ] ] = \$ob_hour if( $W[ 6 ] ) ;
$dataref[ $W[ 7 ] ] = \$ob_min if( $W[ 7 ] ) ;
$dataref[ $W[ 8 ] ] = \$ob_day if( $W[ 8 ] && defined( $ob_day ) ) ;
$dataref[ $W[ 9 ] ] = \$time_obs if( $W[ 9 ] && defined( $time_obs ) ) ;
$dataref[ $W[ 10 ] ] = \$time_nominal if( $W[ 10 ] && defined( $time_nominal ));
$dataref[ $W[ 11 ] ] = \$AUTO if( $W[ 11 ] && defined( $AUTO ) ) ;
$dataref[ $W[ 12 ] ] = \$UNITS if( $W[ 12 ] && defined( $UNITS ) ) ;
$dataref[ $W[ 13 ] ] = \$DIR if( $W[ 13 ] && defined( $DIR ) ) ;
$dataref[ $W[ 14 ] ] = \$SPD if( $W[ 14 ] && defined( $SPD ) ) ;
$dataref[ $W[ 15 ] ] = \$GUST if( $W[ 15 ] && defined( $GUST ) ) ;
$dataref[ $W[ 16 ] ] = \$VRB if( $W[ 16 ] && defined( $VRB ) ) ;
$dataref[ $W[ 17 ] ] = \$DIRmin if( $W[ 17 ] && defined( $DIRmin ) ) ;
$dataref[ $W[ 18 ] ] = \$DIRmax if( $W[ 18 ] && defined( $DIRmax ) ) ;
$dataref[ $W[ 19 ] ] = \$prevail_VIS_SM if( $W[ 19 ] && 
	defined( $prevail_VIS_SM ) ) ;
$dataref[ $W[ 20 ] ] = \$prevail_VIS_KM if( $W[ 20 ] && 
	defined( $prevail_VIS_KM ) ) ;
$dataref[ $W[ 21 ] ] = \$plus_VIS_SM if( $W[ 21 ] && defined( $plus_VIS_SM ) ) ;
$dataref[ $W[ 22 ] ] = \$plus_VIS_KM if( $W[ 22 ] && defined( $plus_VIS_KM ) ) ;
$dataref[ $W[ 23 ] ] = \$prevail_VIS_M if( $W[ 23 ] && 
	defined( $prevail_VIS_M ) ) ;
$dataref[ $W[ 24 ] ] = \$VIS_dir if( $W[ 24 ] && defined( $VIS_dir ) ) ;
$dataref[ $W[ 25 ] ] = \$CAVOK if( $W[ 25 ] && defined( $CAVOK ) ) ;
$dataref[ $W[ 26 ] ] = \$RVRNO if( $W[ 26 ] && defined( $RVRNO ) ) ;
$dataref[ $W[ 27 ] ] = \@RV_designator if( $W[ 27 ] && 
	defined( @RV_designator ) ) ;
$dataref[ $W[ 28 ] ] = \@RV_above_max if( $W[ 28 ] && 
	defined( @RV_above_max ) ) ;
$dataref[ $W[ 29 ] ] = \@RV_below_min if( $W[ 29 ] && 
	defined( @RV_below_min ) ) ;
$dataref[ $W[ 30 ] ] = \@RV_vrbl if( $W[ 30 ] && defined( @RV_vrbl ) ) ;
$dataref[ $W[ 31 ] ] = \@RV_min if( $W[ 31 ] && defined( @RV_min ) ) ;
$dataref[ $W[ 32 ] ] = \@RV_max if( $W[ 32 ] && defined( @RV_max ) ) ;
$dataref[ $W[ 33 ] ] = \@RV_visRange if( $W[ 33 ] && defined( @RV_visRange ) ) ;
$dataref[ $W[ 34 ] ] = \$WX if( $W[ 34 ] && defined( $WX ) ) ;
$dataref[ $W[ 35 ] ] = \$vert_VIS if( $W[ 35 ] && defined( $vert_VIS ) ) ;
$dataref[ $W[ 36 ] ] = \@cloud_type if( $W[ 36 ] && defined( @cloud_type ) ) ;
$dataref[ $W[ 37 ] ] = \@cloud_hgt if( $W[ 37 ] && defined( @cloud_hgt ) ) ;
$dataref[ $W[ 38 ] ] = \@cloud_meters if( $W[ 38 ] && defined( @cloud_meters ));
$dataref[ $W[ 39 ] ] = \@cloud_phenom if( $W[ 39 ] && defined( @cloud_phenom ));
$dataref[ $W[ 40 ] ] = \$T if( $W[ 40 ] && defined( $T ) ) ;
$dataref[ $W[ 41 ] ] = \$TD if( $W[ 41 ] && defined( $TD ) ) ;
$dataref[ $W[ 42 ] ] = \$hectoPasc_ALTIM if( $W[ 42 ] && 
	defined( $hectoPasc_ALTIM ) ) ;
$dataref[ $W[ 43 ] ] = \$inches_ALTIM if( $W[ 43 ] && defined( $inches_ALTIM ));
$dataref[ $W[ 44 ] ] = \$NOSIG if( $W[ 44 ] && defined( $NOSIG ) ) ;

# remarks vars

$dataref[ $W[ 45 ] ] = \$TornadicType if( $W[ 45 ] && defined( $TornadicType ));
$dataref[ $W[ 46 ] ] = \$TornadicLOC if( $W[ 46 ] && defined( $TornadicLOC ) ) ;
$dataref[ $W[ 47 ] ] = \$TornadicDIR if( $W[ 47 ] && defined( $TornadicDIR ) ) ;
$dataref[ $W[ 48 ] ] = \$BTornadic_hh if( $W[ 48 ] && defined( $BTornadic_hh ));
$dataref[ $W[ 49 ] ] = \$BTornadic_mm if( $W[ 49 ] && defined( $BTornadic_mm ));
$dataref[ $W[ 50 ] ] = \$ETornadic_hh if( $W[ 50 ] && defined( $ETornadic_hh ));
$dataref[ $W[ 51 ] ] = \$ETornadic_mm if( $W[ 51 ] && defined( $ETornadic_mm ));
$dataref[ $W[ 52 ] ] = \$AUTOindicator if( $W[ 52 ] && defined($AUTOindicator));
$dataref[ $W[ 53 ] ] = \$PKWND_dir if( $W[ 53 ] && defined( $PKWND_dir ) ) ;
$dataref[ $W[ 54 ] ] = \$PKWND_spd if( $W[ 54 ] && defined( $PKWND_spd ) ) ;
$dataref[ $W[ 55 ] ] = \$PKWND_hh if( $W[ 55 ] && defined( $PKWND_hh ) ) ;
$dataref[ $W[ 56 ] ] = \$PKWND_mm if( $W[ 56 ] && defined( $PKWND_mm ) ) ;
$dataref[ $W[ 57 ] ] = \$WshfTime_hh if( $W[ 57 ] && defined( $WshfTime_hh ) ) ;
$dataref[ $W[ 58 ] ] = \$WshfTime_mm if( $W[ 58 ] && defined( $WshfTime_mm ) ) ;
$dataref[ $W[ 59 ] ] = \$Wshft_FROPA if( $W[ 59 ] && defined( $Wshft_FROPA ) ) ;
$dataref[ $W[ 60 ] ] = \$VIS_TWR if( $W[ 60 ] && defined( $VIS_TWR ) ) ;
$dataref[ $W[ 61 ] ] = \$VIS_SFC if( $W[ 61 ] && defined( $VIS_SFC ) ) ;
$dataref[ $W[ 62 ] ] = \$VISmin if( $W[ 62 ] && defined( $VISmin ) ) ;
$dataref[ $W[ 63 ] ] = \$VISmax if( $W[ 63 ] && defined( $VISmax ) ) ;
$dataref[ $W[ 64 ] ] = \$VIS_2ndSite if( $W[ 64 ] && defined( $VIS_2ndSite ) ) ;
$dataref[ $W[ 65 ] ] = \$VIS_2ndSite_LOC if( $W[ 65 ] && 
	defined( $VIS_2ndSite_LOC ) ) ;
$dataref[ $W[ 66 ] ] = \$LTG_OCNL if( $W[ 66 ] && defined( $LTG_OCNL ) ) ;
$dataref[ $W[ 67 ] ] = \$LTG_FRQ if( $W[ 67 ] && defined( $LTG_FRQ ) ) ;
$dataref[ $W[ 68 ] ] = \$LTG_CNS if( $W[ 68 ] && defined( $LTG_CNS ) ) ;
$dataref[ $W[ 69 ] ] = \$LTG_CG if( $W[ 69 ] && defined( $LTG_CG ) ) ;
$dataref[ $W[ 70 ] ] = \$LTG_IC if( $W[ 70 ] && defined( $LTG_IC ) ) ;
$dataref[ $W[ 71 ] ] = \$LTG_CC if( $W[ 71 ] && defined( $LTG_CC ) ) ;
$dataref[ $W[ 72 ] ] = \$LTG_CA if( $W[ 72 ] && defined( $LTG_CA ) ) ;
$dataref[ $W[ 73 ] ] = \$LTG_DSNT if( $W[ 73 ] && defined( $LTG_DSNT ) ) ;
$dataref[ $W[ 74 ] ] = \$LTG_AP if( $W[ 74 ] && defined( $LTG_AP ) ) ;
$dataref[ $W[ 75 ] ] = \$LTG_VcyStn if( $W[ 75 ] && defined( $LTG_VcyStn ) ) ;
$dataref[ $W[ 76 ] ] = \$LTG_DIR if( $W[ 76 ] && defined( $LTG_DIR ) ) ;
 
# Recent_WX ReWX[3];
$dataref[ $W[ 77 ] ] = \@Recent_WX if( $W[ 77 ] && defined( @Recent_WX ) ) ;
$dataref[ $W[ 78 ] ] = \@Recent_WX_Bhh if( $W[ 78 ] && 
	defined( @Recent_WX_Bhh ) ) ;
$dataref[ $W[ 79 ] ] = \@Recent_WX_Bmm if( $W[ 79 ] && 
	defined( @Recent_WX_Bmm ) ) ;
$dataref[ $W[ 80 ] ] = \@Recent_WX_Ehh if( $W[ 80 ] && 
	defined( @Recent_WX_Ehh ) ) ;
$dataref[ $W[ 81 ] ] = \@Recent_WX_Emm if( $W[ 81 ] && 
	defined( @Recent_WX_Emm ) ) ;

$dataref[ $W[ 82 ] ] = \$Ceiling_min if( $W[ 82 ] && defined( $Ceiling_min ) ) ;
$dataref[ $W[ 83 ] ] = \$Ceiling_max if( $W[ 83 ] && defined( $Ceiling_max ) ) ;

$dataref[ $W[ 84 ] ] = \$CIG_2ndSite_meters if( $W[ 83 ] && 
	defined( $CIG_2ndSite_meters ) ) ;
$dataref[ $W[ 85 ] ] = \$CIG_2ndSite_LOC if( $W[ 85 ] && 
	defined( $CIG_2ndSite_LOC ) ) ;
$dataref[ $W[ 86 ] ] = \$PRESFR if( $W[ 86 ] && defined( $PRESFR ) ) ;
$dataref[ $W[ 87 ] ] = \$PRESRR if( $W[ 87 ] && defined( $PRESRR ) ) ;
$dataref[ $W[ 88 ] ] = \$SLPNO if( $W[ 88 ] && defined( $SLPNO ) ) ;
$dataref[ $W[ 89 ] ] = \$SLP if( $W[ 89 ] && defined( $SLP ) ) ;
$dataref[ $W[ 90 ] ] = \$SectorVIS_DIR if( $W[ 90 ] && 
	defined( $SectorVIS_DIR ) ) ;
$dataref[ $W[ 91 ] ] = \$SectorVIS if( $W[ 91 ] && defined( $SectorVIS ) ) ;
$dataref[ $W[ 92 ] ] = \$GR if( $W[ 92 ] && defined( $GR ) ) ;
$dataref[ $W[ 93 ] ] = \$GRsize if( $W[ 93 ] && defined( $GRsize ) ) ;
$dataref[ $W[ 94 ] ] = \$VIRGA if( $W[ 94 ] && defined( $VIRGA ) ) ;
$dataref[ $W[ 95 ] ] = \$VIRGAdir if( $W[ 95 ] && defined( $VIRGAdir ) ) ;
$dataref[ $W[ 96 ] ] = \$SfcObscuration if( $W[ 96 ] && 
	defined( $SfcObscuration ) ) ;
$dataref[ $W[ 97 ] ] = \$OctsSkyObscured if( $W[ 97 ] && 
	defined( $OctsSkyObscured ) ) ;
$dataref[ $W[ 98 ] ] = \$CIGNO if( $W[ 98 ] && defined( $CIGNO ) ) ;
$dataref[ $W[ 99 ] ] = \$Ceiling_est if( $W[ 99 ] && defined( $Ceiling_est ) ) ;
$dataref[ $W[ 100 ] ] = \$Ceiling if( $W[ 100 ] && defined( $Ceiling ) ) ;
$dataref[ $W[ 101 ] ] = \$VrbSkyBelow if( $W[ 101 ] && defined( $VrbSkyBelow ));
$dataref[ $W[ 102 ] ] = \$VrbSkyLayerHgt if( $W[ 102 ] && 
	defined( $VrbSkyLayerHgt ) ) ;
$dataref[ $W[ 103 ] ] = \$VrbSkyAbove if( $W[ 103 ] && 
	defined( $VrbSkyAbove ) ) ;
$dataref[ $W[ 104 ] ] = \$Sign_cloud if( $W[ 104 ] && defined( $Sign_cloud ) ) ;
$dataref[ $W[ 105 ] ] = \$Sign_dist if( $W[ 105 ] && defined( $Sign_dist ) ) ;
$dataref[ $W[ 106 ] ] = \$Sign_dir if( $W[ 106 ] && defined( $Sign_dir ) ) ;
$dataref[ $W[ 107 ] ] = \$ObscurAloft if( $W[ 107 ] && defined( $ObscurAloft ));
$dataref[ $W[ 108 ] ] = \$ObscurAloftSkyCond if( $W[ 108 ] && 
	defined( $ObscurAloftSkyCond ) ) ;
$dataref[ $W[ 109 ] ] = \$ObscurAloftHgt if( $W[ 109 ] && 
	defined( $ObscurAloftHgt ) ) ;
$dataref[ $W[ 110 ] ] = \$ACFTMSHP if( $W[ 110 ] && defined( $ACFTMSHP ) ) ;
$dataref[ $W[ 111 ] ] = \$NOSPECI if( $W[ 111 ] && defined( $NOSPECI ) ) ;
$dataref[ $W[ 112 ] ] = \$FIRST if( $W[ 112 ] && defined( $FIRST ) ) ;
$dataref[ $W[ 113 ] ] = \$LAST if( $W[ 113 ] && defined( $LAST ) ) ;
$dataref[ $W[ 114 ] ] = \$Cloud_low if( $W[ 114 ] && defined( $Cloud_low ) ) ;
$dataref[ $W[ 115 ] ] = \$Cloud_medium if( $W[ 115 ] && 
	defined( $Cloud_medium ) ) ;
$dataref[ $W[ 116 ] ] = \$Cloud_high if( $W[ 116 ] && defined( $Cloud_high ) ) ;
$dataref[ $W[ 117 ] ] = \$SNINCR if( $W[ 117 ] && defined( $SNINCR ) ) ;
$dataref[ $W[ 118 ] ] = \$SNINCR_TotalDepth if( $W[ 118 ] && 
	defined( $SNINCR_TotalDepth ) ) ;
$dataref[ $W[ 119 ] ] = \$SN_depth if( $W[ 119 ] && defined( $SN_depth ) ) ;
$dataref[ $W[ 120 ] ] = \$SN_waterequiv if( $W[ 120 ] && 
	defined( $SN_waterequiv ) ) ;
$dataref[ $W[ 121 ] ] = \$SunSensorOut if( $W[ 121 ] && 
	defined( $SunSensorOut ) ) ;
$dataref[ $W[ 122 ] ] = \$SunShineDur if( $W[ 122 ] && defined( $SunShineDur ));
$dataref[ $W[ 123 ] ] = \$PRECIP_hourly if( $W[ 123 ] && 
	defined( $PRECIP_hourly ) ) ;
$dataref[ $W[ 124 ] ] = \$PRECIP_amt if( $W[ 124 ] && defined( $PRECIP_amt ) ) ;
$dataref[ $W[ 125 ] ] = \$PRECIP_24_amt if( $W[ 125 ] && 
	defined( $PRECIP_24_amt ) ) ;
$dataref[ $W[ 126 ] ] = \$T_tenths if( $W[ 126 ] && defined( $T_tenths ) ) ;
$dataref[ $W[ 127 ] ] = \$TD_tenths if( $W[ 127 ] && defined( $TD_tenths ) ) ;
$dataref[ $W[ 128 ] ] = \$Tmax if( $W[ 128 ] && defined( $Tmax ) ) ;
$dataref[ $W[ 129 ] ] = \$Tmin if( $W[ 129 ] && defined( $Tmin ) ) ;
$dataref[ $W[ 130 ] ] = \$Tmax24 if( $W[ 130 ] && defined( $Tmax24 ) ) ;
$dataref[ $W[ 131 ] ] = \$Tmin24 if( $W[ 131 ] && defined( $Tmin24 ) ) ;
$dataref[ $W[ 132 ] ] = \$char_Ptend if( $W[ 132 ] && defined( $char_Ptend ) ) ;
$dataref[ $W[ 133 ] ] = \$Ptend if( $W[ 133 ] && defined( $Ptend ) ) ;
$dataref[ $W[ 134 ] ] = \$PNO if( $W[ 134 ] && defined( $PNO ) ) ;
$dataref[ $W[ 135 ] ] = \$PWINO if( $W[ 135 ] && defined( $PWINO ) ) ;
$dataref[ $W[ 136 ] ] = \$FZRANO if( $W[ 136 ] && defined( $FZRANO ) ) ;
$dataref[ $W[ 137 ] ] = \$TSNO if( $W[ 137 ] && defined( $TSNO ) ) ;
$dataref[ $W[ 138 ] ] = \$maintIndicator if( $W[ 138 ] && 
	defined( $maintIndicator ) ) ;
$dataref[ $W[ 139 ] ] = \$PlainText if( $W[ 139 ] && defined( $PlainText ) ) ;

# original report and remarks

$dataref[ $W[ 140 ] ] = \$report if( $W[ 140 ] && defined( $report ) ) ;
$dataref[ $W[ 141 ] ] = \$remarks if( $W[ 141 ] && defined( $remarks ) ) ;

} # end setvars


# create a netcdf file or reopen a existing one
sub doNet {

    my( $hour, $file, $Id, $Num, $Time, $stn, $hr, $i ) ;
    ( $hour ) = @_ ;

    # Select output file
    if (defined($datadir)) {
	$ncdir = $datadir . "/" . $yyyymmdd;
	$ncfile = $ncdir . "/metar." . $yyyymmdd . "." . $hour . "00.nc" ;
    } else {
	$ncdir = dirname($outfile);
	$ncfile = $outfile;
    }

    # writing to same file
    return 1 if( $ncfile eq $lastNc ) ;

    # current time
    $thetime = time() ;

    # save current file info
    $Nets{ $lastNc } = "$ncid $recnum $thetime" if( $lastNc ) ;

    # File is open, get ncfile id and recnum and reset the time 
    if( defined( $Nets{ $ncfile } ) ) { # already open for writes
	( $ncid, $recnum, $ncTime ) = split( " ", $Nets{ $ncfile } ) ;
	$ncTime = $thetime ;
	$lastNc =  $ncfile ;
	return 1 ;
    }

    # close files with no activity for the file open timeout period, or
    # sync currently open files
    foreach $file ( keys %Nets ) {
	( $Id, $Num, $Time ) = split( " ", $Nets{ $file } ) ;
	if( $thetime - $Time > $fileOpenTimeOut ) {
		logmsg("Closing $file with ncid $Id\n") ;
		$result = NetCDF::close( $Id ) ;
		delete( $Nets{ $file } ) ;
	} else {
		#logmsg("Syncing $file with ncid $Id\n") ;
		$result = NetCDF::sync( $Id ) ;
	}
    }

    # remove stn entries older than 4 hours
    if ( ! $yymm ) {
	foreach $stn ( keys %reportslist ) {
	    ( $rec, $Time ) = split( " ", $reportslist{ $stn } ) ;
	    delete( $reportslist{ $stn } ) if( $thetime - $Time > 14400 ) ;
	}
    }
    # open or create ncfiles
    if( -e $ncfile ) {
	$ncid = NetCDF::open( "$ncfile", WRITE ) ;
	$recNum_id = NetCDF::dimid( $ncid, "recNum" ) ;
	$name_id =  "xxxxxxxx"  ;
	$recnum =  -1  ;
	# get current value of recnum
	NetCDF::diminq( $ncid, $recNum_id, $name_id, $recnum ) ;
    } else {
	die "Wrong or missing cdlfile parameter: $!\n" unless  -e $cdlfile ;
   	system( "mkdir -p $ncdir" ) if( ! -e $ncdir ) ;
	system( "$ncgen -o $ncfile $cdlfile" ) ;
	$ncid = NetCDF::open( "$ncfile", WRITE ) ;
	# NetCDF record counter
	$recnum = 0 ;
    }
    $Nets{ $ncfile } = "$ncid $recnum $thetime" ;
    $lastNc = $ncfile ;
    logmsg("Opening $ncfile with ncid $ncid\n") ;

    # check for to many files open
    if( $ncid > 25 ) {
	foreach $file ( keys %Nets ) {
		( $Id, $Num, $Time ) = split( " ", $Nets{ $file } ) ;
		if( $ncid != $Id ) {
			logmsg("Closing $file, too many open files\n") ;
			$result = NetCDF::close( $Id ) ;
			delete( $Nets{ $file } ) ;
		}
	}
    }
    return 1 ;

} # end doNet


sub syncFiles {

    my( $file, $Id, $Num, $Time ) ;

    foreach $file ( keys %Nets ) {
	( $Id, $Num, $Time ) = split( " ", $Nets{ $file } ) ;
	#logmsg("Syncing $file with ncid $Id\n") ;
	$result = NetCDF::sync( $Id ) ;
    }
}

# printclean - removes characters from string before printing to stdout,
# for example, "'", \0's and -99999 values ($F).

sub printclean
{
  ($str) = @_;
  $str =~ s/\'|\0|$F//g;
  print $str;
}


# print vars
sub printvars
{

#
#  if the station name and observation time were not read, then don't print anything
#
return if (!defined($stn_name) || !defined($time_obs));

# report vars
print "rep_type=$rep_type\n" if( defined( $rep_type ) ) ;
print "stn_name=$stn_name\n" if( defined( $stn_name ) ) ;
print "time_obs=$time_obs\n" if( defined( $time_obs ) ) ;
print "time_nominal=$time_nominal\n" if( defined( $time_nominal ) ) ;
print "wmo_id=$wmo_id\n" if( defined( $wmo_id ) ) ;
print "lat=$lat\n" if( defined( $lat ) ) ;
print "lon=$lon\n" if( defined( $lon ) ) ;
print "elev=$elev\n" if( defined( $elev ) ) ;
print "ob_hour=$ob_hour\n" if( defined( $ob_hour ) ) ;
print "ob_min=$ob_min\n" if( defined( $ob_min ) ) ;
print "ob_day=$ob_day\n" if( defined( $ob_day ) ) ;
print "AUTO=$AUTO\n" if( defined( $AUTO ) ) ;
print "gust=$GUST\n" if( defined( $GUST ) ) ;
#print "VRB=$VRB\n" if( defined( $VRB ) ) ;
print "dir=$DIR\n" if( defined( $DIR ) ) ;
print "spd=$SPD\n" if( defined( $SPD ) ) ;
print "units=$UNITS\n" if( defined( $UNITS ) ) ;
#print "DIRmin=$DIRmin\n" if( defined( $DIRmin ) ) ;
#print "DIRmax=$DIRmax\n" if( defined( $DIRmax ) ) ;
print "prevail_VIS_SM=$prevail_VIS_SM\n" if( defined( $prevail_VIS_SM ) ) ;
print "prevail_VIS_KM=$prevail_VIS_KM\n" if( defined( $prevail_VIS_KM ) ) ;
print "plus_VIS_SM=$plus_VIS_SM\n" if( defined( $plus_VIS_SM ) ) ;
print "plus_VIS_KM=$plus_VIS_KM\n" if( defined( $plus_VIS_KM ) ) ;
print "prevail_VIS_M=$prevail_VIS_M\n" if( defined( $prevail_VIS_M ) ) ;
#print "VIS_dir=$VIS_dir\n" if( defined( $VIS_dir ) ) ;
print "CAVOK=$CAVOK\n" if( defined( $CAVOK ) ) ;
#print "RVRNO=$RVRNO\n" if( defined( $RVRNO ) ) ;
#print "RV_designator=@RV_designator\n" if( defined( @RV_designator ) ) ;
#print "RV_above_max=@RV_above_max\n" if( defined( @RV_above_max ) ) ;
#print "RV_below_min=@RV_below_min\n" if( defined( @RV_below_min ) ) ;
#print "RV_vrbl=@RV_vrbl\n" if( defined( @RV_vrbl ) ) ;
#print "RV_min=@RV_min\n" if( defined( @RV_min ) ) ;
#print "RV_max=@RV_max\n" if( defined( @RV_max ) ) ;
#print "RV_visRange=@RV_visRange\n" if( defined( @RV_visRange ) ) ;
print "WX=$WX\n" if( defined( $WX ) ) ;
print "vert_VIS=$vert_VIS\n" if( defined( $vert_VIS ) ) ;
printclean "cloud_type=@cloud_type\n" if( defined( @cloud_type ) ) ;
printclean "cloud_hgt=@cloud_hgt\n" if( defined( @cloud_hgt ) ) ;
printclean "cloud_meters=@cloud_meters\n" if( defined( @cloud_meters ) ) ;
#print "cloud_phenom=@cloud_phenom\n" if( defined( @cloud_phenom ) ) ;
print "T=$T\n" if( defined( $T ) ) ;
print "TD=$TD\n" if( defined( $TD ) ) ;
print "inches_ALTIM=$inches_ALTIM\n" if( defined( $inches_ALTIM ) ) ;
print "hectoPasc_ALTIM=$hectoPasc_ALTIM\n" if( defined( $hectoPasc_ALTIM )) ;
#print "NOSIG=$NOSIG\n" if( defined( $NOSIG ) ) ;

# remarks vars
#print "TornadicType=$TornadicType\n" if( defined( $TornadicType ) ) ;
#print "TornadicLOC=$TornadicLOC\n" if( defined( $TornadicLOC ) ) ;
#print "TornadicDIR=$TornadicDIR\n" if( defined( $TornadicDIR ) ) ;
#print "BTornadic_hh=$BTornadic_hh\n" if( defined( $BTornadic_hh ) ) ;
#print "BTornadic_mm=$BTornadic_mm\n" if( defined( $BTornadic_mm ) ) ;
#print "ETornadic_hh=$ETornadic_hh\n" if( defined( $ETornadic_hh ) ) ;
#print "ETornadic_mm=$ETornadic_mm\n" if( defined( $ETornadic_mm ) ) ;
print "AUTOindicator=$AUTOindicator\n" if( defined( $AUTOindicator ) ) ;
#print "PKWND_dir=$PKWND_dir\n" if( defined( $PKWND_dir ) ) ;
#print "PKWND_spd=$PKWND_spd\n" if( defined( $PKWND_spd ) ) ;
#print "PKWND_hh=$PKWND_hh\n" if( defined( $PKWND_hh ) ) ;
#print "PKWND_mm=$PKWND_mm\n" if( defined( $PKWND_mm ) ) ;
#print "WshfTime_hh=$WshfTime_hh\n" if( defined( $WshfTime_hh ) ) ;
#print "WshfTime_mm=$WshfTime_mm\n" if( defined( $WshfTime_mm ) ) ;
#print "Wshft_FROPA=$Wshft_FROPA\n" if( defined( $Wshft_FROPA ) ) ;
#print "VIS_TWR=$VIS_TWR\n" if( defined( $VIS_TWR ) ) ;
#print "VIS_SFC=$VIS_SFC\n" if( defined( $VIS_SFC ) ) ;
#print "VISmin=$VISmin\n" if( defined( $VISmin ) ) ;
#print "VISmax=$VISmax\n" if( defined( $VISmax ) ) ;
#print "VIS_2ndSite=$VIS_2ndSite\n" if( defined( $VIS_2ndSite ) ) ;
#print "VIS_2ndSite_LOC=$VIS_2ndSite_LOC\n" if( defined( $VIS_2ndSite_LOC )) ;
#print "LTG_OCNL=$LTG_OCNL\n" if( defined( $LTG_OCNL ) ) ;
#print "LTG_FRQ=$LTG_FRQ\n" if( defined( $LTG_FRQ ) ) ;
#print "LTG_CNS=$LTG_CNS\n" if( defined( $LTG_CNS ) ) ;
#print "LTG_CG=$LTG_CG\n" if( defined( $LTG_CG ) ) ;
#print "LTG_IC=$LTG_IC\n" if( defined( $LTG_IC ) ) ;
#print "LTG_CC=$LTG_CC\n" if( defined( $LTG_CC ) ) ;
#print "LTG_CA=$LTG_CA\n" if( defined( $LTG_CA ) ) ;
#print "LTG_DSNT=$LTG_DSNT\n" if( defined( $LTG_DSNT ) ) ;
#print "LTG_AP=$LTG_AP\n" if( defined( $LTG_AP ) ) ;
#print "LTG_VcyStn=$LTG_VcyStn\n" if( defined( $LTG_VcyStn ) ) ;
#print "LTG_DIR=$LTG_DIR\n" if( defined( $LTG_DIR ) ) ;
 
# Recent_WX ReWX[3];
print "Recent_WX=@Recent_WX\n" if( defined( @Recent_WX ) ) ;
print "Recent_WX_Bhh=@Recent_WX_Bhh\n" if( defined( @Recent_WX_Bhh ) ) ;
print "Recent_WX_Bmm=@Recent_WX_Bmm\n" if( defined( @Recent_WX_Bmm ) ) ;
print "Recent_WX_Ehh=@Recent_WX_Ehh\n" if( defined( @Recent_WX_Ehh ) ) ;
print "Recent_WX_Emm=@Recent_WX_Emm\n" if( defined( @Recent_WX_Emm ) ) ;

#print "Ceiling_min=$Ceiling_min\n" if( defined( $Ceiling_min ) ) ;
#print "Ceiling_max=$Ceiling_max\n" if( defined( $Ceiling_max ) ) ;
#print "CIG_2ndSite_meters=$CIG_2ndSite_meters\n" if( defined( $CIG_2ndSite_meters ) ) ;
#print "CIG_2ndSite_LOC=$CIG_2ndSite_LOC\n" if( defined( $CIG_2ndSite_LOC )) ;
#print "PRESFR=$PRESFR\n" if( defined( $PRESFR ) ) ;
#print "PRESRR=$PRESRR\n" if( defined( $PRESRR ) ) ;
#print "SLPNO=$SLPNO\n" if( defined( $SLPNO ) ) ;
print "SLP=$SLP\n" if( defined( $SLP ) ) ;
#print "SectorVIS_DIR=$SectorVIS_DIR\n" if( defined( $SectorVIS_DIR ) ) ;
#print "SectorVIS=$SectorVIS\n" if( defined( $SectorVIS ) ) ;
#print "GR=$GR\n" if( defined( $GR ) ) ;
#print "GRsize=$GRsize\n" if( defined( $GRsize ) ) ;
#print "VIRGA=$VIRGA\n" if( defined( $VIRGA ) ) ;
#print "VIRGAdir=$VIRGAdir\n" if( defined( $VIRGAdir ) ) ;
print "SfcObscuration=$SfcObscuration\n" if( defined( $SfcObscuration ) ) ;
print "OctsSkyObscured=$OctsSkyObscured\n" if( defined( $OctsSkyObscured )) ;
#print "CIGNO=$CIGNO\n" if( defined( $CIGNO ) ) ;
print "Ceiling_est=$Ceiling_est\n" if( defined( $Ceiling_est ) ) ;
print "Ceiling=$Ceiling\n" if( defined( $Ceiling ) ) ;
#print "VrbSkyBelow=$VrbSkyBelow\n" if( defined( $VrbSkyBelow ) ) ;
#print "VrbSkyLayerHgt=$VrbSkyLayerHgt\n" if( defined( $VrbSkyLayerHgt ) ) ;
#print "VrbSkyAbove=$VrbSkyAbove\n" if( defined( $VrbSkyAbove ) ) ;
#print "Sign_cloud=$Sign_cloud\n" if( defined( $Sign_cloud ) ) ;
#print "Sign_dist=$Sign_dist\n" if( defined( $Sign_dist ) ) ;
#print "Sign_dir=$Sign_dir\n" if( defined( $Sign_dir ) ) ;
#print "ObscurAloft=$ObscurAloft\n" if( defined( $ObscurAloft ) ) ;
print "ObscurAloftSkyCond=$ObscurAloftSkyCond\n" if( defined( $ObscurAloftSkyCond ) ) ;
print "ObscurAloftHgt=$ObscurAloftHgt\n" if( defined( $ObscurAloftHgt ) ) ;
print "ACFTMSHP=$ACFTMSHP\n" if( defined( $ACFTMSHP ) ) ;
#print "NOSPECI=$NOSPECI\n" if( defined( $NOSPECI ) ) ;
#print "FIRST=$FIRST\n" if( defined( $FIRST ) ) ;
#print "LAST=$LAST\n" if( defined( $LAST ) ) ;
print "Cloud_low=$Cloud_low\n" if( defined( $Cloud_low ) ) ;
print "Cloud_medium=$Cloud_medium\n" if( defined( $Cloud_medium ) ) ;
print "Cloud_high=$Cloud_high\n" if( defined( $Cloud_high ) ) ;
print "SNINCR=$SNINCR\n" if( defined( $SNINCR ) ) ;
print "SNINCR_TotalDepth=$SNINCR_TotalDepth\n" if( defined( $SNINCR_TotalDepth ) ) ;
print "SN_depth=$SN_depth\n" if( defined( $SN_depth ) ) ;
print "SN_waterequiv=$SN_waterequiv\n" if( defined( $SN_waterequiv ) ) ;
#print "SunSensorOut=$SunSensorOut\n" if( defined( $SunSensorOut ) ) ;
print "SunShineDur=$SunShineDur\n" if( defined( $SunShineDur ) ) ;
print "PRECIP_hourly=$PRECIP_hourly\n" if( defined( $PRECIP_hourly ) ) ;
print "PRECIP_amt=$PRECIP_amt\n" if( defined( $PRECIP_amt ) ) ;
print "PRECIP_24_amt=$PRECIP_24_amt\n" if( defined( $PRECIP_24_amt ) ) ;
print "T_tenths=$T_tenths\n" if( defined( $T_tenths ) ) ;
print "TD_tenths=$TD_tenths\n" if( defined( $TD_tenths ) ) ;
print "Tmax=$Tmax\n" if( defined( $Tmax ) ) ;
print "Tmin=$Tmin\n" if( defined( $Tmin ) ) ;
print "Tmax24=$Tmax24\n" if( defined( $Tmax24 ) ) ;
print "Tmin24=$Tmin24\n" if( defined( $Tmin24 ) ) ;
print "char_Ptend=$char_Ptend\n" if( defined( $char_Ptend ) ) ;
print "Ptend=$Ptend\n" if( defined( $Ptend ) ) ;
#print "PWINO=$PWINO\n" if( defined( $PWINO ) ) ;
#print "PNO=$PNO\n" if( defined( $PNO ) ) ;
#print "FZRANO=$FZRANO\n" if( defined( $FZRANO ) ) ;
#print "TSNO=$TSNO\n" if( defined( $TSNO ) ) ;
#print "maintIndicator=$maintIndicator\n" if( defined( $maintIndicator ) ) ;
#print "PlainText Language=$PlainText\n" if( defined( $PlainText ) ) ;
#  signal end of report
print "END METAR REPORT\n\n";

} # end printvars



# init Recent weather arrays
sub initReWX
{
# Recent_WX ReWX[3];
@Recent_WX = ( $S8, $S8, $S8 ) ;
@Recent_WX_Bhh = ( $F, $F, $F ) ;
@Recent_WX_Bmm = ( $F, $F, $F ) ;
@Recent_WX_Ehh = ( $F, $F, $F ) ;
@Recent_WX_Emm = ( $F, $F, $F ) ;

} # end initReWX

# pad str to correct length
sub padstr
{
( $str, $len ) = @_ ;

local( $size, $i ) ;

$size = length( $str ) ;

for( $i = $size; $i < $len; $i++ ) {
        $str .= "\0" ;
	#print "$str,\n" ;
}
if( $size > $len ) {
	logmsg("String length over $len chars:\n $str\n") ;
	$str = substr( $str, 0, $len ) ;
	#exit 0 ;
}
return $str ;
}

sub thetime
{

my( $when, $add, $ss, $mm, $hh, $mday, $mon, $year, $wday, $yday, $isdst ) ;

( $when ) = @_ ;

$add = 0 ;
$mm = 0 ;
$hh = 0 ;
$mday = $ob_day ;
if( $when eq "tomorrow" ) {
	$add = 86400 ;
} elsif( $when eq "yesterday" ) {
	$add = -86400 ;
	$mday = $tday ;
} elsif( $when eq "obs" ) {
	$mm = $ob_min ;
	$hh = $ob_hour ;
} elsif( $when eq "nominal" ) {
#	$mm = $ob_min if( $ob_min > 14 && $ob_min < 45 ) ;
	$year = substr ($yyyymmdd, 2, 2);
	$mon = substr ($yyyymmdd, 4, 2);
	$mday = substr ($yyyymmdd, 6, 2);
	$hh = $hour ;
} elsif( $when eq "trans" ) {
	$mm = $tmin ;
	$hh = $thour ;
	$mday = $tday ;
}

$year += 100 if ($year < 70);

return timegm_nocheck(0, $mm, $hh, $mday, $mon-1, $year+1900, 0, 0, 0) if ( $when eq "nominal" ) ;
 
$time = timegm_nocheck(0, $mm, $hh, $mday, $themonth -1, $theyear, 0, 0, 0) ;
return $time if( $when eq "obs" || $when eq "trans" ) ;

( $ss, $mm, $hh, $mday, $mon, $year, $wday, $yday, $isdst ) =
	gmtime( $time + $add ) ;
$mon++ ;
$mon = sprintf( "%02d", $mon ) ;
$mday = sprintf( "%02d", $mday ) ;
return sprintf( "%04d%02d%02d", $year+1900, $mon, $mday ) ;

}


# Subroutine to print a message to a log file or stdout/stderr with
# a time string in front. 

sub logmsg {

    my ($sec, $min, $hr, $day, $mon, $yr, $wd, $jd, $dst, $time, $date);

    ($sec, $min, $hr, $day, $mon, $yr, $wd, $jd, $dst) = gmtime() ;
    $time = sprintf("%02d:%02d:%02d",$hr, $min, $sec);
    $date = sprintf("%04d%02d%02d", $yr+1900, $mon+1, $day) ;

    if ($logdir)
    {
	#$logfile = ">>$logdir/$logname.$date.log";
	$logfile = ">>$logdir/$logname.$date.asc";

	if ($logfile ne $prevlog)
	{
	    close LOG;
	    open LOG, $logfile;
	    select (LOG) ; $| = 1 ; 
	}
	$prevlog = $logfile;
    }
    else
    {
	select (STDOUT) ; $| = 1;
    }

    print "$time @_";
}


# execute at exit
sub atexit
{
local( $sig ) = @_ ;

if( $sig eq "timeout" ) {
        logmsg("Timed-out after $timeout seconds\n") ;
} elsif( $sig eq "eof" ) {
	logmsg("eof on STDIN --shutting down\n") ;
} elsif( defined( $sig )) {
	logmsg("Caught SIG$sig --shutting down\n") ;
}
# open $metar_list, and save reports processed in the last 4 hours.
# NOTE: real-time mode and netCDF writing only! NOTE2: don't do at all!
#if ( ! $yymm && defined($cdlfile) ) {
#    open( LST, ">$metar_list" );
#    if (defined(LST)) {
#      foreach $stn ( keys %reportslist ) {
#	( $rec, $Time ) = split( " ", $reportslist{ $stn } ) ;
#	print LST "$stn $reportslist{ $stn }\n" if( $thetime - $Time < 14400 ) ;
#      }
#      close LST ;
#    }
#}
if (defined($cdlfile)) {
  foreach $file ( keys %Nets ) {
	( $ncid, $recnum, $nctime ) = split( " ", $Nets{ $file } ) ;
	logmsg("Closing $file with ncid $ncid\n") ;
	$result = NetCDF::close( $ncid ) ;
  }
}
logmsg("Ending\n") ;

close( STDOUT ) ;
close( STDERR ) ;
exit( 0 ) ;

}
