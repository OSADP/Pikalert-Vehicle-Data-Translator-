// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_road_segment_file_reader.hh"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* vdt_road_segment_file_reader::LATITUDE = "latitude";
const char* vdt_road_segment_file_reader::LONGITUDE = "longitude";
const char* vdt_road_segment_file_reader::ELEVATION = "elevation";
const char* vdt_road_segment_file_reader::SEG_NAME = "seg_name";
const char* vdt_road_segment_file_reader::SEG_ID = "seg_id";
const char* vdt_road_segment_file_reader::RWFS_ID = "rwfs_id";
const char* vdt_road_segment_file_reader::AUX_ID = "aux_id";
const char* vdt_road_segment_file_reader::ROAD_TYPE = "road_type";
const char* vdt_road_segment_file_reader::POINT_TYPE = "point_type";
const char* vdt_road_segment_file_reader::REC_NUM = "point_num";
const char* vdt_road_segment_file_reader::SEG_NAME_LEN = "seg_name_len";
const char* vdt_road_segment_file_reader::RWFS_ID_LEN = "rwfs_id_len";
const char* vdt_road_segment_file_reader::AUX_ID_LEN = "aux_id_len";


int vdt_road_segment_file_reader::get_road_segments(vector<vdt_road_segment>& segs)
{
  if (nc == NULL)
    {
      return 1;
    }
  NcVar* lat_var = nc->get_var(LATITUDE);
  NcVar* lon_var = nc->get_var(LONGITUDE);
  NcVar* name_var = nc->get_var(SEG_NAME);
  NcVar* seg_id_var = nc->get_var(SEG_ID);
  NcVar* rwfs_id_var = nc->get_var(RWFS_ID);
  NcVar* aux_id_var = nc->get_var(AUX_ID);
  NcVar* point_type_var = nc->get_var(POINT_TYPE);
  int recnum = nc->get_dim(REC_NUM)->size();
  int namesize = nc->get_dim(SEG_NAME_LEN)->size();
  int rwfs_id_size = nc->get_dim(RWFS_ID_LEN)->size();
  int aux_id_size = nc->get_dim(AUX_ID_LEN)->size();
  
  double* lats = new double[recnum];
  double* lons = new double[recnum];
  int* ids = new int[recnum];
  int* point_types = new int[recnum];
  char* names = new char[recnum*namesize];
  char* rwfs_ids = new char[recnum*rwfs_id_size];
  char* aux_ids = new char[recnum*aux_id_size];
  lat_var->get(lats,recnum);
  lon_var->get(lons,recnum);
  name_var->get(names,recnum,namesize);
  rwfs_id_var->get(rwfs_ids,recnum,rwfs_id_size);
  aux_id_var->get(aux_ids,recnum,aux_id_size);
  seg_id_var->get(ids,recnum);
  point_type_var->get(point_types,recnum);
  map<string, vector<vdt_point> > pts;
  map<string, vector<vdt_point> >::iterator it;
  map<int, string> seg_id_map;
  map<int, string> rwfs_id_map;
  map<int, string> aux_id_map;
  char* name = new char[namesize];
  char* rwfs_id = new char[rwfs_id_size];
  char* aux_id = new char[aux_id_size];
  for (int i = 0; i < recnum; i++)
    {
      // Road segments have a point type = 0
      if (point_types[i] == 0)
      {     
	memcpy(name, &names[i*namesize], namesize);
	string name_str(name);
	it = pts.find(name_str);
	if (it == pts.end())
        {
          pts[name_str] = vector<vdt_point>();
        }
      
	memcpy(rwfs_id, &rwfs_ids[i*rwfs_id_size], rwfs_id_size);
	string rwfs_id_str(rwfs_id);
      
	memcpy(aux_id, &aux_ids[i*aux_id_size], aux_id_size);
	string aux_id_str(aux_id);
      
	//printf("aux_id_str: %s\n", aux_id_str.c_str());
	//printf("lats[%d]: %f, lons[%d]: %f\n", i, lats[i], i, lons[i]);
      
	vdt_point pt;
	pt.set_latitude(lats[i]);
	pt.set_longitude(lons[i]);
	pts[name_str].push_back(pt);
	seg_id_map[ids[i]] = name_str;
	rwfs_id_map[ids[i]] = rwfs_id_str;
	aux_id_map[ids[i]] = aux_id_str;
      }
    }
  delete [] lats;
  delete [] lons;
  delete [] names;
  delete [] rwfs_ids;
  delete [] aux_ids;
  
  int initial_index = -1;
  for (int i=0; i<recnum; i++)
  {
    if (ids[i] != initial_index)
    {
      if (point_types[i] == 0)
      {
	string name_str = seg_id_map[ids[i]];
	string rwfs_id_str = rwfs_id_map[ids[i]];
	string aux_id_str = aux_id_map[ids[i]];
	vector<vdt_point> &points = pts[name_str];
	
	//printf("points.size(): %d\n", (int)points.size());
	//printf("ids[%d]: %d, aux_id_str: %s  \n", i, ids[i], aux_id_str.c_str());
	
	vdt_road_segment seg;
	seg.set_points(points);
	seg.set_road_name(name_str);
	seg.set_rwfs_id(rwfs_id_str);
	seg.set_aux_id(aux_id_str);
	int id = ids[i];
	seg.set_id(id);
	segs.push_back(seg);
	initial_index = ids[i];
      }
    }
  }
  return 0;
}

