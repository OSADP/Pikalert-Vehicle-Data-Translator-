#!/usr/bin/env python

import sys
import optparse
import log_msg
import glob
import hashlib
import os


"""
A simple script to remove duplicate files from a globbed file listing.
"""

def hash_file(path):
    """
    Returns the hashed contents of a file.
    """
    fhash = None
    with open(path) as f:
        data = f.read()
        fhash = hashlib.md5(data).hexdigest()

    return fhash


def process_dups(paths, pfun):
    """
    Processes all duplicate files in paths executing pfun on each duplicate file.
    Returns the number of files processed.
    """
    proc_count = 0
    if len(paths) < 2:
        return proc_count

    hashset = {hash_file(paths[0])}
    for path in paths[1:]:
        fhash = hash_file(path)
        if fhash in hashset:
            pfun(path)
            proc_count += 1
        else:
            hashset.add(fhash)
        
    return proc_count
    

if __name__ == '__main__':
    usage_str = "%prog 'pathglob'"
    parser = optparse.OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-t", "--test", action="store_true", dest="test", help="Prints out a list of all duplicates found but does not actually remove them.")

    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.print_usage()
        sys.exit(1)

    pathglob = args[0]
    
    if options.log:
        logg = log_msg.LogMessage(options.log,"pyl")
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting()

    fs = sorted(glob.glob(pathglob))
    #print fs
    
    if options.test:
        rm_count = process_dups(fs, lambda path: logg.write_info("%s would be removed" % path) or True)
    else:
        rm_count = process_dups(fs, lambda path: logg.write_info("%s was removed" % path) or os.remove(path))
    
    logg.write_info("Removed %d files" % rm_count)
    
    logg.write_ending()
    
