#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2014 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2014/10/20 13:00:00 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import datetime
import fcntl
import os

class PhoneLog(object):

    def __init__(self, phone_dir):

        self.phone_dir = phone_dir
        return
    
    def generate_file_name(self, phone_number, yyyymmdd):

        file_name = os.path.join(self.phone_dir, "phone_%s.%s.csv" % (phone_number, yyyymmdd))
        return file_name
    
    def write(self, file_name, log_string):

        # open maw global file
        out_file = os.open(file_name, os.O_RDWR|os.O_CREAT)

        # lock file
        fcntl.lockf(out_file, fcntl.LOCK_EX)
        
        os.lseek(out_file, 0, os.SEEK_END)

        ret = os.write(out_file, log_string)

        fcntl.lockf(out_file, fcntl.LOCK_UN)
        os.close(out_file)

def main():
    
    phone_log = PhoneLog("/tmp")
    phone_log.write("3034423676", "20140207", "my_first_log\n")
    
if __name__ == "__main__":

    main()
