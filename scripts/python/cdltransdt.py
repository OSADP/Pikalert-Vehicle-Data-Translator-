#!/usr/bin/env python

"""
Script to convert the dates in the cdl output of a ncdump -fc file.nc command from unix to a different format
Usage: ./cdltransdt.py timefield1 [timefield2 ...]
"""

import sys
import re
import time


def match_pats(pats, s):
    """
    Takes a list of patterns and a string.  Returns the first successful match or None.
    """
    for pat in pats:
        match = pat.match(s)
        if match:
            return match

    return None


def process_stdin(fieldnames, datefmt = "%Y%m%d%H%M%S"):
    begfieldpats = [re.compile("(^\s+%s = )(\d+)(,|;)(.*)" % field) for field in fieldnames]

    begandates = False
    datepat = re.compile("(^\s+)(\d+)(,|;)(.*)")

    for line in sys.stdin:
        if begandates:
            match = datepat.match(line)
        else:
            match = match_pats(begfieldpats, line)

        if match:
            prefix = match.group(1)
            date = match.group(2)
            term = match.group(3)
            suffix = match.group(4)

            tm = time.strftime(datefmt, time.gmtime(int(date)))        
            sys.stdout.write("%s%s%s%s\n" % (prefix, tm, term, suffix))

            if term == ";":
                #End dates
                begandates = False
            else:
                begandates = True
        else:
            sys.stdout.write(line)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "Usage: %s timefield1 [timefield2 ...]" % sys.argv[0]
        exit(1)

    fieldnames = sys.argv[1:]
    process_stdin(fieldnames)
    
