#!/usr/bin/env python

import os,sys
import urllib
import HTMLParser
import zipfile

class FileDownloader(object):

    """ initialized by a url to retrieve from, and a file path and a file name"""

    def __init__(self, url, directory, name, extract):
        
        self.file_path = "%s/%s" % (directory, name)
          
        if not os.path.lexists(directory):
            os.mkdir(directory)
            
        urllib.urlretrieve(url, self.file_path)
        
        if extract and zipfile.is_zipfile(self.file_path):

            zip_file = zipfile.ZipFile(self.file_path,"r")
            
            zip_file.extractall(directory)
            zip_file.close()

        self.path_exists = os.path.exists(self.file_path)

    def file_downloaded(self):
        return self.path_exists
            
         
            




            
        
        
        
        
        
