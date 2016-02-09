//==============================================================================
//
//   (c) Copyright, 2009 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: vdt_nearest_nbr.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_nearest_nbr.cc
 *
 */

// Include files 
#include "vdt_point.hh"
#include "vdt_nearest_nbr.hh"

// Constant and macros
const double Big_distance = 1000000;

// Types, structures and classes

// Global variables 

// Functions

vdt_nearest_nbr::vdt_nearest_nbr(const vector<double> &gridx, const vector<double> &gridy)
{
  num_input_points = 0;

  data_pts = 0;
  kd_tree = 0;

  if (gridx.size() == 0 || gridy.size() == 0)
    {
      error = string("number of input points was 0");
      return;
    }

  // Establish nearest neighbor data structures
  // Initialize the kd tree for nearest neighbor searching
  ann_dimension = 2;

  num_input_points = gridx.size();
  data_pts = annAllocPts(num_input_points, ann_dimension);

  for (int i=0; i<(int)gridx.size(); i++)
    {
      data_pts[i][0] = gridx[i];
      data_pts[i][1] = gridy[i];
      // There are valid values here
      //printf("data_pts[i][0], data_pts[i][1]: %f, %f\n", data_pts[i][0], data_pts[i][1]);
    }

  kd_tree = new ANNkd_tree(data_pts, (int)gridx.size(), ann_dimension);
}

vdt_nearest_nbr:: ~vdt_nearest_nbr()
{
  if (kd_tree != 0)
    {
      delete kd_tree;
      annDeallocPts(data_pts);
      annClose();
    }
}


int vdt_nearest_nbr::get_nearest_pt(double eps, double x, double y, int *ind, double *dist) const
{
  ANNidxArray nn_indices;
  ANNdistArray nn_distances;
  int num_neighbor = 1;

  if (num_input_points == 0)
    {
      *ind = -1;
      *dist = -1;
      return -1;
    }

  nn_indices = new ANNidx[num_neighbor];
  nn_distances = new ANNdist[num_neighbor];

  ANNpoint query_pt = annAllocPt(ann_dimension);	// allocate query point

  query_pt[0] = x;
  query_pt[1] = y;

  kd_tree->annkSearch(query_pt, num_neighbor, nn_indices, nn_distances, eps);

  *ind = nn_indices[0];
  *dist = nn_distances[0];

  delete [] nn_indices;
  delete [] nn_distances;
  annDeallocPt(query_pt);
  return 0;
}

int vdt_nearest_nbr::get_nearest_pts(double eps, double x, double y, int num_neighbor, vector<int> &indices, vector<double> &distances) const
{
  ANNidxArray nn_indices;
  ANNdistArray nn_distances;

  if (num_neighbor <= 0 || num_input_points == 0)
    {
      return -1;
    }
  nn_indices = new ANNidx[num_neighbor];
  nn_distances = new ANNdist[num_neighbor];

  ANNpoint query_pt = annAllocPt(ann_dimension);	// allocate query point

  query_pt[0] = x;
  query_pt[1] = y;

  kd_tree->annkSearch(query_pt, num_neighbor, nn_indices, nn_distances, eps);

  for (int i=0; i<num_neighbor; i++)
    {
      indices.push_back(nn_indices[i]);
      distances.push_back(nn_distances[i]);
    }

  delete [] nn_indices;
  delete [] nn_distances;
  annDeallocPt(query_pt);

  return 0;
}

int vdt_nearest_nbr::get_nearest_pts(double eps, double x, double y, int num_neighbor, double squared_radius, vector<int> &indices, vector<double> &distances) const
{
  ANNidxArray nn_indices;
  ANNdistArray nn_distances;
  
  if (num_neighbor <= 0 || num_input_points == 0)
    {
      return -1;
    }
  nn_indices = new ANNidx[num_neighbor];
  nn_distances = new ANNdist[num_neighbor];

  ANNpoint query_pt = annAllocPt(ann_dimension);	// allocate query point

  query_pt[0] = x;
  query_pt[1] = y;

  if (eps != 0)
    {
      kd_tree->annkFRSearch(query_pt, squared_radius, num_neighbor, nn_indices, nn_distances, eps);
    }
  else
    {
      kd_tree->annkFRSearch(query_pt, squared_radius, num_neighbor, nn_indices, nn_distances);
    }

  for (int i=0; i<num_neighbor; i++)
    {
      indices.push_back(nn_indices[i]);
      distances.push_back(nn_distances[i]);
    }

  delete [] nn_indices;
  delete [] nn_distances;
  annDeallocPt(query_pt);

  return 0;
}

