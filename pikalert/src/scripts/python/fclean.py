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

""" A file cleanup module which uses a config file to determine which files to cleanup and when. """

from stat import *
import os, sys
import getopt
import string
import time
import log

global log_base


def make_pair_dict(ls, dict):
    """ Make a dictionary from pairs in a list. Tacitly assumes list is even in size """
    ls_len = len(ls)
    if ls_len % 2 != 0:
        return -1
    
    i = 0
    while i < ls_len:
        dict[ls[i]] = ls[i+1]
        i = i + 2

    return 0

def reset_key(dict, key):
    """ Reset dict value to None if key is false """
    if dict[key] != None:
        kval = string.lower(dict[key])
        if kval == "false":
            dict[key] = None
            
fields = ["age:", "dir_name:", "recurse:", "tmp_dir:", "type:", "user:", "action:"]
(AGE, DIR, RECUR, TMP, TYPE, USER, ACTION) = range(len(fields))
binary_fields = ["recurse:", "user:"]

class Remove_script:
    def __init__(self, line, prefix, test, verbose):
        self.line = line
        self.dict = {}
        self.path_prefix = prefix
        self.test = test
        self.print_flag = verbose

        for f in fields:
            self.dict[f] = None
            
        # Parse line. Ignore comments and blank lines
        if line[0] != '#':
            ls = string.split(line)

            if len(ls) > 0:
                # Store in dictionary
                if make_pair_dict(ls, self.dict) < 0:
                    logf.write_time("Error: config entry does not have an even number of fields, %s" % self.line)
		    return

                for f in binary_fields:
                    reset_key(self.dict, f)
            
                try:
                    self.dict[fields[AGE]] = string.atoi(self.dict[fields[AGE]]) * 60.0
                except ValueError:
                    logf.write_time("Error: Improper value for %s in %s\n" % (fields[AGE], self.line))
                    return

                if self.dict[fields[USER]]:
                    self.dict[fields[USER]] = os.getuid()

                # Prefix relative pathnames
                if self.dict[fields[DIR]][0] != "/":
                    self.dict[fields[DIR]] = "%s%s" % (prefix, self.dict[fields[DIR]])


    def set_age(self, age):
        self.dict[fields[AGE]] = age
        return

    def set_dir(self, dir):
        self.dict[fields[DIR]] = dir
        return

    def get_dir(self):
        return self.dict[fields[DIR]]

    def set_recurse(self):
        self.dict[fields[RECUR]] = 'true'
        return

    def set_type(self, type):
        self.dict[fields[TYPE]] = type
        return
        
    def set_print(self):
        self.dict[fields[PR]] = 'true'
        return
    
    def set_tmp(self, tmp):
        self.dict[fields[TMP]] = tmp
        return

    def set_uid(self):
        self.dict[fields[USER]] = os.getuid()
        return

    def remove(self, dir_name):
        age = self.dict[fields[AGE]]
        recurse_flag = self.dict[fields[RECUR]]
        tmp = self.dict[fields[TMP]]
        type_flag = self.dict[fields[TYPE]]
        uid = self.dict[fields[USER]]
        user_flag = self.dict[fields[USER]]
        test_flag = self.test
        print_flag = self.print_flag
        action = self.dict[fields[ACTION]]

        if dir_name == None:
            return

	if action == None:
	    action = "rm"

        if not os.path.isdir(dir_name):
            logf.write_time("Warning: %s not a directory\n" % dir_name)
            return

	files = os.listdir(dir_name)
	for file in files:
	    try:

                path = os.path.join(dir_name, file)
		status = os.lstat(path)

		if recurse_flag and S_ISDIR(status[ST_MODE]):
		    self.remove(path)

                # Determine whether to continue processing
                go = 1
                if user_flag and uid != status[ST_UID]:
                    go = 0  
                elif type_flag == "files_only" and not S_ISREG(status[ST_MODE]):
                    go = 0  
                elif type_flag == "dirs_only" and not S_ISDIR(status[ST_MODE]):
                    go = 0  

                if go:
		    tim = time.time()

                    # Check for old files (regular or symlinks) or empty
		    # directories older than a day
		    
		    if (((S_ISREG(status[ST_MODE]) or S_ISLNK(status[ST_MODE])) and tim - status[ST_MTIME] > age) or (S_ISDIR(status[ST_MODE]) and len(os.listdir(path)) == 0 and tim - status[ST_MTIME] > 86400)):

			(root, ext) = os.path.splitext(file)

			# Handle a few specific actions. Adjust rm commands
			# for directories, and add -f to force removal of files
			if action == "rm":
                            if S_ISDIR(status[ST_MODE]):
                                cmd = "rmdir"
			    else:
			        cmd = "rm -f"

			# Not "rm", but is directory or link - skip
			elif S_ISDIR(status[ST_MODE]) or S_ISLNK(status[ST_MODE]):
			    continue

			# Compress actions - skip if already compressed
			elif action == "bzip2" and ext == ".bz2":
			    continue
			elif action == "gzip" and ext == ".gz":
			    continue
			elif action == "compress" and ext == ".Z":
			    continue

			# Otherwise, use defined action as-is
			else:
			    cmd = action


			if tmp:
			    pos = string.rfind(path, '/')
			    new_path = path[pos+1:]

                            if print_flag:
                                logf.write_time("Info: mv %s %s\n" % (path, tmp))
                                logf.write_time("Info: %s %s/%s\n" % (cmd, tmp, new_path))
			    if not test_flag:
				os.system("mv '%s' '%s'" % (path, tmp))
				os.system("%s '%s/%s'" % (cmd, tmp, new_path))
			else:
                            if print_flag:
                                logf.write_time("Info: %s %s\n" % (cmd, path))
			    if not test_flag:
				os.system("%s '%s'" % (cmd, path))
	    except:
		continue


