#!/usr/bin/env python

"""
This function contains classes to handle filename io.  
"""
# Author Beth Weekley @ UCAR/RAL  5/06

import os
import sys
import compare_sets
import index_file
#from sets import Set
import name_schema
import ncvdefs

# -------------------------
class MapFileName:
    """
    This is a class which remaps the name
    of an input file to an output file.

    Inputs:
      input_name_schema
      output_name_schema
            
    """
    
    def __init__(self, input_name_schema, output_name_schema):
        self.input_name_schema = input_name_schema
        self.output_name_schema = output_name_schema
        
        # initialize variables
        self.input_name_schema = input_name_schema
        self.output_name_schema = output_name_schema
        
    def map_name(self, output_name):
        """
        This module maps the output_name to the input name
        """

        out_date = self.output_name_schema.get_date(output_name)
        out_it = self.output_name_schema.get_it(output_name)
        
        #print "out_date = "
        #print out_date
        #print "out_it = "
        #print out_it

        in_name = self.input_name_schema.make_name(out_date, out_it)
       
        # the name has an extra '.' at the beginning of the name
        if in_name[0] == "." :
            in_name = in_name[1:]
        
        
        return in_name

# -------------------------
class UnprocessedFiles:
    """
    This is a class which gets all the unprocessed files.
    
    Inputs:
      map_file_name   - map_name method
      input_index     - input index filename
      output_index    - output index filename
    """
    
    def __init__(self, map_file_name, input_index, output_index):

        # initialize variables
        self.map_file_name = map_file_name
        self.input_index = input_index
        self.output_index = output_index
        
    def get_files(self, date):
        """
        This method returns a list of unprocessed
        files for the given date.
        """
        
        # ---------
        # get the input and output index file paths
        # ---------
        curr_input_file = index_file.index_file_path(self.input_index.base_dir,self.input_index.index_name, date)
        curr_output_file = index_file.index_file_path(self.output_index.base_dir,self.output_index.index_name, date)

        # ---------
        # open the input and output index files and compare them
        # ---------
        input_set = compare_sets.open_read_file(curr_input_file)
        output_set = compare_sets.open_read_file(curr_output_file)

        if (len(output_set)>0):
            out_new_filenames = set([])
            for name in output_set:
                new_output_name = self.map_file_name.map_name(name) 
                out_new_filenames.add(new_output_name)

            # get the set difference between the converted input set and the output set
            unproc_filenames = compare_sets.set_difference(out_new_filenames, input_set)

        else:
            unproc_filenames = input_set
        
        if (len(unproc_filenames) == 0):
            return(unproc_filenames)

        # ---------
        # sort the set of unprocessed files
        # ---------
        else:
            unproc_files = compare_sets.sort_set(unproc_filenames)
            return(unproc_files)

        
# -------------------------
class ProcessedFiles:
    """
    This is a class which gets all the Processed files.
    
    Inputs:
      map_file_name             - map_name method
      input_index_file_list     - list of input index filename
      output_index              - output index filename
    """
    
    def __init__(self, map_file_name, input_index_list, output_index):

        # initialize variables
        self.map_file_name = map_file_name
        self.input_index_list = input_index_list
        self.output_index = output_index
        
    def get_files(self, date):
        """
        This method returns a list of processed
        files for the given date.
        """
        
        # ---------
        # get the input list of files and output index file paths
        # ---------
        # loop over the list of input_index files
        input_files = []
        for ind in xrange(len(self.input_index_list)):
            input_index = self.input_index_list[ind]
            input_files.append(index_file.index_file_path(input_index.base_dir,input_index.index_name, date) )
            
            
        curr_output_file = index_file.index_file_path(self.output_index.base_dir,self.output_index.index_name, date)

        # ---------
        # open the input list of index files
        # and get the intersection of all of the input sets
        # ---------
        for ind in xrange(len(input_files)):
            curr_input_file = input_files[ind]
            curr_set = compare_sets.open_read_file(curr_input_file)
            if ind == 0:
                intersection = curr_set
            else:
                intersection = compare_sets.set_intersection(intersection, curr_set)
                
        input_set = intersection

        # ---------
        # open the input and output index files and compare them
        # ---------
        output_set = compare_sets.open_read_file(curr_output_file)

        if (len(output_set)>0):
            out_new_filenames = set([])
            for name in output_set:
                new_output_name = self.map_file_name.map_name(name) 
                out_new_filenames.add(new_output_name)

            # get the set difference between the converted input set and the output set
            unproc_filenames = compare_sets.set_difference(out_new_filenames, input_set)

        else:
            unproc_filenames = input_set
        
        if (len(unproc_filenames) == 0):
            return(unproc_filenames)

        # ---------
        # sort the set of unprocessed files
        # ---------
        else:
            unproc_files = compare_sets.sort_set(unproc_filenames)
            return(unproc_files)

        
