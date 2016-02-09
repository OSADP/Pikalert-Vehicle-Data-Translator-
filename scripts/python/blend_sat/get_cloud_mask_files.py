#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2004
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
# Author Beth Weekley  3/05  @ UCAR

import os,sys
import ncvdefs
import log
import string
import time
import getopt
import datetime
import operator
from optparse import OptionParser


#----------------------------------------------------------
def usage(prog):
    print "usage: %s  [-L logfile_base] input_dir input_base date_str" % os.path.basename(prog)
    print "input_dir:\t input directory where the dated subdirectories are located"
    print "input_base:\t base to the file names ie g12 or g10 for a file name of g12.20050316.0015.nc"
    print "date_str:\t date string in the form YYYYMMDDHHMM"
    print "\n"
    sys.exit(2)
 
#----------------------------------------------------------
def get_config_data(config_file):
    """
    This function takes in a config_file and loads the data
    and returns the data
    """

    # -----------
    # get the config files without the path or the .py
    # -----------
    config_basename = os.path.basename(config_file)    
    if config_basename[-3:] == ".py":
        config_file = config_basename[:-3]
    else:
        config_file = config_basename
        
    # -----------
    # import the HHMM times from the config file
    # -----------
    config_data = __import__(config_file, globals(), locals(), [])

    return config_data
 
#----------------------------------------------------------
def get_current_time():
    """
    This function gets the current time in the form of a string
    YYYYMMDDHH
    """
    
    dt = datetime.datetime.utcnow()
    dtstr = dt.strftime("%Y%m%d%H%M")

    return (dtstr)

#----------------------------------------------------------

def date2sec(date):

    """
    This function converts the input date to seconds

    Inputs:
        date:     date in either format YYYYMMDDHH or YYYYMMDDHHMM to be converted to seconds
        
    Returns:
        date_sec: the date converted to seconds

    """
    if len(date) == 10:
        date_str = date + '0000'
    elif len(date) == 12:
        date_str = date + '00'

    date_tup = time.strptime("%s" % (date_str),"%Y%m%d%H%M%S")
    date_sec = int(time.mktime(date_tup) )

    return(date_sec)

#----------------------------------------------------------

def sec2date(date):

    """
    This function converts the input date in seconds to YYYYMMDDHH

    Inputs:
        date:   date in sec
        
    Returns:
        date_str:       date  YYYYMMDDHH 
        hr_str:         hour  HH 
    """
    
    date_str_long = time.gmtime(date)
    date_str = "%s%s%s" %(date_str_long[0],string.zfill(date_str_long[1],2),string.zfill(date_str_long[2],2)) 
    hr_str = "%s" %(string.zfill(date_str_long[3],2))
    
    return(date_str, hr_str)

#----------------------------------------------------------
def get_sitefilenames(dir):
    """
    This function gets the names of all the files in the
    input directory.

    Inputs:
      dir     - list of directories in which to get the names of all the files
    
    Returns:
      files   - a list with the names of all the files
    
    """
    files = []
    for dir_ind in xrange(len(dir)):
        files = operator.concat(files,
                                filter(os.path.isfile,[os.path.join(dir[dir_ind], f)
                                                       for f in os.listdir(dir[dir_ind])] )
    )
        
    return files
    
#----------------------------------------------------------
def get_minute(config_data, run_str):

    """
    This function uses the config file data, HHMM strings
    and returns the closest one <=  the run_str.  This takes into
    account the full scans every quadrant hour at 00 for goes10
    and at 45 for goes12 in the config file.  If the user wants the
    code to run just on the hour then the config file can be changed.  

    Input
         config_data:  configuration data with a list of all the HHMM times in a day
         run_str:      HHMM string which corresponds to run time

    Output
         closest_str:  HHMM string from the config file closest to the run_str
         
    """

    # -----------
    # convert the run_str into a number
    # -----------
    run_time = string.atoi(run_str)
    
    # -----------
    # find which time the run_time is closest to
    # -----------
    time_ind = 0
    for i in xrange(1, len(config_data.times)):
        if run_time >= config_data.times[i]:
            time_ind = i
        elif abs(config_data.times[i]-run_time) < abs(run_time-config_data.times[i-1]):
            time_ind = i
    

    # -----------
    # make sure time_ind[0] has 4 digits if not prepend zeros
    # -----------
    closest_str = "%04d" % config_data.times[time_ind]

    return closest_str
    
