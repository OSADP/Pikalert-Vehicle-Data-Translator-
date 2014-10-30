/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright UCAR (c) 1992 - 2007
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization.
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

//----------------------------------------------------------------------
// Module: var_input.cc
//
// Author: Gerry Wiener
//
// Date:   7/2/07
//
// Description:
//     Class for getting netCDF variable input
//----------------------------------------------------------------------

// Include files 
#include "../include/ncfc/ncfc.hh"

// Constant, macro and type definitions 

// Global variables 

void Var_input::init_var_input(int in_ncid, const vector<string> &var_names)
{
  m_error_status = 0;
  m_var_names = var_names;
  m_error_status = NCFC_get_variables(in_ncid, m_var_names, m_var_ptrs, m_var_fill_ptrs, m_var_types, m_var_units, m_var_ndims, m_var_dims, m_var_sizes, m_error);

  if (m_error_status != 0)
    {
      return;
    }

  for (unsigned int i=0; i<m_var_names.size(); i++)
    {
      m_var_index_map.insert(make_pair(m_var_names[i], i));
    }

  for (unsigned int i=0; i<m_file_dim_names.size(); i++)
    {
      m_file_dim_map.insert(make_pair(m_file_dim_names[i], m_file_dim_len[i]));
    }
}

void Var_input::init_var_input(int in_ncid, const vector<string> &var_names, const vector<string> &dim_names)
{
  m_error_status = 0;
  m_var_names = var_names;

  m_error_status = NCFC_get_variables(in_ncid, m_var_names, m_var_ptrs, m_var_fill_ptrs, m_var_types, m_var_units, m_var_ndims, m_var_dims, m_var_sizes, m_error);

  if (m_error_status != 0)
    {
      m_error = string("could not get variables from input");
      return;
    }
  else
    {
      m_file_dim_names = dim_names;
      m_error_status = NCFC_get_dims(in_ncid, m_file_dim_names, m_file_dim_len, m_error);
    }

  for (unsigned int i=0; i<m_var_names.size(); i++)
    {
      m_var_index_map.insert(make_pair(m_var_names[i], i));
    }

  for (unsigned int i=0; i<m_file_dim_names.size(); i++)
    {
      m_file_dim_map.insert(make_pair(m_file_dim_names[i], m_file_dim_len[i]));
    }
}

void Var_input::init_var_input(const char *in_file, const vector<string> &var_names)
{
  int in_ncid;
  m_error_status = 0;
  int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
  if (status != NC_NOERR)
    {
      m_error = string("could not open: ") + in_file;
      m_error_status = status;
      return;
    }

  Var_input::init_var_input(in_ncid, var_names);

  nc_close(in_ncid);
}

void Var_input::init_var_input(const char *in_file, const vector<string> &var_names, const vector<string> &dim_names)
{
  int in_ncid;
  m_error_status = 0;
  int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
  if (status != NC_NOERR)
    {
      m_error = string("could not open: ") + in_file;
      m_error_status = status;
      return;
    }

  Var_input::init_var_input(in_ncid, var_names, dim_names);

  nc_close(in_ncid);
}

Var_input::Var_input()
{
  m_error_status = 0;
}

Var_input::Var_input(const char *in_file, const vector<string> &var_names)
{
  Var_input::init_var_input(in_file, var_names);
}

Var_input::Var_input(const char *in_file, const vector<string> &var_names, const vector<string> &dim_names)
{
  Var_input::init_var_input(in_file, var_names, dim_names);
}

Var_input::Var_input(int in_ncid, const vector<string> &var_names)
{
  Var_input::init_var_input(in_ncid, var_names);
}

Var_input::Var_input(int in_ncid, const vector<string> &var_names, const vector<string> &dim_names)
{
  Var_input::init_var_input(in_ncid, var_names, dim_names);
}

void Var_input::get_file_dim_map(map<string, size_t> &file_dim_map_param) const
{
  file_dim_map_param = m_file_dim_map;
}

void Var_input::get_var_index_map(map<string, unsigned int> &var_index_map_param) const
{
  var_index_map_param = m_var_index_map;
}  

