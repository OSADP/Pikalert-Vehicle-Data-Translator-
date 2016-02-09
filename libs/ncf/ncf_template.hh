// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:32:06 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
//----------------------------------------------------------------------
//   Module: ncf_template.hh
//
//   Author: Gerry Wiener
//
//   Date:   10/11/06
//
//   Description: io using maps to provide flexibility in
//   inputting and outputting netCDF data
//----------------------------------------------------------------------

#ifndef NCF_TEMPLATE_HH
#define NCF_TEMPLATE_HH

#include <string>
#include <map>
#include <vector>
#include <netcdf.hh>

using namespace std;

// The following class is useful for storing all relevant information for a variable.
// It will release allocated memory when destroyed.
template<class T>
class NCF_input_map
{
public:
  NCF_input_map(map<string, int> &input_map);
  NCF_input_map(vector<string> &input_vec);
  ~NCF_input_map();
  void free_input_map();

  map<string, T *> var_data;
  map<string, int> var_fields;
  map<string, int> var_num_dims;
  map<string, long *> var_dims;
  map<string, map<string, NcValues *> *> var_atts;

private:
  NCF_input_map(const NCF_input_map<T>& im);
  NCF_input_map<T>& operator= (const NCF_input_map<T>& im);
};

template<class T>
void NCF_input_map<T>::free_input_map()
{
  typename map<string, T *>::iterator pdata;
  for (pdata = var_data.begin(); pdata != var_data.end(); pdata++)
    delete [] pdata->second;
  var_data.clear();

  // delete allocated dim arrays
  map<string, long *>::iterator pdims;
  for (pdims = var_dims.begin(); pdims != var_dims.end(); pdims++)
    delete [] pdims->second;
  var_dims.clear();

  map<string, map<string, NcValues *> *>::iterator pvar_atts;
  for (pvar_atts = var_atts.begin(); pvar_atts != var_atts.end(); pvar_atts++)
    {
      map<string, NcValues *>::iterator pvalues;
      for (pvalues = pvar_atts->second->begin(); pvalues != pvar_atts->second->end(); pvalues++)
	{
	  delete pvalues->second;
	}

      delete pvar_atts->second;
    }
  var_atts.clear();
}

template<class T>
NCF_input_map<T>::NCF_input_map(map<string, int> &input_map)
{
  typename map<string, int>::iterator pname;
  for (pname = input_map.begin(); pname != input_map.end(); pname++)
    var_fields[pname->first] = 0;
}

template<class T> NCF_input_map<T>::NCF_input_map(vector<string> &input_vec)
{
  for (unsigned int i=0; i<input_vec.size(); i++)
    var_fields[input_vec[i]] = 0;
}


template<class T>
NCF_input_map<T>::~NCF_input_map()
{
  free_input_map();
}
  
template<class T>
int get_var_info(NcFile *ncfp, map<string, T *> &var_data, map<string, int> &var_num_dims, map<string, long *> &var_dims, map<string, map<string, NcValues *> *> &var_atts, string &error)
{
  NcVar *vp = 0;
  int var_ct = 0;

  NcDim *rec_dimension = ncfp->rec_dim();
  if (rec_dimension->size() == 0)
    {
      error = string("record dimension has 0 size");
      return -1;
    }

  for (int i=0; (vp = ncfp->get_var(i)); i++)
    {
      if (var_data.count(vp->name()) > 0)
	{
	  var_ct++;
	  T *store = new T[vp->num_vals()];
	  var_num_dims[vp->name()] = vp->num_dims();
	  var_dims[vp->name()] = vp->edges();
	  vp->get(store, var_dims[vp->name()]);
	  var_data[vp->name()] = store;
	  map<string, NcValues *> *att_map = new map<string, NcValues *>;
	  NcAtt *ap;
	  
	  for (unsigned int j=0; (ap = vp->get_att(j)); j++)
	    {
	      var_atts[vp->name()] = att_map;
	      (*var_atts[vp->name()])[ap->name()] = ap->values();
	      delete ap;
	    }
	}
    }

  if ((int)var_data.size() != var_ct)
    {
      error = string("there are variables in the input map that were not in the input netCDF file");
      return -1;
    }
  else
    return 0;
}

template<class T>
void free_var_info(map<string, T *> &var_data, map<string, long *> &var_dims, map<string, map<string, NcValues *> *> &var_atts)
{
  typename map<string, T *>::iterator pvar_data;
  for (pvar_data = var_data.begin(); pvar_data != var_data.end(); pvar_data++)
    {
      delete [] pvar_data->second;
    }
  var_data.clear();

  map<string, long *>::iterator pvar_dims;
  for (pvar_dims = var_dims.begin(); pvar_dims != var_dims.end(); pvar_dims++)
    {
      delete [] pvar_dims->second;
    }
  var_dims.clear();

  map<string, map<string, NcValues *> *>::iterator pvar_atts;
  for (pvar_atts = var_atts.begin(); pvar_atts != var_atts.end(); pvar_atts++)
    {
      map<string, NcValues *>::iterator pvalues;
      for (pvalues = pvar_atts->second->begin(); pvalues != pvar_atts->second->end(); pvalues++)
	{
	  delete pvalues->second;
	}

      delete pvar_atts->second;
    }
  var_atts.clear();
}

template<class T>
int get_var_info(NcFile &ncfp, NCF_input_map<T> &im, string &error)
{
  int var_ct = 0;
  NcVar *vp = 0;

  NcDim *rec_dimension = ncfp.rec_dim();

  if (rec_dimension->size() == 0)
    {
      error = string("record dimension has 0 size");
      return -1;
    }

  for (int i=0; (vp = ncfp.get_var(i)); i++)
    {
      if (im.var_fields.count(vp->name()) > 0)
	{
	  var_ct++;
	  T *store = new T[vp->num_vals()];
	  im.var_num_dims[vp->name()] = vp->num_dims();
	  im.var_dims[vp->name()] = vp->edges();
	  vp->get(store, im.var_dims[vp->name()]);
	  im.var_data[vp->name()] = store;
	  map<string, NcValues *> *att_map = new map<string, NcValues *>;
	  NcAtt *ap;
	  
	  for (unsigned int j=0; (ap = vp->get_att(j)); j++)
	    {
	      im.var_atts[vp->name()] = att_map;
	      (*im.var_atts[vp->name()])[ap->name()] = ap->values();
	    }
	}
    }

  if ((int)im.var_fields.size() != var_ct)
    {
      error = string("there are variables in the input map that were not in the input netCDF file");
      return -1;
    }
  else
    return 0;
}

template <class T>
int put_var_info(NcFile *ncfp, map<string, T *> &var_map_data, map<string, long *> &var_dims, string &error)
{
  int var_ct = 0;
  NcVar *vp = 0;

  for (int i=0; (vp = ncfp->get_var(i)); i++)
    {
      if (var_map_data.count(vp->name()) > 0)
	{
	  var_ct++;
	  vp->put(var_map_data[vp->name()], var_dims[vp->name()]);
	}
    }

  if ((int)var_map_data.size() != var_ct)
    {
      error = string("there are variables in the output map that were not in the output netCDF file");
      return -1;
    }
  else
    return 0;
  return 0;
}


#endif /* NCF_TEMPLATE_HH */
