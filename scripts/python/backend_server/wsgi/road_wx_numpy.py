#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import numpy
import math
from netCDF4 import Dataset
import pyproj


def print_fun_io(func):
    """
    A decorator to print function input/output.
    To use add this before the function: @print_fun_io
    """
    def wrapper(*args, **kwargs):
        ret = func(*args, **kwargs)
        print "%s(%s) -> %r" % (func.__name__, ", ".join(map(str,args)), ret)
        return ret
    return wrapper


def build_save_numpy_index(road_segment_file, latitude_field_name, longitude_field_name, seg_id_field_name, output_numpy_seg_pts_file, output_numpy_seg_id_file):

        cdf = Dataset(road_segment_file, "r")
        latitude = cdf.variables[latitude_field_name]
        longitude = cdf.variables[longitude_field_name]
        seg_id = cdf.variables[seg_id_field_name]

        seg_pts = numpy.vstack((latitude, longitude)).T
        numpy.save(output_numpy_seg_pts_file, seg_pts)
        numpy.save(output_numpy_seg_id_file, seg_id)


class RoadWeatherLocator(object):
    #@print_fun_io
    def __init__(self, numpy_seg_pts_file, numpy_seg_id_file, degree_radius):

        self.seg_pts = numpy.load(numpy_seg_pts_file)
        self.seg_id = numpy.load(numpy_seg_id_file)
        self.lat, self.lon = zip(*list(self.seg_pts))
        self.lat = numpy.array(self.lat)
        self.lon = numpy.array(self.lon)
        self.degree_radius = degree_radius
        self.geod = pyproj.Geod('+ellps=clrk66')
        #Standard Positioning Service Performance Standard (meters)
        self.gps_accuracy = 3.5

        
    @staticmethod
    #@print_fun_io
    def bearing_sector(bear, half_theta):
        """
        Returns begin,end that is half_theta degrees less/greater than bearing.
        """
        assert 0.0 < half_theta < 180.0, "half_theta must be in range (0,180)"
        begin, end = (bear-half_theta)%360,(bear+half_theta)%360
        return (begin,end)

    
    @staticmethod
    #@print_fun_io
    def azimuth_to_bearing(az):
        """
        Converts a pyproj azimuth in the range [0,-180] or [0,180] to a bearing in range [0,360]
        """
        assert 0 <= abs(az) <= 180, "Azimuth must be in range [0,-180] or [0,180]"
        if az < 0:
            return 360.0+az
        else:
            return az

        
    @staticmethod
    #@print_fun_io 
    def in_sector(bear, begin, end):
        """
        >>> all([RoadWeatherLocator.in_sector((bear-1)%360,*RoadWeatherLocator.bearing_sector(bear, half_theta)) for half_theta in range(1,180) for bear in range(361)])
        True
        >>> all([RoadWeatherLocator.in_sector((bear+1)%360,*RoadWeatherLocator.bearing_sector(bear, half_theta)) for half_theta in range(1,180) for bear in range(361)])
        True
        """
        assert 0 <= bear <= 360, "Bearing must be in range [0,360]"
        if begin > end:
           return begin <= bear <= 360 or 0 <= bear <= end
        else:
            return begin <= bear <= end

        
    #@print_fun_io  
    def radius(self, lat, lon, meter_radius, bearing=None, sector_half_theta=None):
        q = numpy.array([lat, lon])
        sq_dist = numpy.array([numpy.inner(q-x, q-x) for x in self.seg_pts])
        sorted_indices = numpy.argsort(sq_dist)
        sorted_dist = sq_dist[sorted_indices]
        index = numpy.searchsorted(sorted_dist, self.degree_radius * self.degree_radius, side='right')

        final_sorted_dist = []
        final_sorted_indices = []
        if index > 0:
            az12, az21, dist = self.geod.inv([lon]*len(self.lon), [lat]*len(self.lat), self.lon[sorted_indices], self.lat[sorted_indices])
            sector = ()
            if bearing and sector_half_theta:
                sector = self.bearing_sector(bearing,sector_half_theta)
                
            for ind in range(len(dist)):
                if dist[ind] < meter_radius and \
                  (not sector or dist[ind] <= self.gps_accuracy or self.in_sector(self.azimuth_to_bearing(az12[ind]),*sector)):
                    final_sorted_dist.append(dist[ind])
                    final_sorted_indices.append(sorted_indices[ind])

            return numpy.array(final_sorted_indices), numpy.array(final_sorted_dist)
        else:
            return numpy.array([]), numpy.array([])
            

        

def main():

    print "building numpy index from mn_roads.nc"
    build_save_numpy_index("/d2/vii/data/static/cdl/mn_roads.nc", "latitude", "longitude", "seg_id", "mn_numpy_seg_pts", "mn_numpy_seg_id")

    print "building numpy index from mi_roads.nc"
    build_save_numpy_index("/d2/vii/data/static/cdl/mi_roads.nc", "latitude", "longitude", "seg_id", "mi_numpy_seg_pts", "mi_numpy_seg_id")

    print "building numpy index from nv_roads.20131111.nc"
    build_save_numpy_index("/d2/vii/data/static/cdl/nv_roads.nc", "latitude", "longitude", "seg_id", "nv_numpy_seg_pts", "nv_numpy_seg_id")

    print "establishing locator"
    michigan_wx_locator = RoadWeatherLocator("mi_numpy_seg_pts.npy", "mi_numpy_seg_id.npy", 20)

    print "getting radius"
    
   
    lat = 42.3366087
    lon = -82.99912

    (index, dist) = michigan_wx_locator.radius(lat, lon, 20)
    print "final index, dist: ", index, dist

    lat = 42.336676004
    lon = -82.9997885

    (index, dist) = michigan_wx_locator.radius(lat, lon, 20)
    print "final index, dist: ", index, dist

    #michigan_wx_locator.radius(44.2451, -84.2273, 0.0005)

    #print michigan_wx_locator.seg_pts[index]
    #num_neighbor = 1
    #print "single nn: ", michigan_wx_locator.locate(lat, lon, num_neighbor)

if __name__ == "__main__":

   main()