def usage():
    print "Usage: %s [-h] [-l log_base] [-t] [-v] [-p path] config_file(s) " % sys.argv[0]
    print "  -h\t\tprint usage and exit"
    print "  -l log_base\tbase path for log file"
    print "  -p path\tPrefix any relative paths in config file with 'path'"
    print "  -t\t\ttest, does not actually remove anything"
    print "  -v\t\tverbose mode, log actions"
    print "  config_file\ta file containing file cleaning directives"
    print "\nEach line in config_file contains a set of fields as described below."
    print "\nRequired fields:"
    print "  dir_name: ~name # cleanup the directories specified"
    print "  age: ~minute # cleanup files older than the number of specified minutes"
    print "\nOptional fields:"
    print "  type: ~(files_only, dirs_only) # If files_only is specified, remove only files but not subdirectories. If dirs_only is specified remove only directories but not files."
    print "  recurse: ~(true, false) # recurse down through subdirectories"
    print "  tmp_dir: ~tmp_dir_path # move all files and subdirectories to a temporary directory tmp_dir_path before removing them"
    print "  user: ~(true, false) # only remove files or directories owned by current user"
    print "  action: ~(command) # perform 'command' on file (rm, gzip, bzip2,...) Default is to remove (rm)."
    print "\nExamples:"
    print "  dir_name: /tmp age: 21600"
    print "  dir_name: data/tmp age: 21600 recurse: true action: gzip"
    sys.exit(2)

    
if __name__ == '__main__':

    arg_len = len(sys.argv)

    optlist = []
    args = []
    prefix = ""
    testing = 0
    verbose = 0
    log_base = ""
    script = os.path.basename(sys.argv[0])

    try:
	optlist, args = getopt.getopt(sys.argv[1:], "hl:tvp:")
    except:
	print "%s: error in parsing options, %s" % (sys.argv[0], sys.exc_value)
	usage()
	sys.exit(1)
	
    for i in xrange(0,len(optlist)):
        if optlist[i][0] == "-h":
            usage()
            sys.exit(2)
        elif optlist[i][0] == "-t":
            testing = 1
        elif optlist[i][0] == "-v":
            verbose = 1
        elif optlist[i][0] == "-p":
            prefix = optlist[i][1]
            if prefix[-1] != "/":
                prefix = prefix + "/"
        elif optlist[i][0] == "-l":
            log_base = optlist[i][1]


    if (len(args) < 1):
        print "%s: error - incorrect number of arguments" % sys.argv[0]
        usage()


    # Set up log file
    logf = log.Log(log_base, "pyl")
    logf.write_time("Starting %s\n" % script)

    for conf in args:
        fp = open(conf, "r")
        lines = fp.readlines()
        for l in lines:
	    if l[0] != "#":
	        logf.write_time("Processing entry: '%s'\n" % l[0:-1])
                rs = Remove_script(l, prefix, testing, verbose)
                rs.remove(rs.get_dir())

    logf.write_time("Ending. Exit status = 0.\n")