int Var_input::get_char_vec(const string &var_name, vector<char> &fvec)
{
  map<string, unsigned int>::iterator iter = m_var_index_map.find(var_name);
  if (iter != m_var_index_map.end())
    {
      int ind = iter->second;
      if (m_var_types[ind] == NC_CHAR)
	{
	  copy((char *)m_var_ptrs[ind], (char *)m_var_ptrs[ind] + m_var_sizes[ind], back_inserter(fvec));
	}
      else
	{
	  m_error = var_name + string(" variable is not a char");
	  return -1;
	}
    }
  else
    {
      m_error = string("could not find variable ") + var_name;
      return -1;
    }

  return 0;
}

int Var_input::get_double_vec(const string &var_name, vector<double> &vec)
{
  map<string, unsigned int>::iterator iter = m_var_index_map.find(var_name);
  if (iter != m_var_index_map.end())
    {
      int ind = iter->second;
      if (m_var_types[ind] == NC_DOUBLE)
	{
	  copy((double *)m_var_ptrs[ind], (double *)m_var_ptrs[ind] + m_var_sizes[ind], back_inserter(vec));
	}
      else
	{
	  m_error = var_name + string(" variable is not a double");
	  return -1;
	}
    }
  else
    {
      m_error = string("could not find variable ") + var_name;
      return -1;
    }

  return 0;
}

int Var_input::get_float_vec(const string &var_name, vector<float> &fvec)
{
  map<string, unsigned int>::iterator iter = m_var_index_map.find(var_name);
  if (iter != m_var_index_map.end())
    {
      int ind = iter->second;
      if (m_var_types[ind] == NC_FLOAT)
	{
	  copy((float *)m_var_ptrs[ind], (float *)m_var_ptrs[ind] + m_var_sizes[ind], back_inserter(fvec));
	}
      else
	{
	  m_error = var_name + string(" variable is not a float");
	  return -1;
	}
    }
  else
    {
      m_error = string("could not find variable ") + var_name;
      return -1;
    }

  return 0;
}

int Var_input::get_int_vec(const string &var_name, vector<int> &fvec)
{
  map<string, unsigned int>::iterator iter = m_var_index_map.find(var_name);
  if (iter != m_var_index_map.end())
    {
      int ind = iter->second;
      if (m_var_types[ind] == NC_INT)
	{
	  copy((int *)m_var_ptrs[ind], (int *)m_var_ptrs[ind] + m_var_sizes[ind], back_inserter(fvec));
	}
      else
	{
	  m_error = var_name + string(" variable is not a int");
	  return -1;
	}
    }
  else
    {
      m_error = string("could not find variable ") + var_name;
      return -1;
    }

  return 0;
}

int Var_input::get_short_vec(const string &var_name, vector<short> &fvec)
{
  map<string, unsigned int>::iterator iter = m_var_index_map.find(var_name);
  if (iter != m_var_index_map.end())
    {
      int ind = iter->second;
      if (m_var_types[ind] == NC_SHORT)
	{
	  copy((short *)m_var_ptrs[ind], (short *)m_var_ptrs[ind] + m_var_sizes[ind], back_inserter(fvec));
	}
      else
	{
	  m_error = var_name + string(" variable is not a short");
	  return -1;
	}
    }
  else
    {
      m_error = string("could not find variable ") + var_name;
      return -1;
    }

  return 0;
}

Var_input::~Var_input()
{
  for (unsigned int i=0; i<m_var_ptrs.size(); i++)
    {
      delete [] m_var_dims[i];
      if (m_var_types[i] == NC_FLOAT)
	{
	  delete [] (float *)m_var_ptrs[i];
	  delete [] (float *)m_var_fill_ptrs[i];
	}
      else if (m_var_types[i] == NC_INT)
	{
	  delete [] (int *)m_var_ptrs[i];
	  delete [] (int *)m_var_fill_ptrs[i];
	}
      else if (m_var_types[i] == NC_SHORT)
	{
	  delete [] (short *)m_var_ptrs[i];
	  delete [] (short *)m_var_fill_ptrs[i];
	}
      else if (m_var_types[i] == NC_CHAR)
	{
	  delete [] (char *)m_var_ptrs[i];
	  delete [] (char *)m_var_fill_ptrs[i];
	}
      else if (m_var_types[i] == NC_DOUBLE)
	{
	  delete [] (double *)m_var_ptrs[i];
	  delete [] (double *)m_var_fill_ptrs[i];
	}
    }
}
