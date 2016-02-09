//==============================================================================
//
//   (c) Copyright, 2009 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: vdt_nearest_nbr.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_nearest_nbr.hh
 *
 * @class vdt_nearest_nbr
 */

#ifndef VDT_NEAREST_NBR_HH
#define VDT_NEAREST_NBR_HH

#include <ANN/ANN.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "vdt_index_distance.hh"

using std::string;
using std::unordered_map;
using std::vector;

/**
 * @class vdt_nearest_nbr
 */
class vdt_nearest_nbr
{
public:
  /** Constructor 
   * @brief get_nearest_pt 
   * get nearest neighbor to a query point
   * @param[in] xcoord x coordinates of input points
   * @param[in] ycoord y coordinates of input points
   */
  vdt_nearest_nbr(const vector<double> &xcoord, const vector<double> &ycoord);

  /** Destructor */
  virtual ~vdt_nearest_nbr();

  /** @brief get_nearest_pt 
   * get nearest neighbor to a query point
   * @param[in] eps if eps != 0, find approximate nearest neighbor within (1+eps) * dist. Set eps = 0 for exact search.
   * @param[in] x x coordinate of query point
   * @param[in] y y coordinate of query point
   * @param[out] ind index of nearest point
   * @param[out] dist distance to nearest point
   * @return return 0 on success, -1 on failure
   */
  int get_nearest_pt(double eps, double x, double y, int *ind, double *dist) const;
    
  /** @brief get_nearest_pts 
   * get nearest neighbors to a query point
   * @param[in] eps if eps != 0, find approximate nearest neighbor within (1+eps) * dist. Set eps = 0 for exact search.
   * @param[in] x x coordinate of query point
   * @param[in] y y coordinate of query point
   * @param[in] num_neighbor number of desired neighbors
   * @param[out] indices indices of nearest points
   * @param[out] distances distances of nearest points
   * @return return 0 on success, -1 on failure
   */
  int get_nearest_pts(double eps, double x, double y, int num_neighbor, vector<int> &indices, vector<double> &distances) const;
  
  /** @brief get_nearest_pts 
   * get nearest neighbors to a query point
   * @param[in] eps if eps != 0, find approximate nearest neighbor within (1+eps) * dist. Set eps = 0 for exact search.
   * @param[in] x x coordinate of query point
   * @param[in] y y coordinate of query point
   * @param[in] num_neighbor number of desired neighbors
   * @param[in] squared_radius search radius squared
   * @param[out] indices indices of nearest points
   * @param[out] distances distances of nearest points
   * @return return 0 on success, -1 on failure
   */
  int get_nearest_pts(double eps, double x, double y, int num_neighbor, double squared_radius, vector<int> &indices, vector<double> &distances) const;

  /** @brief get_nearest_indices 
   * get indices of nearest neighbors
   * @param[in] eps if eps != 0, find approximate nearest neighbor within (1+eps) * dist. Set eps = 0 for exact search.
   * @param[in] xcoord vector of x coordinates
   * @param[in] ycoord vector of y coordinates
   * @param[out] nn_indices vector of indices of points closest to (x, y) points
   * @param[out] nn_distances vector of distances of points closest to (x, y) points
   * @return return 0 on success, -1 on failure
   */
  int get_nearest_indices(double eps, const vector<double> &xcoord, const vector<double> &ycoord, vector<int> &nn_indices, vector<double> &nn_distances) const;
    
  /** @brief get_grc_nearest_indices 
   * Get indices of the n nearest neighbors in euclidean distance. Assumes the underlying point are lat, longs. Returns greatest circle distances instead of euclidean distances.
   * @param[in] eps if eps != 0, find approximate nearest neighbor within (1+eps) * dist. Set eps = 0 for exact search.
   * @param[in] n > 0, find up to n nearest neighbors
   * @param[in] xcoord vector of x coordinates
   * @param[in] ycoord vector of y coordinates
   * @param[out] nn_indices vector contains indices of nearest points (x, y) in euclidean distance
   * @param[out] nn_distances vector contains great circle km distances of query points to their nearest neighbors
   * @return return 0 on success, -1 on failure
   */
  int get_grc_nearest_indices(double eps, int n, const vector<double> &xvec, const vector<double> &yvec, vector< vector<int> > &indices, vector< vector<double> > &distances) const;

    
  /** @brief get_inverse_indices 
   * construct map that maps indices of destination nearest neighbor points back to the vector of indices of source points and their distances to the destination point
   * @param[in] max_dist_to_segment maximum distance in degrees of probe message lat,long location to seg midpoint lat,long location
   * @param[in] nn_indices vector of indices of nn points closest to source (x, y) points
   * @param[in] nn_distances vector of distances of nn points closest to (x, y) points
   * @param[out] nn_index_map map that maps nn indices back to pair consisting of vector of indices of source points and vector of distances of source points to their destination nn points
   */
  static void get_inverse_indices(float max_dist_to_segment, const vector<int> &nn_indices, const vector<double> &nn_distances, unordered_map<int, vdt_index_distance> &nn_index_map);

  /** @brief get_error return error. For example, if constructor failed, error can be found here
   */
  const string &get_error() const {return error;}

private:
  int num_input_points;
  int ann_dimension;
  ANNkd_tree *kd_tree;
  ANNpointArray data_pts;
  string error;
};


#endif /* VDT_NEAREST_NBR_HH */
