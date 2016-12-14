//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: qc_check.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/04/08 17:10:42 $
//
//==============================================================================

/**
 *
 * @file qc_check.hh
 *
 * @class qc_check
 *
 *   Quality control check class
 *
 */

#ifndef QC_CHECK_HH
#define QC_CHECK_HH

#include "config_reader.hh"
#include "vdt4/vdt_const.hh"
#include "vdt4/vdt_probe_message_qc.hh"
#include "vdt4/vdt_probe_message_qc_statistics.hh"
#include "vdt4/vdt_climatology_file_reader.hh"
#include "vdt4/vdt_spatial_check.hh"
#include "vdt_probe_message_datasets.hh"

int get_month_offset(double obstime);
unsigned char range_check( double min, double max, double value);
unsigned char range_check(int min, int max, int value);
unsigned char climate_range_check(double min, double max, double value);
unsigned char climate_range_check(int min, int max, int value);
unsigned char exclude_check(const std::vector<double>& excluded, double value, double tol);
unsigned char exclude_check(const std::vector<int>& excluded, int value, int tol);

inline unsigned char range_check(const std::string& min, const std::string& max, const std::string& value)
{
  throw VDT_EXCEPTION("Range check not implemented for type string");
}

class qc_check
{
public:
  /**
   * @brief qc_check
   * quality control check
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] ds probe message datasets object
   */
  qc_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *ds);
  
  /** @brief perform range check on given quality controlled probe message */
  virtual void operator()(vdt_probe_message_qc &probe_message) const {};

  virtual ~qc_check() {};

protected:
  /** @brief *configuration file parameters */
  const config_reader &cfg_reader;

  /** @brief vdt climatology file reader object */
  const vdt_climatology_file_reader *climate_data;

  /** @brief probe message datasets object */
  vdt_probe_message_datasets *datasets;
};

class qc_check2
{
public:
  virtual void operator()(vdt_probe_message& msg) const{};
};

template <class T>
class qc_range_check : public qc_check2
{
protected:
  std::string fieldname_;
  T min_;
  T max_;

  virtual std::string create_out_key() const
  {
    return fieldname_ + "_range_qc_passed";
  }

  //NO-OP
  //Override this class:member function to perform action before range check is performed
  virtual void pre_check(T value, vdt_probe_message& msg) const {};
  //NO-OP
  //Override this class:member function to perform follow-up action after range check is performed
  virtual void visit_result (T value, unsigned char range_check_result, vdt_probe_message& msg) const {};
public:
  qc_range_check(const std::string& fieldname, T min, T max) : fieldname_(fieldname), min_(min), max_(max)
  {
  }

  virtual void operator()(vdt_probe_message& msg) const
  {
    //std::cout << fieldname_ << std::endl;
    T value = msg.getattr<T>(fieldname_);
    pre_check(value, msg);
    unsigned char range_check_result = range_check(min_, max_, value); 
    msg.setattr(create_out_key(), range_check_result);
    visit_result(value, range_check_result, msg);
    //msg.dump();
  }
};

template <class T>
class qc_range_exclude_check : public qc_range_check<T>
{
protected:
  std::vector<T> excluded_;
  T tol_;

public:
  qc_range_exclude_check(const std::string& fieldname, const T& min, const T& max, const std::vector<T>& excluded, const T& tol) : qc_range_check<T>(fieldname, min, max), excluded_(excluded), tol_(tol)
  {
  }

  virtual void visit_result (T value, unsigned char range_check_result, vdt_probe_message& msg) const
  {
    if(range_check_result == vdt_probe_message_qc::PASSED)
    {
      unsigned char exclude_check_result = exclude_check(excluded_, value, tol_); 
      msg.setattr(qc_range_check<T>::create_out_key(), exclude_check_result);
    }
  }
};

typedef std::shared_ptr<qc_check2> QC_CHECK_PTR;

template <class T>
class dft_range_check : public qc_range_check <T>
{
public:
  dft_range_check(const std::string& fieldname, T min, T max) : qc_range_check<T>(fieldname, min, max)
  {
  }

  virtual std::string create_out_key() const
  {
    return qc_range_check<T>::fieldname_ + "_dft_passed";
  }
};

class range_check_visitor
  : public boost::static_visitor<QC_CHECK_PTR>
{
protected:
  std::string fieldname;
public:
    range_check_visitor(const std::string& fieldname) : fieldname(fieldname)
    {
    }

    template <typename T, typename U>
    QC_CHECK_PTR operator()( const T& min, const U& max) const
    {
      const std::string error = "min and max value must have the same type.  Check field " + fieldname;
      throw VDT_EXCEPTION(error);
    }

    template <typename T>
    QC_CHECK_PTR operator()( const T& min, const T& max) const
    {
      assert(min <= max);
      return QC_CHECK_PTR(new qc_range_check<T> (fieldname, min, max));
    }
};

