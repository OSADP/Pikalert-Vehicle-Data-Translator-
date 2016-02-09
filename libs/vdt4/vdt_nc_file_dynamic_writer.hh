/* ============================================================================== */
/*                                                                                */
/*   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).  */
/*       All rights reserved.                                                     */
/*                                                                                */
/*       File: $RCSfile: fileheader,v $                                           */
/*       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           */
/*                                                                                */
/* ============================================================================== */

/**
 *
 * @file vdt_nc_file_dynamic_writer.hh
 *
 * @note <brief note description> [optional]
 *
 * @date 3/12/15
 *
 * @todo [optional]
 *  
 */

#ifndef VDT_NC_FILE_DYNAMIC_FILE_WRITER_HH
#define VDT_NC_FILE_DYNAMIC_FILE_WRITER_HH

/* Include files */
#include <vector>
#include "vdt_nc_file_writer.hh"
#include "pika_message.hh"

/* Constant and macros */

/* Types, structures and classes */
/**
 * @class vdt_nc_file_dynamic_writer
 * Represents any pika_message
 */
template <typename PIKA_MSG>
class vdt_nc_file_dynamic_writer : public vdt_nc_file_writer
{

  class PikaNetCdfVisitor : public boost::static_visitor<>
  {
    NcVar* var;
  public:
    PikaNetCdfVisitor(NcVar* ncvar):var(ncvar){}

    void operator()(const int& value)
    {
      var->put(&value,1);
    }

    void operator()(const double& value)
    {
      var->put(&value,1);
    }

    void operator()(const std::string& value)
    {
      var->put(value.data(),1,value.size());
    }

    void set_cur(size_t i)
    {
      var->set_cur(i);
    }
  };

public:
  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] cdl   cdl file
   */
  vdt_nc_file_dynamic_writer(const char* fpath, const char* cdl): vdt_nc_file_writer(fpath,cdl)
  {
  }

  ~vdt_nc_file_dynamic_writer()
  {
  }

  /**
   * @brief  writes a pika message
   * @param[in] stats  vector of pika_message
   * @return 0 on success, -1 on failure
   */
  int write_pika_messages(const std::vector<PIKA_MSG>& msgs)
  {
     if(msgs.empty())
       return -1;

     std::unique_ptr<NcError> ncErr(new NcError(NcError::silent_nonfatal));

     std::vector<std::string> keys;
     msgs[0].getkeys(keys);

     std::map<std::string, PikaNetCdfVisitor> name_visitor_map;
     for(const std::string& key : keys)
     {
       NcVar* var = nc->get_var(key.c_str());
       if(!var)
       {
	 std::cout << "Skipping msg output for key " << key << ". Field not found in cdl template " << cdl << std::endl;
       }
       else
       {
	 PikaNetCdfVisitor visitor(var);
	 name_visitor_map.insert(std::make_pair(key, visitor));
       }
     }

     for (size_t i = 0; i < msgs.size(); i++)
     {
       const PIKA_MSG& msg = msgs[i];

       for(auto& elem : name_visitor_map)
       {
	 PikaNetCdfVisitor& visitor = elem.second;
	 visitor.set_cur(i);
	 boost::apply_visitor(visitor, msg.find(elem.first)->second);
       }
     }
     return 0;
  }
};


/* Global variables */

/* Functions */


#endif /* VDT_NC_FILE_DYNAMIC_FILE_WRITER_HH */
