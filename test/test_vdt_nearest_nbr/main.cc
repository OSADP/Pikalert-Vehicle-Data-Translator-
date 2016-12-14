
//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: main.cc,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2013/08/30 17:08:16 $
//
//==============================================================================

/**
 * @file main.cc
 *
 * test road segment file reader functionality
 *
 * @date 7/1/13
 */

// Include files 
#include <algorithm>
#include <math.h>
#include "vdt4/vdt_nearest_nbr.hh"

int Road_segment[] = {
  1,   // seg_id(0)
  1,   // seg_id(1)
  1,   // seg_id(2)
  1,   // seg_id(3)
  1,   // seg_id(4)
  1,   // seg_id(5)
  1,   // seg_id(6)
  1,   // seg_id(7)
  1,   // seg_id(8)
  2,   // seg_id(9)
  2,   // seg_id(10)
  3,   // seg_id(11)
  3,   // seg_id(12)
  3,   // seg_id(13)
  3,   // seg_id(14)
  4,   // seg_id(15)
  4,   // seg_id(16)
  4,   // seg_id(17)
  4,   // seg_id(18)
  5,   // seg_id(19)
  5   // seg_id(20)
};
  
double Road_latitude_array[] = {
    46.8453189999721,   // latitude(0)
    46.8449630006421,   // latitude(1)
    46.8446430002744,   // latitude(2)
    46.844242000668,   // latitude(3)
    46.8437740006698,   // latitude(4)
    46.8431820003492,   // latitude(5)
    46.8423810002831,   // latitude(6)
    46.8411220006799,   // latitude(7)
    46.8375130006246,   // latitude(8)
    46.8375130006246,   // latitude(9)
    46.8090629998577,   // latitude(10)
    46.8090629998577,   // latitude(11)
    46.8080819995848,   // latitude(12)
    46.8072619995418,   // latitude(13)
    46.7926819998746,   // latitude(14)
    46.7926819998746,   // latitude(15)
    46.7926710002666,   // latitude(16)
    46.7865710004515,   // latitude(17)
    46.7818710004465,   // latitude(18)
    46.7818710004465,   // latitude(19)
    46.7665819995598};   // latitude(20)

const double Road_longitude_array[] = {
    -96.6437010001009,   // longitude(0)
    -96.6400409994924,   // longitude(1)
    -96.6376069996631,   // longitude(2)
    -96.635882000154,   // longitude(3)
    -96.6345160001131,   // longitude(4)
    -96.6331509992191,   // longitude(5)
    -96.6316860000087,   // longitude(6)
    -96.6298549992087,   // longitude(7)
    -96.625376000402,   // longitude(8)
    -96.625376000402,   // longitude(9)
    -96.5901490000762,   // longitude(10)
    -96.5901490000762,   // longitude(11)
    -96.5888989999887,   // longitude(12)
    -96.5877849998777,   // longitude(13)
    -96.5676730005084,   // longitude(14)
    -96.5676730005084,   // longitude(15)
    -96.5674980005322,   // longitude(16)
    -96.5589979997577,   // longitude(17)
    -96.5522529998759,   // longitude(18)
    -96.5522529998759,   // longitude(19)
    -96.5267010005506};   // longitude(20)


// Constants and macros
const double EPS = 10E-7;

// Types

// Structures, unions and classes

// Global variables 

// Functions


void pair_sort (vector<int> &ind, vector<double> &distances, vector< std::pair<double, int> > &distance_index)
{
  if (ind.size() != distances.size())
    return;

  for (size_t i=0; i<ind.size(); i++)
    {
      distance_index.push_back(std::make_pair(distances[i], ind[i]));
    }

  std::sort (distance_index.begin(), distance_index.end());
}

int main(int argc, char **argv)
{
  vector<double> road_latitude_vec(Road_latitude_array, Road_latitude_array + sizeof(Road_latitude_array)/sizeof(Road_latitude_array[0]));
  vector<double> road_longitude_vec(Road_longitude_array, Road_longitude_array + sizeof(Road_longitude_array)/sizeof(Road_longitude_array[0]));

  int test_failure_count = 0;

  vdt_nearest_nbr road_segment_nearest_nbr(road_latitude_vec, road_longitude_vec);
  
  vector<double> qc_msg_latitude(Road_latitude_array, Road_latitude_array + (sizeof(Road_latitude_array)/sizeof(double)));
  vector<double> qc_msg_longitude(Road_longitude_array, Road_longitude_array + (sizeof(Road_longitude_array)/sizeof(double)));
  vector< vector<int> > qc_msg_point_indices;
  vector< vector<double> > qc_msg_segment_distances;

  double eps = 0;
  int max_nearest_neighbors = 5;

  vector<vector<int> > indices;
  vector<vector<double> > distances;

  int ret = road_segment_nearest_nbr.get_grc_nearest_indices(eps, max_nearest_neighbors, qc_msg_latitude, qc_msg_longitude, indices, distances);


  for (unsigned int i=0; i<indices.size(); i++)
    {
      vector< std::pair<double, int> > distance_index;

      pair_sort(indices[i], distances[i], distance_index);
      printf("\nconsidering point with index %d: %lg %lg\n", i, qc_msg_latitude[i], qc_msg_longitude[i]);
      
      for (unsigned int j=0; j<indices[i].size(); j++)
	{
	  printf("indices[%d][%d] = %d, distances[%d][%d] = %lg\n", i, j, indices[i][j], i, j, distances[i][j]);
	}
      for (int j=0; j<distance_index.size(); j++)
	{
	  printf("distance_index[%d] = %lg %d\n", j, distance_index[j].first, distance_index[j].second);
	}

    }


#ifdef NOTNOW
  // Change the longitudes by adding 0.001
  for (unsigned int i=0; i<qc_msg_longitude.size(); i++)
    {
      qc_msg_longitude[i] += 0.001;
    }

  ret = road_segment_nearest_nbr.get_grc_nearest_indices(eps, max_nearest_neighbors, qc_msg_latitude, qc_msg_longitude, indices, distances);

  for (unsigned int i=0; i<indices.size(); i++)
    {
      printf("indices[%d] = %d, distances[%d] = %lg\n", i, indices[i], i, distances[i]);
    }
#endif

  return 0;
}