int vdt_road_segment_file_reader::get_rwis_sites(vector<vdt_road_segment>& rwis_sites)
{
  if (nc == NULL)
    {
      return 1;
    }
  NcVar* lat_var = nc->get_var(LATITUDE);
  NcVar* lon_var = nc->get_var(LONGITUDE);
  NcVar* name_var = nc->get_var(SEG_NAME);
  NcVar* seg_id_var = nc->get_var(SEG_ID);
  NcVar* rwfs_id_var = nc->get_var(RWFS_ID);
  NcVar* aux_id_var = nc->get_var(AUX_ID);
  NcVar* point_type_var = nc->get_var(POINT_TYPE);
  int recnum = nc->get_dim(REC_NUM)->size();
  int namesize = nc->get_dim(SEG_NAME_LEN)->size();
  int rwfs_id_size = nc->get_dim(RWFS_ID_LEN)->size();
  int aux_id_size = nc->get_dim(AUX_ID_LEN)->size();
  
  double* lats = new double[recnum];
  double* lons = new double[recnum];
  int* ids = new int[recnum];
  int* point_types = new int[recnum];
  char* names = new char[recnum*namesize];
  char* rwfs_ids = new char[recnum*rwfs_id_size];
  char* aux_ids = new char[recnum*aux_id_size];
  lat_var->get(lats,recnum);
  lon_var->get(lons,recnum);
  name_var->get(names,recnum,namesize);
  rwfs_id_var->get(rwfs_ids,recnum,rwfs_id_size);
  aux_id_var->get(aux_ids,recnum,aux_id_size);
  seg_id_var->get(ids,recnum);
  point_type_var->get(point_types,recnum);
  map<string, vector<vdt_point> > pts;
  map<string, vector<vdt_point> >::iterator it;
  map<int, string> seg_id_map;
  map<int, string> rwfs_id_map;
  map<int, string> aux_id_map;
  char* name = new char[namesize];
  char* rwfs_id = new char[rwfs_id_size];
  char* aux_id = new char[aux_id_size];
  for (int i = 0; i < recnum; i++)
    {
      // Road segments have a point type = 0
      if (point_types[i] == 1)
      {     
	memcpy(name, &names[i*namesize], namesize);
	string name_str(name);
	it = pts.find(name_str);
	if (it == pts.end())
        {
          pts[name_str] = vector<vdt_point>();
        }
      
	memcpy(rwfs_id, &rwfs_ids[i*rwfs_id_size], rwfs_id_size);
	string rwfs_id_str(rwfs_id);
      
	memcpy(aux_id, &aux_ids[i*aux_id_size], aux_id_size);
	string aux_id_str(aux_id);
      
	//printf("aux_id_str: %s\n", aux_id_str.c_str());
	//printf("lats[%d]: %f, lons[%d]: %f\n", i, lats[i], i, lons[i]);
      
	vdt_point pt;
	pt.set_latitude(lats[i]);
	pt.set_longitude(lons[i]);
	pts[name_str].push_back(pt);
	seg_id_map[ids[i]] = name_str;
	rwfs_id_map[ids[i]] = rwfs_id_str;
	aux_id_map[ids[i]] = aux_id_str;
      }
    }
  delete [] lats;
  delete [] lons;
  delete [] names;
  delete [] rwfs_ids;
  delete [] aux_ids;
  
  int initial_index = -1;
  for (int i=0; i<recnum; i++)
  {
    if (ids[i] != initial_index)
    {
      if (point_types[i] == 1)
      {
	string name_str = seg_id_map[ids[i]];
	string rwfs_id_str = rwfs_id_map[ids[i]];
	string aux_id_str = aux_id_map[ids[i]];
	vector<vdt_point> &points = pts[name_str];
	
	//printf("points.size(): %d\n", (int)points.size());
	//printf("ids[%d]: %d, aux_id_str: %s  \n", i, ids[i], aux_id_str.c_str());
	
	vdt_road_segment seg;
	seg.set_points(points);
	seg.set_road_name(name_str);
	seg.set_rwfs_id(rwfs_id_str);
	seg.set_aux_id(aux_id_str);
	int id = ids[i];
	seg.set_id(id);
	rwis_sites.push_back(seg);
	initial_index = ids[i];
      }
    }
  }
  return 0;
}

vdt_road_segment_file_reader::~vdt_road_segment_file_reader()
{
}
