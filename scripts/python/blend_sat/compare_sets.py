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

import os, sys
#from sets import Set
import string
import sys_functions

#------------------------------------------------------------------
def set_difference(set_a, set_b):
    """
    compare two sets and return the items
    which are in set_b but not in set_a
    """
    
    diff = set_b - set_a
 
    return diff
#------------------------------------------------------------------
def set_intersection(set_a, set_b):
    """
    compare two sets and return the items
    which are in set_b and also in set_a
    """
    
    intersection = set_b & set_a
 
    return intersection

#------------------------------------------------------------------
def sort_set(set_a):
    """
    Sorts set_a
    """

    # convert to a list and sort the list.
    sort_a = [b for b in set_a]
    sort_a.sort()

    # Reverse the sort so that most recent (time) is first
    sort_a.reverse()

    return sort_a

#------------------------------------------------------------------
def open_read_file(file):
    """
    This function opens the files and puts
    the output into a set
    """

    file_name = set([])
    if (os.path.exists(file)):
        data = open(file, 'r')
    
        lines = data.readlines()
        # create an instance of IndexFileIO
        index_file_io = sys_functions.IndexFileIo()
        # print 'lines: '
        # print lines
        for line in lines:
            (name, date) = index_file_io.parse_index_line(line)
            file_name.add(name)
            
            #sp_line = string.split(line)
            #if (sp_line != []):
            #    name = sp_line[0]
            #    file_name.add(name)

    # print file_name
    return file_name
        
#------------------------------------------------------------------
def compare_files(input_index_file, output_index_file ):
    """
    This function compares two sets and returns the items which
    are in the input_index_file but not in the output_index_file
    """
    
    # -------------
    # open the input index file for reading
    # -------------
    input_set = open_read_file(input_index_file)

    # -------------
    # open the output index file for reading
    # -------------
    output_set = open_read_file(output_index_file)

    # -------------
    # get the difference in the files where
    # the input_set is the larger set
    # -------------
    unproc_files = set_difference(output_set, input_set)
    #print unproc_files

    return unproc_files

#------------------------------------------------------------------
def add_suffix(filenames, suffix):
    """This function adds the suffix to every name
    in the set filenames and returns a set with the
    new file names

    inputs:
       filenames -      a set with all the filenames
       suffix    -      the suffix to be removed from the filenames

    output
       new_filenames -  a set with the filenames without the suffix

    """

    new_filenames = set([])
    # loop over the list of files and add the suffix
    for name in filenames:
        new_filenames.add(name + "." + suffix)
        #print "filenames = "
        #print name + "." + suffix
        
    return new_filenames

#------------------------------------------------------------------
def remove_suffix(filenames, suffix):
    """This function removes the suffix from every name
    in the set filenames and returns a set with the
    new file names

    inputs:
       filenames -      a set with all the filenames
       suffix    -      the suffix to be added to the filenames

    output
       new_filenames -  a set with the filenames and the added suffix
    """

    new_filenames = set([])
    len_suffix = len(suffix) + 1  # add one for the "."
    # loop over the list of files and remove the suffix
    for name in filenames:
        name = name[:-len_suffix]
        new_filenames.add(name)
        
    return new_filenames
    
#------------------------------------------------------------------
def transform_suffix(filenames, suffix_old, suffix_new):
    """This function removes the suffix from every name
    in the set filenames and returns a set with the
    new file names

    inputs:
       filenames -      a set with all the filenames
       suffix_old -     the suffix to be removed from the filenames
       suffix_new -     the suffix to be added to the filenames

    output
       new_filenames -  a set with the filenames and the added suffix
    """

    new_filenames = set([])
    len_suffix_old = len(suffix_old) + 1  # add one for the "."
    # loop over the list of files and remove the suffix
    for name in filenames:
        name = name[:-len_suffix_old]
        new_filenames.add(name + "." + suffix_new)
        
    return new_filenames
    
#------------------------------------------------------------------
def transform_prefix(filenames, prefix_old, prefix_new):
    """This function removes the prefix from every name
    in the set filenames and returns a set with the
    new file names

    inputs:
       filenames -      a set with all the filenames
       prefix_old -     the prefix to be removed from the filenames
       prefix_new -     the prefix to be added to the filenames

    output
       new_filenames -  a set with the filenames and the added prefix
    """

    new_filenames = set([])
    len_prefix_old = len(prefix_old)
    # loop over the list of files and remove the prefix
    for name in filenames:
        name = name[len_prefix_old:]
        new_filenames.add(prefix_new + name)


    return new_filenames
    
#------------------------------------------------------------------
