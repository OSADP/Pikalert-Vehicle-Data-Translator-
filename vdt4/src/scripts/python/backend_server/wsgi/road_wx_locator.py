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

import numpy
from netCDF4 import Dataset
import pyflann

def build_save_flann_index(road_segment_file, latitude_field_name, longitude_field_name, seg_id_field_name, precision, output_flann_file, output_numpy_seg_pts_file, output_numpy_seg_id_file):

        cdf = Dataset(road_segment_file, "r")
        latitude = cdf.variables[latitude_field_name]
        longitude = cdf.variables[longitude_field_name]
        seg_id = cdf.variables[seg_id_field_name]

        seg_pts = numpy.vstack((latitude, longitude)).T
        nn = pyflann.FLANN(target_precision=precision)
        nn.build_index(seg_pts)
        nn.save_index(output_flann_file)
        numpy.save(output_numpy_seg_pts_file, seg_pts)
        numpy.save(output_numpy_seg_id_file, seg_id)

class RoadWeatherLocator(object):

    def __init__(self, flann_file, numpy_seg_pts_file, numpy_seg_id_file):
	print "trying to open flann_file for test purpose"
	fp = open(flann_file, "rb")
	if fp == None:
	    print "fp is None"	
        print "initializing road weather locator: loading %s %s %s" % (flann_file, numpy_seg_pts_file, numpy_seg_id_file)
        self.seg_pts = numpy.load(numpy_seg_pts_file)
        self.seg_id = numpy.load(numpy_seg_id_file)
	print "seg_pts: ", self.seg_pts
	print "seg_id: ", self.seg_id
        self.nn = pyflann.FLANN()
	print "nn: ", self.nn
	print "loading: file: ", flann_file
        self.nn.load_index(flann_file, self.seg_pts)
        print "finished loading"

    def locate(self, lat, lon, num_neighbor):
        (nnidx, nndist) = self.nn.nn_index(numpy.array([lat, lon]), num_neighbors=num_neighbor)
        if len(nnidx) > 0:
	    print "length of index: ", len(nnidx[0]), type(nnidx[0])
	    print "length of dist: ", len(nndist[0]), type(nndist[0])
            return nnidx[0], nndist[0]
        else:
            return numpy.array([]), numpy.array([])
            
    def radius(self, lat, lon, radius):
	print "calling nn_radius for lat, lon, radius: ", lat, lon, radius
        (nnidx, nndist) = self.nn.nn_radius(numpy.array([lat, lon]), radius)
        if len(nnidx) > 0:
	    print "radius: length of index: ", len(nnidx), type(nnidx)
	    print "radius: length of dist: ", len(nndist), type(nndist)
	    print "radius: nnidx, nndist: ", nnidx, nndist
            return nnidx, nndist
        else:
            print "radius: empty"
            return numpy.array([]), numpy.array([])
            

        

def main():

    print "building flann index from mn_roads.20131111.nc"
    build_save_flann_index("/d1/vii/data/static/cdl/mn_roads.20131111.nc", "latitude", "longitude", "seg_id", 1.0, "mn_flann_index.dat", "mn_numpy_seg_pts", "mn_numpy_seg_id")

    print "building flann index from mi_roads.20140613.nc"
    build_save_flann_index("/d1/vii/data/static/cdl/mi_roads.20140613.nc", "latitude", "longitude", "seg_id", 1.0, "mi_flann_index.dat", "mi_numpy_seg_pts", "mi_numpy_seg_id")

    print "building flann index from nv_roads.20131111.nc"
    build_save_flann_index("/d1/vii/data/static/cdl/nv_roads.20131111.nc", "latitude", "longitude", "seg_id", 1.0, "nv_flann_index.dat", "nv_numpy_seg_pts", "nv_numpy_seg_id")

    print "establishing locator"
    michigan_wx_locator = RoadWeatherLocator("/d1/vii/data/static/config/mi_flann_index.dat", "/d1/vii/data/static/config/mi_numpy_seg_pts.npy", "/d1/vii/data/static/config/mi_numpy_seg_id.npy")

    print "getting radius"
    michigan_wx_locator.radius(42.3367096515, -82.9987478256, 5e-07)


    print "locating points from 0 to 30"
    num_neighbors = 10
    for i in range(0, 30):
	    print "point index: ", i
	    print "point lat, long: ", michigan_wx_locator.seg_pts[i][0], michigan_wx_locator.seg_pts[i][1]

	    print "calling locate:"
	    print michigan_wx_locator.locate(michigan_wx_locator.seg_pts[i][0], michigan_wx_locator.seg_pts[i][1], num_neighbors)

	    print "calling radius:"
	    print michigan_wx_locator.radius(michigan_wx_locator.seg_pts[i][0], michigan_wx_locator.seg_pts[i][1], 0.5)

if __name__ == "__main__":

   main()