#----------------------------------------------------------
def get_satellite_files(ndays,dtsec,delta_time,input_base,file_sufix,delta_file_match,over_days,min_str):
    """
    get the satellite file from that subdirectory
    & 19 days prior if they exist

    Input:
       ndays:              number of days to get files, 20 including today
       dtsec:              run date in seconds
       delta_time:         time between files in seconds, 1 day
       input_base:         g12 or g10
       file_sufix:         nc
       delta_file_match:   window to match the file
       over_days:          flag to check both today and yesterday
       min_str:            HHMM used to match the files to    
    """

    file_list = []           # initialize the file list
    file_list_dates = []     # initialize the file list
    dir_list = []

    for day_ind in xrange(ndays):
        if over_days:
            day_prior = [dtsec - day_ind*delta_time , dtsec - day_ind*delta_time - 1*3600]
        else:
            day_prior = [dtsec - day_ind*delta_time]
            
        day_prior = [dtsec - day_ind*delta_time]
            
        # -------------
        # loop over day_prior
        # -------------
        for i in xrange(len(day_prior)):
            (date_str, hr_str) = sec2date(day_prior[i])
        
            # -------------
            # append date to input_dir to get file names at the hour
            # -------------
            file_name = "%s.%s.%s%s.%s" %(input_base, date_str, hr_str, min_str, file_sufix )
    
            # -------------
            # get directory names
            # -------------
            dir_name = "%s/%s" %(input_dir, date_str)
    
            # -------------
            # check that the directory exists 
            # -------------
            if os.path.isdir(dir_name):
                dir_list.append(dir_name)
       
        # -------------
        # get the satellite files in the dir_list 
        # // with the time closest to the file_name above
        # -------------
        file_names = get_sitefilenames(dir_list)
    
        file_match_sec = delta_file_match   # init the file match window
        # -------------
        # loop over the files in the directory & find
        # the file closest to the hour of interest
        # -------------
        for file_ind in xrange(len(file_names)):
	    base_filename = os.path.basename(file_names[file_ind])

	    if (base_filename[0] == "." ):
		continue
            file_str = string.split(base_filename,".")

            file_date = "%s%s" %(file_str[1],file_str[2])
        
            # -------------
            # convert the file date to seconds
            # -------------
            file_sec = date2sec(file_date)
            if (abs(file_sec - day_prior[0]) < file_match_sec):
                file_match_sec = abs(file_sec - day_prior[0])
                file_match = file_names[file_ind]

        # -------------
        # if a file was found within the file_match_sec
        # append the file name to the list of satellite file names
        # also create a file date list to check to see if todays file
        # is in the list, if not exit
        # -------------
        if (file_match_sec != delta_file_match):
            file_list.append(file_match)
            # -------------
            # list of dates
            # -------------
            file_list_dates.append(file_date[0:8])
           

    todays_file = 0
    (dtdate,dthour)= sec2date(dtsec)

    # -------------
    # check and see if the run date is in the date list
    # -------------
    for i in xrange(len(file_list_dates)):
        if file_list_dates[i] == dtdate[0:8]:
            todays_file = 1
            
    if  todays_file == 0:
        file_list = 0

    return file_list

#----------------------------------------------------------

