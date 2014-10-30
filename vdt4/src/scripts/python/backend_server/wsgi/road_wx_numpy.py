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
import math
from netCDF4 import Dataset

class RoadWeatherLocator(object):

    def __init__(self, flann_file, numpy_seg_pts_file, numpy_seg_id_file):

        #print "initializing road weather locator: loading %s %s %s" % (flann_file, numpy_seg_pts_file, numpy_seg_id_file)
        self.seg_pts = numpy.load(numpy_seg_pts_file)
        self.seg_id = numpy.load(numpy_seg_id_file)
	#print "seg_pts: ", self.seg_pts
	#print "seg_id: ", self.seg_id

    def locate(self, lat, lon, num_neighbor):
        q = numpy.array([lat, lon])
	index = numpy.argmin([numpy.inner(q-x, q-x) for x in self.seg_pts])
	dist = math.hypot(q[0]-self.seg_pts[index][0], q[1]-self.seg_pts[index][1])
        return numpy.array([index]), numpy.array([dist])
            
    def radius(self, lat, lon, radius):
        q = numpy.array([lat, lon])
        sq_dist = numpy.array([numpy.inner(q-x, q-x) for x in self.seg_pts])
        sorted_indices = numpy.argsort(sq_dist)
        sorted_dist = sq_dist[sorted_indices]
        index = numpy.searchsorted(sorted_dist, radius, side='right')
        #sorted_sq_dist = sq_dist(sorted_indices)
        #print "dist1: ", sq_dist[sorted_indices[0]]
        #print "dist2: ", sq_dist[sorted_indices[1]]
        #print "dist3: ", sq_dist[sorted_indices[2]]
        #print "dist4: ", sq_dist[sorted_indices[3]]

	#index = numpy.argmin([numpy.inner(q-x, q-x) for x in self.seg_pts])
	#print self.seg_pts[index]
        #print "deltas: ", q[0]-self.seg_pts[index][0], q[1]-self.seg_pts[index][1]
	#dist = math.hypot(q[0]-self.seg_pts[index][0], q[1]-self.seg_pts[index][1])
        print "given pt: ", lat, lon
        print "sorted indices: ", sorted_indices[0:index]
        print "sorted distances: ", sorted_dist[0:index]
        if index > 0:
            return numpy.array(sorted_indices[0:index]), numpy.array(sorted_dist[0:index])
        else:
            #print "dist, radius: ", dist, radius
            return numpy.array([]), numpy.array([])
            

        

def main():

    print "establishing locator"
    michigan_wx_locator = RoadWeatherLocator("/d1/vii/data/static/config/mi_flann_index.dat", "/d1/vii/data/static/config/mi_numpy_seg_pts.npy", "/d1/vii/data/static/config/mi_numpy_seg_id.npy")

    print "getting radius"
    
    (index, dist) = michigan_wx_locator.radius(42.3366087, -82.99912, 0.0002)
    #michigan_wx_locator.radius(44.2451, -84.2273, 0.0005)
    print "index, dist: ", index, dist
    print michigan_wx_locator.seg_pts[index]

if __name__ == "__main__":

   main()