class dft_range_check_visitor
  : public boost::static_visitor<QC_CHECK_PTR>
{
protected:
  std::string fieldname;
public:
    dft_range_check_visitor(const std::string& fieldname) : fieldname(fieldname)
    {
    }
  
    template <typename T, typename U>
    QC_CHECK_PTR operator()( const T& min, const U& max) const
    {
      const std::string error = "min and max value must have the same type.  Check field " + fieldname;
      throw VDT_EXCEPTION(error);
    }

    template <typename T>
    QC_CHECK_PTR operator()( const T& min, const T& max) const
    {
      assert(min <= max);
      return QC_CHECK_PTR(new dft_range_check<T> (fieldname, min, max));
    }
};

class qc_range_check_brake : public qc_range_check <int>
{
public:
  qc_range_check_brake(const std::string& fieldname, int min, int max) : qc_range_check(fieldname, min, max)
  {
  }

  virtual void visit_result (int status, unsigned char range_check_result, vdt_probe_message& msg) const
  {
    if (range_check_result == vdt_probe_message_qc::PASSED)
    {
      if ( status == vdt_probe_message::BRAKE_STATUS_ALL_OFF ||
           status == vdt_probe_message::BRAKE_STATUS_LF_ACTIVE  ||
           status == vdt_probe_message::BRAKE_STATUS_LR_ACTIVE ||
           status == vdt_probe_message::BRAKE_STATUS_RF_ACTIVE ||
           status == vdt_probe_message::BRAKE_STATUS_RR_ACTIVE ||
           status == vdt_probe_message::BRAKE_STATUS_ALL_ON )
        {
	  msg.setattr(this->create_out_key(), vdt_probe_message_qc::PASSED);
        }
      else
        {
	  msg.setattr(this->create_out_key(), vdt_probe_message_qc::FAILED);
        }
    }
  else if (range_check_result == vdt_probe_message_qc::FAILED)
    {
      msg.setattr(this->create_out_key(), vdt_probe_message_qc::FAILED);
    }
  else
    {
      msg.setattr(this->create_out_key(), range_check_result);
    }
  }
};

class qc_range_check_wiper : public qc_range_check <int>
{
public:
  qc_range_check_wiper(const std::string& fieldname, int min, int max) : qc_range_check(fieldname, min, max)
  {
  }

  virtual void visit_result (int status, unsigned char range_check_result, vdt_probe_message& msg) const
  {
    if (range_check_result == vdt_probe_message_qc::PASSED)
    {
      if ( status == vdt_probe_message::NOT_EQUIPPED ||
           status == vdt_probe_message::OFF  ||
           status == vdt_probe_message::INTERMITTENT ||
           status == vdt_probe_message::LOW ||
           status == vdt_probe_message::HIGH ||
           status == vdt_probe_message::WASHER ||
           status == vdt_probe_message::AUTOMATICPRESENT)

        {
	  msg.setattr(this->create_out_key(), vdt_probe_message_qc::PASSED);
        }
      else
        {
	  msg.setattr(this->create_out_key(), vdt_probe_message_qc::FAILED);
        }

    }
  else if (range_check_result == vdt_probe_message_qc::FAILED)
    {
      msg.setattr(this->create_out_key(), vdt_probe_message_qc::FAILED);
    }
  else
    {
      msg.setattr(this->create_out_key(), range_check_result);
    }
  }
};

class qc_range_check_headlight : public qc_range_check <int>
{
public:
  qc_range_check_headlight(const std::string& fieldname, int min, int max) : qc_range_check(fieldname, min, max)
  {
  }

  virtual void visit_result (int status, unsigned char range_check_result, vdt_probe_message& msg) const
  {
    if (range_check_result == vdt_probe_message_qc::PASSED)
    {
      msg.setattr(this->create_out_key(), vdt_probe_message_qc::PASSED);
    }
    else if (range_check_result == vdt_probe_message_qc::FAILED)
    {
      msg.setattr(this->create_out_key(), vdt_probe_message_qc::FAILED);
    }
    else
    {
      msg.setattr(this->create_out_key(), range_check_result);
    }
  }
};

template <class T>
class qc_climate_range_check : public qc_check2
{
    std::string fieldname_;
    const vdt_climatology_file_reader* climate_data_;
    const Array<float, 3>& min_grid_;
    const Array<float, 3>& max_grid_;
public:
  qc_climate_range_check(const std::string& fieldname, const vdt_climatology_file_reader *climate_data, const Array<float, 3>& min_grid, const Array<float, 3>& max_grid) : fieldname_(fieldname), climate_data_(climate_data), min_grid_(min_grid), max_grid_(max_grid)
  {
  }

  virtual std::string create_out_key() const
  {
    return fieldname_ + "_crt_passed";
  }