if __name__ == "__main__":
    """
    This function creates a file with a list of input files
    for the nasa_cloud_id code.  File names from dated 
    subdirectories of the input directory over the past ndays.
    For every day the file closest to the hour and within
    delta (delta_file_match) seconds of the hour is added to
    the output list. 
    """

    optlist = []
    args    = []
    logfile_path = ""
    input_dir    = ""
    output_file  = ""
    input_base   = ""    
    dtstr        = ""
    
    # -------------
    # Process command args
    # -------------
    try:
        optlist, args = getopt.getopt(sys.argv[1:], "L:")
    except:
        print "%s: Error in parsing options, %s" % (sys.argv[0], sys.exc_value)
        usage(sys.argv[0])
        sys.exit(2)

    for i in xrange(0,len(optlist)):
        if optlist[i][0] == "-L":    # logfile
            logfile_path = optlist[i][1]

    if (len(args) != 5):
        print "%s: Error incorrect number of command line arguments" %sys.argv[0]
        usage(sys.argv[0])

    script = os.path.basename(sys.argv[0])
    input_dir   = args[0]
    output_file = args[1]
    input_base  = args[2]
    dtstr       = args[3]
    tmp_dir     = args[4]

    # -------------
    # set up the log file
    # -------------
    logfile = log.Log(logfile_path)
    if (logfile_path == ""):
        logfile_arg = ""
    else:
	logfile_arg = "-l %s" % logfile_path
        logfile.set_suffix(ncvdefs.PYL_SUFFIX)

    logfile.write_time("Starting %s\n" % script)
    logfile.write_time("  Info: input_base =  %s\n" % input_base)

    file_sufix = "nc"
    ndays = 20                 # number of days to get data files from
    delta_time = 24*60*60      # 1 day 
    #delta_file_match = 60*20   # the interval of time in sec to see if there is a file match
    delta_file_match = 60*4   # the interval of time in sec to see if there is a file match
    over_days = 0
    
    # -------------
    # convert run date to seconds, used to increment by days
    # -------------
    dtsec = date2sec(dtstr)

    # -------------
    # check to see if run hour is 00 and if so check both today's
    # data file and the prior day for all 20 days do this.  
    # -------------
    if dtstr[8] == "0" and  dtstr[9] == "0" :
        over_days = 1
        
    # -------------
    # get the satellite file from that subdirectory
    # & 19 days prior if they exist
    # -------------
    #min_str = HHMM[2:5]
    min_str = dtstr[10:12]
    
    file_list = get_satellite_files(ndays,dtsec,delta_time,input_base, file_sufix, delta_file_match, over_days, min_str)
    if file_list == 0:
        logfile.write_time("  Info: todays file is not in the directory = %s \n" %(dtstr))
        logfile.write_time("Ending. Exit status = 0.\n")
        sys.exit(1)

    # -------------
    # reverse the list of files so the most current is
    # at the end.  Cloud mask code is expecting this
    # -------------
    file_list.reverse()

    # -------------
    # check to be sure the file_list is not empty
    # -------------
    if (file_list == []):
        logfile.write_time("  Error: No files from %s and the %d days prior \n" % (dtstr, ndays) )
        logfile.write_time("Ending. Exit status = 1.\n")
        sys.exit(1)

    logfile.write_time("  Info: number of files in the output file = %d \n" %(len(file_list)))

    # -------------
    # unzip the files and save the files into the output file make
    # a dated subdirectory of the tmp dir and unzip the files there
    # -------------
    #tmp_dated_dir =  "%s/%s" %(tmp_dir,dtstr)
    tmp_dated_dir =  tmp_dir
    if (not os.path.exists(tmp_dated_dir)):
        os.makedirs(tmp_dated_dir)
    
    file_name = []
    tmp_name  = []

    output = open(output_file, 'w')
    for i in xrange(len(file_list)):
        file = os.path.basename(file_list[i])
        if file[-3:] == ".gz":
            file_name.append(file[:-3])
        else:
            file_name.append(file)
            
        # -------------
        # unzip the file
        # -------------
        tmp_name.append("%s/%s" %(tmp_dated_dir, file_name[i]))
        command = "gunzip -c %s > %s" %(file_list[i],tmp_name[i])
        logfile.write_time("  Info: executing the command: %s\n" % command)
        ret = os.system(command)
        if (ret != 0):
            logfile.write_time("  Error executing the command: %s.\n" %command)            
            logfile.write_time("Ending. Exit status = 1.\n")          
            sys.exit(1)
            
        output.writelines(tmp_name[i])
        output.writelines("\n")

    output.close()
    # print "tmp_name = %s \n" %tmp_name

    logfile.write_time("Ending. Exit status = 0.\n")
