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
 * @file pika_message.hh
 *
 * @note A dynamic variant based data structure for holding key,value records
 *
 * @date 3/6/15
 *
 * @todo [optional]
 *  
 */

#ifndef PIKA_MESSAGE_HH
#define PIKA_MESSAGE_HH

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>

#include "vdt_exception.hh"

#include "boost/variant.hpp"
#include <boost/lexical_cast.hpp>

using std::string;
using std::stringstream;

//Must be adjusted in concert with vdt_type_names
enum pika_type_id
{
  INT_TYPE,
  DOUBLE_TYPE,
  STRING_TYPE
};

//Must be adjusted in concert with pika_type_id
//Weird double brace initialization required in GCC 4.7.2 to eliminate warning.  This should probably be simplified to a single brace later.
static const std::unordered_map<std::string, pika_type_id> pika_type_names = {{"int",INT_TYPE}, {"short",INT_TYPE}, {"double",DOUBLE_TYPE}, {"float",DOUBLE_TYPE}, {"string",STRING_TYPE}};
static const std::unordered_map<size_t, std::string> pika_type_names_inv = {{INT_TYPE, "int"}, {DOUBLE_TYPE, "double"}, {STRING_TYPE, "string"}};

typedef boost::variant<int, double, std::string> PIKA_TYPE;
typedef std::unordered_map<std::string, PIKA_TYPE> STR_PIKA_TYPE_MAP;
typedef std::vector<std::pair<std::string,pika_type_id>> FIELD_NAME_TYPE_VEC;

class PikaTypeVisitor : public boost::static_visitor<pika_type_id>
{
public:
    pika_type_id operator()(const int& value) const
    {
        return INT_TYPE;
    }

    pika_type_id operator()(const double& value) const
    {
        return DOUBLE_TYPE;
    }

    pika_type_id operator()(const std::string& value) const
    {
        return STRING_TYPE;
    }
};

inline pika_type_id typename_to_id(const std::string& type)
{
  auto it = pika_type_names.find(type);
  if(it == pika_type_names.end())
    throw VDT_EXCEPTION("ERROR: invalid type " + type);
  else
    return it->second;
}

inline pika_type_id pika_typeid(const PIKA_TYPE& pikadatum)
{
  return boost::apply_visitor(PikaTypeVisitor(), pikadatum);
}

inline std::string pika_typename(const PIKA_TYPE& pikadatum)
{
  auto it = pika_type_names_inv.find(pika_typeid(pikadatum));
  if(it == pika_type_names_inv.end())
    throw VDT_EXCEPTION("ERROR: pika_type_id not matched to a string");
  else
    return it->second;
}

inline std::string pika_to_string(const PIKA_TYPE& pikadatum)
{
  return boost::lexical_cast<std::string>(pikadatum);
}

class pika_message 
{
protected:
  std::map<std::string, PIKA_TYPE> fields;
public:
  typedef std::map<std::string, PIKA_TYPE>::iterator iterator;
  typedef std::map<std::string, PIKA_TYPE>::const_iterator const_iterator;

  const_iterator find(const std::string& name)const
  {
    return fields.find(name);
  }

  iterator find(const std::string& name)
  {
    return fields.find(name);
  }

  iterator begin() 
  { 
    return fields.begin();
  }

  const_iterator begin() const 
  { 
    return fields.begin(); 
  }

  iterator end() 
  { 
    return fields.end();
  }

  const_iterator end() const 
  { 
    return fields.end();
  }

  template <typename T>
  T getattr(const std::string& name) const
  {
    const_iterator it = find(name);
    if(it == end())
    {
      throw VDT_EXCEPTION("Unable to find key " + name);
    }
    else
    {
      return boost::get<T>(it->second);
    }
  }

  template <typename T>
  T& getattr(const std::string& name)
  {
    iterator it = find(name);
    if(it == end())
    {
      throw VDT_EXCEPTION("Unable to find key " + name);
    }
    else
    {
      return boost::get<T>(it->second);
    }
  }

  template <typename T>
  void setattr(const std::string& name, T value)
  {
    iterator it = find(name);
    if(it == end())
    {
      fields[name] = value;
    } 
    else
    {
      it->second = value;
    }
  }

  template <typename T>
  void setattr(const std::map<std::string,T> vals, bool overwrite=true)
  {
    for(const auto& elem : vals)
    {
      if(overwrite || find(elem.first) == end())
      {
	setattr(elem.first, elem.second);
      }
    }
  }

  void getkeys(std::vector<std::string>& keys)const
  {
    for(const auto& kv : fields)
    {
      keys.push_back(kv.first);
    }
  }

  void getvalues(const std::vector<std::string>& keys, std::vector<PIKA_TYPE>& values)const
  {
    for(const std::string& key : keys)
    {
      const_iterator it = find(key);
      if(it != end())
      {
	values.push_back(it->second);
      }
    }
  }

  void dump(bool header = true, bool type = true)const
  {
    if(header)
    {
      std::cout << "field : value" << std::endl;
    }

    for(const auto& elem : fields)
    {
      if(type)
  	std::cout << "<type '" << pika_typename(elem.second) << "'>" << std::endl;
      std::cout << elem.first << " : " << elem.second << std::endl;
    }
  }

  /** @brief probe message header used for testing */
  std::string message_hdr()const
  {
    stringstream oss;

    bool first = true;
    for(const auto& kv : fields)
    {
      if(!first)
      {
	oss << ",";
      }
      oss << kv.first;
      first = false;
    }

    return oss.str();
  }

  /** @brief probe message string used for testing */
  std::string message_string()const
  {
     stringstream oss;
   
     bool first = true;
     for(const auto& kv : fields)
     {
       if(!first)
       {
	 oss << ",";
       }

       oss << kv.second;
       first = false;
     }

     return oss.str();
  }
};

//Prints to a JSON dictionary object
inline std::ostream& operator<<(std::ostream &ostr, const pika_message& msg)
{
  ostr << "{";
  bool first = true;
  for(const auto& field : msg)
  {
    if(!first)
    {
      ostr << ", ";
    }

    ostr << "\"" << field.first << "\" : ";
    if(pika_typeid(field.second) == STRING_TYPE)
    {
      ostr << "\"" << field.second << "\"";
    }
    else
    {
       ostr << field.second; 
    }
    
    first = false;
  }
  ostr << "}";
  return ostr;
}

 template <typename T>
 inline void getattr_cols(const std::vector<T>& msg_vec, 
			  const std::vector<std::string>& cols, 
			  std::map<std::string, std::vector<PIKA_TYPE>>& valuesmap)
 {
   for(const std::string& col : cols)
   {
     if(valuesmap.count(col) == 0)
     {
       valuesmap.insert(std::make_pair(col, std::vector<PIKA_TYPE>()));
     }
   }
   for(const T& msg : msg_vec)
   {
     for(const std::string& col : cols)
     {
       valuesmap[col].push_back(msg.find(col)->second);
     }
   }
 }


#endif /* PIKA_MESSAGE_HH */
