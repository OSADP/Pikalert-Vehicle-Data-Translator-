
#include "vdt_probe_message_file_reader.hh"

const char* vdt_probe_message_file_reader::REC_NUM = "rec_num";

vdt_probe_message_file_reader::~vdt_probe_message_file_reader()
{
}

int vdt_probe_message_file_reader::get_probe_messages(const FIELD_NAME_TYPE_VEC& name_id_pairs, vector<vdt_probe_message>& msgs)
{
  int recnum = nc->get_dim(REC_NUM)->size(); 

  msgs.reserve(recnum);

  int ival;
  double dval;

  const vdt_probe_message default_msg(name_id_pairs);
  for (int i = 0; i < recnum; i++)
  {
    msgs.push_back(default_msg);
    vdt_probe_message& msg = msgs[msgs.size()-1];
    for(const auto& var_pair : name_id_pairs)
    {
        NcVar* nc_var = nc->get_var(var_pair.first.c_str());
        nc_var->set_cur(i);

	//Extract as type to boost variant
	switch(var_pair.second)
	{
            case INT_TYPE:
	    {
	      nc_var->get(&ival,1);
	      msg.setattr(var_pair.first, ival);
	      break;
	    }
            case DOUBLE_TYPE:
	    {
	      nc_var->get(&dval,1);
	      msg.setattr(var_pair.first, dval);
	      break;
	    }
            case STRING_TYPE:
	    {
	      if(nc_var->num_dims() != 2)
	      {
		throw VDT_EXCEPTION("Anticipating an array of strings.");
	      }

	      long size = nc_var->get_dim(1)->size();
	      std::vector<char> strval(size+1, '\0');
	      nc_var->get(strval.data(),1,size);
	      msg.setattr(var_pair.first, std::string(strval.data()));
	      break;
	    }
            default:
	    {
	      throw VDT_EXCEPTION("Invalid variable type");
	    }
	} //End switch
    }
    //msg.dump();
  }

  return 0;
}