# -------------------------
class MapFileNameGeneral:
    """
    This is a class which remaps the name
    of an input file to an output file.

    Inputs:
      input_name_schema
      output_name_schema
            
    """
    
    def __init__(self, input_name_schema, output_name_schema):
        self.input_name_schema = input_name_schema
        self.output_name_schema = output_name_schema
        
        # initialize variables
        self.input_name_schema = input_name_schema
        self.output_name_schema = output_name_schema
        
    def map_name(self, output_name):
        """
        This module maps the output_name to the input name
        """

        out_date = self.output_name_schema.get_date(output_name)
        out_it = self.output_name_schema.get_it(output_name)
        out_ft = self.output_name_schema.get_ft(output_name) 
        
        #print "out_date = "
        #print out_date
        #print "out_it = "
        #print out_it

        in_name = self.input_name_schema.make_name(out_date, out_it, out_ft)
       
        # the name has an extra '.' at the beginning of the name
        if in_name[0] == "." :
            in_name = in_name[1:]
        
        
        return in_name

    def map_name_out(self, input_name):
        """
        This module maps the input_name to the output name
        """
        in_date = self.input_name_schema.get_date(input_name)
        in_it = self.input_name_schema.get_it(input_name)
        in_ft = self.input_name_schema.get_ft(input_name) 
        
        out_name = self.output_name_schema.make_name(in_date, in_it, in_ft)
       
        # the name has an extra '.' at the beginning of the name
        if out_name[0] == "." :
            out_name = out_name[1:]
        
        return out_name
# -------------------------
class UnlinkedFiles:
    """
    This is a class which is used to get all the unlinked files.
    This takes in a map_name method and an input_set of files
    and and output_index method.
    
    Inputs:
      map_file_name             - map_name method
      input_set                 - input set of files
      output_index              - output index method
    """
    
    def __init__(self, map_file_name, input_set, output_index):

        # initialize variables
        self.map_file_name = map_file_name
        self.input_set = input_set
        self.output_index = output_index
        
    def get_files(self, date):
        """
        This method returns a list of processed
        files for the given date.
        """
        
        # ---------
        # get the input list of files and output index file paths
        # ---------
        output_file = index_file.index_file_path(self.output_index.base_dir,self.output_index.index_name, date)

        # ---------
        # open the output index file and that set to the input set
        # ---------
        output_set = compare_sets.open_read_file(output_file)
        if (len(output_set)>0):
            out_new_filenames = set([])
            for name in output_set:
                new_output_name = self.map_file_name.map_name(name)
                out_new_filenames.add(new_output_name)

            # get the set difference between the converted input set and the output set
            unproc_filenames = compare_sets.set_difference(out_new_filenames, self.input_set)

        else:
            unproc_filenames = self.input_set
        
        if (len(unproc_filenames) == 0):
            return(unproc_filenames)

        # ---------
        # sort the set of unprocessed files
        # ---------
        else:
            unproc_files = compare_sets.sort_set(unproc_filenames)
            return(unproc_files)



    def get_files_poes(self, date):
        """
        This method returns a list of processed
        files for the given date.
        """
        
        # ---------
        # get the input list of files and output index file paths
        # ---------
        output_file = index_file.index_file_path(self.output_index.base_dir,self.output_index.index_name, date)

        # ---------
        # open the output index file and that set to the input set
        # ---------
        unproc_filenames = set([])
        output_set = compare_sets.open_read_file(output_file)
        if (len(output_set)>0):
            for name in self.input_set:
                new_input_filenames = set([])
                new_input_name = self.map_file_name.map_name_out(name)
                new_input_filenames.add(new_input_name)
                
                # check to see if this file name is in the output set 
                intersect_file = compare_sets.set_intersection(new_input_filenames,output_set)
                if len(intersect_file) == 0:                 
                    unproc_filenames.add(name)
                    
        else:
            unproc_filenames = self.input_set
        
        if (len(unproc_filenames) == 0):
            return(unproc_filenames)

        # ---------
        # sort the set of unprocessed files
        # ---------
        else:
            unproc_files = compare_sets.sort_set(unproc_filenames)
            return(unproc_files)

        
# -------------------------

def run_filename_io_test():
    
    AWC_in_ns_pattern = ["YYYYMMDD","HHMM","","gE_ir4km_ch2",""]
    AWC_in_ns_format = "%D_%I.%F%B%S"
    AWC_ns = name_schema.Name_schema(AWC_in_ns_format,AWC_in_ns_pattern)
    
    out_ns_pattern = ["east_3_9" ,"YYYYMMDD","HHMM","","gini"]
    out_ns_format = "%B.%D.%I.%F%S"
    out_ns = name_schema.Name_schema(out_ns_format,out_ns_pattern)
    
    # -------------
    # create an instance of the MapFileNameGeneral class
    # -------------
    map_file_name = MapFileNameGeneral(AWC_ns,out_ns)

    output_dir = os.getcwd()
    # the output_dir contains an index file, index.20060704 with contains one line,
    # east_3_9.20060704.0015.gini 1152118740
    file_str = "east_3_9.20060704.0015.gini 1152118740"
    out_ind = open("index.20060704",'w')
    out_ind.write(file_str)
    out_ind.write("\n")
    out_ind.close()
    
    input_files = ['20060704_0015.gE_ir4km_ch2', '20060704_0045.gE_ir4km_ch2', '20060704_0101.gE_ir4km_ch2', '20060704_0115.gE_ir4km_ch2']
    input_set = set([])
    for i in xrange(len(input_files)):
        input_set.add(input_files[i])
        
    output_index = index_file.Index_file(output_dir, ncvdefs.Index_base, ncvdefs.Index_period, "w", ncvdefs.Index_wait)

    # -------------
    # create an instance of the UnlinkedFiles
    # -------------
    get_unlinked_files = UnlinkedFiles(map_file_name,input_set,output_index)
    yyyymmdd = "20060704"
    unlinked_files = get_unlinked_files.get_files(yyyymmdd)

    if (unlinked_files == ['20060704_0045.gE_ir4km_ch2', '20060704_0101.gE_ir4km_ch2', '20060704_0115.gE_ir4km_ch2']):
        print "successful test!"
    else:
        print "test failed"
        
# -------------------------
if __name__ =='__main__':
    run_filename_io_test()
