
#include "vdt_road_segment_assessment_file_writer.hh"

const char* vdt_road_segment_assessment_file_writer::ALL_HAZARDS = "all_hazards";
const char* vdt_road_segment_assessment_file_writer::PAVEMENT_CONDITION = "pavement_condition";
const char* vdt_road_segment_assessment_file_writer::PRECIPITATION = "precipitation";
const char* vdt_road_segment_assessment_file_writer::ROAD_SEGMENT_ID = "road_segment_id";
const char* vdt_road_segment_assessment_file_writer::VISIBILITY = "visibility";

vdt_road_segment_assessment_file_writer::vdt_road_segment_assessment_file_writer(const char* fpath, const char* cdl) : vdt_nc_file_writer(fpath,cdl)
{
}

vdt_road_segment_assessment_file_writer::~vdt_road_segment_assessment_file_writer()
{
}

int vdt_road_segment_assessment_file_writer::write_road_segment_assessment(vector<vdt_road_segment_assessment>& assessments)
{
  NcVar* all_hazards_var = nc->get_var(ALL_HAZARDS);
  NcVar* pavement_condition_var = nc->get_var(PAVEMENT_CONDITION);
  NcVar* precipitation_var = nc->get_var(PRECIPITATION);
  NcVar* road_segment_id_var = nc->get_var(ROAD_SEGMENT_ID);
  NcVar* visibility_var = nc->get_var(VISIBILITY);

  for (size_t i = 0; i < assessments.size(); i++)
    {
      vdt_road_segment_assessment assessment = assessments[i];
      short all_hazards = assessment.get_all_hazards();
      short pavement_condition = assessment.get_pavement_condition();
      short precipitation = assessment.get_precip_type();
      int road_segment_id = assessment.get_road_segment_id();
      short visibility = assessment.get_visibility();

      all_hazards_var->set_cur(i);
      pavement_condition_var->set_cur(i);
      precipitation_var->set_cur(i);
      road_segment_id_var->set_cur(i);
      visibility_var->set_cur(i);
      
      all_hazards_var->put(&all_hazards,1);
      pavement_condition_var->put(&pavement_condition,1);
      precipitation_var->put(&precipitation,1);
      road_segment_id_var->put(&road_segment_id,1);
      visibility_var->put(&visibility,1);
    }

  return 0;
}
