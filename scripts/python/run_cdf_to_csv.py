#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_cdf_to_csv.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/08/20 16:39:43 $           #
#                                                                                #
# ============================================================================== #

# cdf_to_csv /d2/vii/data/static/config/vdt_output.cfg /d2/vii/data/processed/mn_vdt_output/20130731/vdt.20130731.1050.nc /var/autofs/mnt/ftp/restricted/ftp_vdt/root/download/minnesota/csv_output/vdt.20130731.1050.csv

#run_cdf_to_csv.py -l /d2/vii/data/log/run_cdf_to_csv /d2/vii/data/static/config/ 20130730 /d2/vii/data/processed/mn_vdt_output/ /var/autofs/mnt/ftp/restricted/ftp_vdt/root/download/minnesota/csv_output/

import log_msg
import os
import sys
from optparse import OptionParser

def run_cdf_to_csv(config_dir, date, input_dir, output_dir, state_abbrev, logg):

    dated_input_dir = input_dir + date

    if os.path.exists(dated_input_dir):
    
        file_list = os.listdir(dated_input_dir)
        
        vdt_list = []
        statistics_list = []
        assessment_list = []

        vdt_config = ""
        statistics_config = ""
        assessment_config = ""
        
        for myfile in file_list:
            if "vdt" in myfile:
                vdt_list.append(myfile)
                vdt_config = config_dir + "vdt_output.cfg"
            elif "statistics" in myfile:
                statistics_list.append(myfile)
                statistics_config = config_dir + "statistics_output.cfg"
            elif "assessment" in myfile:
                assessment_list.append(myfile)
                assessment_config = config_dir + "assessment_output.cfg"
                
            else:
                logg.write_error("Filename schema for %s not recognized. Expecting to see vdt, statistics, or assessment in the filename." % (myfile))

        if vdt_list != []:
            for vdt_file in vdt_list:
                
                input_path = dated_input_dir + "/" + vdt_file
                csv_file_name =  state_abbrev + "_" + vdt_file[:-2] + "csv"
                dated_output_dir = output_dir + "/" + date
                output_path = dated_output_dir + "/" + csv_file_name
                logfile_base = "cdf_to_csv_" + state_abbrev

                if not os.path.exists(dated_output_dir):
                    os.mkdir(dated_output_dir)
                
                cmd = "cdf_to_csv -l /d2/vii/data/log/%s %s %s %s" % (logfile_base, vdt_config, input_path, output_path)
                logg.write_info(cmd)
                ret = os.system(cmd)
                if ret != 0:
                    logg.write_error("run_cdf_to_csv.py returned %d when trying to convert a vdt file: %s" % (ret, input_path))
                    logg.write_ending(1, "run_cdf_to_csv.py")
                
        if statistics_list != []:
            for statistics_file in statistics_list:
                
                input_path = dated_input_dir + "/" + statistics_file
                csv_file_name =  state_abbrev + "_" + statistics_file[:-2] + "csv"
                dated_output_dir = output_dir + "/" + date
                output_path = dated_output_dir + "/" + csv_file_name
                logfile_base = "cdf_to_csv_" + state_abbrev

                if not os.path.exists(dated_output_dir):
                    os.mkdir(dated_output_dir)
                
                cmd = "cdf_to_csv -l /d2/vii/data/log/%s %s %s %s" % (logfile_base, statistics_config, input_path, output_path)
                logg.write_info(cmd)
                ret = os.system(cmd)
                if ret != 0:
                    logg.write_error("run_cdf_to_csv.py returned %d when trying to convert a statistics file: %s" % (ret, input_path))
                    logg.write_ending(1, "run_cdf_to_csv.py")

        if assessment_list != []:
            for assessment_file in assessment_list:
                
                input_path = dated_input_dir + "/" + assessment_file
                csv_file_name =  state_abbrev + "_" + assessment_file[:-2] + "csv"
                dated_output_dir = output_dir + "/" + date
                output_path = dated_output_dir + "/" + csv_file_name
                logfile_base = "cdf_to_csv_" + state_abbrev

                if not os.path.exists(dated_output_dir):
                    os.mkdir(dated_output_dir)
                
                cmd = "cdf_to_csv -l /d2/vii/data/log/%s %s %s %s" % (logfile_base, assessment_config, input_path, output_path)
                logg.write_info(cmd)
                ret = os.system(cmd)
                if ret != 0:
                    logg.write_error("run_cdf_to_csv.py returned %d when trying to convert a assessment file: %s" % (ret, input_path))
                    logg.write_ending(1, "run_cdf_to_csv.py")

    else:
        logg.write_info("The dated input directory %s does not exist." % (dated_input_dir))


    return 0       

def main():

    usage_str = "%prog config_dir date input_dir output_dir state_abbrev \n\tNOTE: date format should be YYYYMMDD"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 4:
        parser.print_help()
        sys.exit(2)

    config_dir = args[0]
    date = args[1]
    input_dir = args[2]
    output_dir = args[3]
    state_abbrev = args[4]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.set_suffix("pyl")
    logg.write_starting("run_cdf_to_csv.py")

    run_cdf_to_csv(config_dir, date, input_dir, output_dir, state_abbrev, logg)

    logg.write_ending(0, "run_cdf_to_csv.py")

if __name__ == "__main__":

   main()