  virtual void operator()(vdt_probe_message& msg) const
  {
    // Get month, latitude, and longitude offsets
    double lat_offset, lon_offset;
    T min_value = vdt_const::SHORT_FILL_VALUE;
    T max_value = vdt_const::SHORT_FILL_VALUE;
    int month_offset = get_month_offset(msg.getattr<double>("obs_time"));
    climate_data_->latlon2xy(msg.getattr<double>("latitude"), msg.getattr<double>("longitude"), &lon_offset, &lat_offset);

    // Get min/max values for given offsets
    int lat_vec_offset;
    int lon_vec_offset;
    if (climate_data_->round_to_integer_coords(lat_offset, lon_offset, &lat_vec_offset, &lon_vec_offset) == 0)
    {
      climate_data_->get_min_max_climate_values(month_offset, lat_vec_offset, lon_vec_offset, min_grid_, max_grid_, &min_value, &max_value);
    }
    
    T value = msg.getattr<T>(fieldname_);
    unsigned char range_check_result = climate_range_check(min_value, max_value, value);
    msg.setattr(create_out_key(), range_check_result);
  }
};

class iqr_spatial_check2 : public qc_check2
{
protected:
  /** @brief *configuration file parameters */
  const config_reader &cfg_reader;

  /** @brief vdt climatology file reader object */
  const vdt_climatology_file_reader *climate_data;

  /** @brief probe message datasets object */
  vdt_probe_message_datasets *datasets;

  std::map<std::string, pika_type_id> var_type_map;
  std::map<std::string, std::string> var_met_field_map;
  std::map<std::string, vdt_spatial_check_iqr> met_spatial_map;

  void init();
public:
  iqr_spatial_check2(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : cfg_reader(config_reader_arg), climate_data(climate_data), datasets(probe_message_datasets)
  {
    init();
  }

  virtual void operator()(vdt_probe_message& msg) const;
};

/**
 * @class vdt_neighbor_check_iqr
 * perform iqr neighboring vehicle check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_neighbor_check_iqr : public iqr_spatial_check2
{
public:
  /**
   * @brief vdt neighbor check iqr constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   * @param[in] seg_stats vector of road segment statistics
   * @param[in] seg_indices vector of road segment indices corresponding to probe message ids
   * @param[in] seg_index_offset maps a segment index into its offset into the statistics vector
   */
  vdt_neighbor_check_iqr(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets, const vector<vdt_probe_message_qc_statistics> &seg_stats, const vector<int> &seg_indices, const unordered_map<int, int> &seg_index_offset) : iqr_spatial_check2(config_reader_arg, climate_data, probe_message_datasets), road_seg_stats(seg_stats), segment_indices(seg_indices), seg_index_offset_map(seg_index_offset)
  {
    init();
  }

  /**
   * @brief perform iqr neighboring vehicle check on given quality controlled probe message
   * @param[in] id road segment index 
   * @param[in] probe_message probe message qc input
   */
  void operator()(int id, vdt_probe_message_qc &probe_message) const;

  const vector<vdt_probe_message_qc_statistics> &road_seg_stats;
  const vector<int> &segment_indices;
  const unordered_map<int, int> &seg_index_offset_map;
};


/**
 * @class vdt_model_check
 * perform model check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_model_check : public qc_check
{
  std::map<std::string, double> var_delta_map;
  std::map<std::string, pika_type_id> var_type_map;
  std::map<std::string, std::string> var_met_field_map;
  std::set<std::string> model_set;

  void init();
public:
  /**
   * @brief vdt model check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  vdt_model_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {
    init();
  }

  /**
   * @brief perform model check on given quality controlled probe message
   * @param[in] id road segment index 
   * @param[in] probe_message input probe message
   */
  void operator()(int id, vdt_probe_message_qc &probe_message) const;

  float model_dewpoint_delta;
  float model_air_temp_delta;
  float model_bar_press_delta;
};

/**
 * @class vdt_stdev_check
 * perform standard deviation check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_stdev_check : public qc_check
{
  std::map<std::string, double> var_mult_map;
  std::map<std::string, pika_type_id> var_type_map;

  void init();
public:
  /**
   * @brief vdt stdev check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   * @param[in] seg_stats vector of road segment statistics
   * @param[in] seg_indices vector of road segment indices corresponding to probe message ids
   * @param[in] seg_index_offset maps a segment index into its offset into the statistics vector
   */
  vdt_stdev_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets, const vector<vdt_probe_message_qc_statistics> &seg_stats, const vector<int> &seg_indices, unordered_map<int, int> &seg_index_offset) : qc_check(config_reader_arg, climate_data, probe_message_datasets), road_seg_stats(seg_stats), segment_indices(seg_indices), seg_index_offset_map(seg_index_offset)
  {
    init();
  }

  /**
   * @brief perform standard deviation check on given quality controlled probe message
   * @param[in] id road segment index 
   * @param[in] probe_message input probe message
   */
  void operator()(int id, vdt_probe_message_qc &probe_message) const;

  const vector<vdt_probe_message_qc_statistics> &road_seg_stats;
  const vector<int> &segment_indices;
  const unordered_map<int, int> &seg_index_offset_map;
};

/**
 * @class vdt_combined_check
 * perform combined qc check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_combined_check : public qc_check
{
  std::map<std::string, std::string> var_met_field_map;

  void init();
public:
  /**
   * @brief vdt combined check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  vdt_combined_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {
    init();
  }

  /**
   * @brief perform combined algorithm check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};


#endif /* QC_CHECK_HH */