int vdt_nearest_nbr::get_nearest_indices(double eps, const vector<double> &xvec, const vector<double> &yvec, vector<int> &indices, vector<double> &distances) const
{
  ANNidxArray nn_indices;
  ANNdistArray nn_distances;
  int num_neighbor = 1;

  if (num_input_points == 0)
    {
      return -1;
    }

  nn_indices = new ANNidx[num_neighbor];
  nn_distances = new ANNdist[num_neighbor];

  ANNpoint query_pt = annAllocPt(ann_dimension);	// allocate query point

  for (int i=0; i<(int)xvec.size(); i++)
    {
      query_pt[0] = xvec[i];
      query_pt[1] = yvec[i];

      if (eps != 0.0)
	{
	  kd_tree->annkSearch(query_pt, num_neighbor, nn_indices, nn_distances, eps);
	}
      else
	{
	  kd_tree->annkSearch(query_pt, num_neighbor, nn_indices, nn_distances);
	}

      indices.push_back(nn_indices[0]);
      distances.push_back(nn_distances[0]);
    }

  delete [] nn_indices;
  delete [] nn_distances;
  annDeallocPt(query_pt);

  return 0;
}

int vdt_nearest_nbr::get_grc_nearest_indices(double eps, int n, const vector<double> &xvec, const vector<double> &yvec, vector< vector<int> > &indices, vector< vector<double> > &distances) const
{
  ANNidxArray nn_indices;
  ANNdistArray nn_distances;
  int num_neighbor = n;

  if (indices.size() != xvec.size())
    {
      indices.resize(xvec.size());
    }
  if (distances.size() != xvec.size())
    {
      distances.resize(xvec.size());
    }

  if (num_input_points == 0 || num_neighbor <= 0)
    {
      return -1;
    }

  nn_indices = new ANNidx[num_neighbor];
  nn_distances = new ANNdist[num_neighbor];

  ANNpoint query_pt = annAllocPt(ann_dimension);	// allocate query point

  for (int i=0; i<(int)xvec.size(); i++)
    {
      query_pt[0] = xvec[i];
      query_pt[1] = yvec[i];

      if (eps != 0.0)
	{
	  kd_tree->annkSearch(query_pt, num_neighbor, nn_indices, nn_distances, eps);
	}
      else
	{
	  kd_tree->annkSearch(query_pt, num_neighbor, nn_indices, nn_distances);
	}

      // Now record nearest indices great circle distances
      for (int j=0; j<num_neighbor; j++)
	{
	  double lat = data_pts[nn_indices[j]][0];
	  double lon = data_pts[nn_indices[j]][1];
	  double dist = vdt_point::great_circle_distance(lat, lon, xvec[i], yvec[i]);
	  indices[i].push_back(nn_indices[j]);
	  distances[i].push_back(dist);
	}
    }

  delete [] nn_indices;
  delete [] nn_distances;
  annDeallocPt(query_pt);

  return 0;
}

// associates probe messages and their distances to their containing road segments
void vdt_nearest_nbr::get_inverse_indices(float max_dist_to_segment, const vector<int> &nn_indices, const vector<double> &nn_distances, unordered_map<int, vdt_index_distance> &nn_index_map)
{
  for (size_t i=0; i<nn_indices.size(); i++)
    {
      if (nn_distances[i] < max_dist_to_segment)
	{
	  auto itr = nn_index_map.find(nn_indices[i]);
	  if (itr == nn_index_map.end())
	    {
	      vdt_index_distance index_distance;
	      index_distance.index = nn_indices[i];
	      index_distance.indices.push_back(i);
	      index_distance.distance.push_back(nn_distances[i]);
	      nn_index_map[nn_indices[i]] = index_distance;
	    }
	  else
	    {
	      itr->second.index = nn_indices[i];
	      itr->second.indices.push_back(i);
	      itr->second.distance.push_back(nn_distances[i]);
	    }
	}
    }
}
