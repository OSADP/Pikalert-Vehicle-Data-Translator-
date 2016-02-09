// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:23:05 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: parse_treatment_list.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description:
 *              This file contains a routine that reads and parses a user-provided 
 *              configuration file containing one or more user defined treatment plans.
 *              The routine finds the treatment plan associated with the indicated site 
 *              and stores the treatment plan.
 *     
 *     parse_treatment_list()...Main routine described above.
 *     
 */

#include "stdio.h"
#include "stdlib.h"
#include <ncf/ncf.hh> 
#include <log/log.hh>
#include "RulesOfPractice.hh"

extern ROPParamsStruct ROP_Params;
extern Log *proc_log;


//  
//  Routine: parse_treatment_list()
//  
//  Description:
//      Opens, reads, and parses a configuration file to get the user-defined treatment 
//      associated with the specified site. This plan is incorporated into internal data 
//      structures.
//  
//  Inputs: 
//    treatment_def_file_name...The name of the text file containing the treament plan.
//    site_id...................The ID of the site.
//    forc_time.................The start (Unix) time of the forecast period.
//    num_fcst..................The number of hours in the forecast period.
//    app_rate_units............Units we are expecting the application rates to be in
//  
//  Outputs:
//    TreatmentList.............The internal data structure containing the treatment plan.
//  
//  Returns:
//    num_treat.................The number of treatments found in the user-defined plan that
//                              fall within the forecast period.
//  


int parse_treatment_list(char *treatment_def_file_name, int site_id, double forc_time,
			 int num_fcst, int app_rate_units, Treatment *TreatmentList)
{
  FILE *fp;
  int num_treat;    // Number of treatments in config file
  int treat_count;  // Number of treatments falling within 48 hr time window
  int nr;           // Number of items read on line of treatment
  int i;
  int id;
  int MAX_LINE = 80;
  char buf[MAX_LINE];

  if (NULL == (fp = fopen(treatment_def_file_name, "r")))
    {
      proc_log->write_time("Error: open user defined treatment file failed\n");
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  id = -1;

  // Find this site's plan
  while (site_id != id)
    {
      if (fgets(buf, 80, fp) == NULL)
	{
	  proc_log->write_time("Error: Read site ID in user def treatment plan failed\n");
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      sscanf(buf, "%d", &id);
    }

  // Get number of treatments in plan. Include the newline in the format string
  // so that it is consumed by this next fscanf call.
  if (1 != fscanf(fp, "%d\n", &num_treat))
    {
      proc_log->write_time("Error: Read num user defined treatments failed\n");
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  treat_count = 0;
  for (i=0; i<num_treat; i++)
    {
			InitNewTreatment( &TreatmentList[treat_count] );
      // Read the line, parse values
      fgets(buf, 80, fp);
      nr = sscanf(buf, "%d %f %d %d", &TreatmentList[treat_count].StartTime, 
		  &TreatmentList[treat_count].ApplicationRate,
		  &TreatmentList[treat_count].Chemicals.UserChemType,
		  &TreatmentList[treat_count].Chemicals.UserChemForm);

      // If only 3 items are in the treatment, fill in the chem form for backward
      // compatibility
      if (nr == 3)
	{
	  TreatmentList[treat_count].Chemicals.UserChemForm = C_CHEMFORM_PREWET;
	}
      else if (nr != 4)
	{
	  proc_log->write_time("Error: Wrong number of items in treatment number %d for site %d\n", i, id);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      //
      // Convert to internal units - lb/lane-mile - assuming that the provided
      // treatment are in 'app_rate_units'. Liquids must also be converted to
      // their solid equivalents. If the form is liquid, and app_rate_units
      // are not lb/lane-mile, assume they are in liters/lane-km, since that is
      // currently the only other liquid (volume) units supported.
      //

      TreatmentList[treat_count].Chemicals.Units = C_CHEMUNITS_LBS_LANEMILE;

      if (app_rate_units != C_CHEMUNITS_LBS_LANEMILE)
	{
	  if ( TreatmentList[treat_count].Chemicals.UserChemForm == C_CHEMFORM_LIQUID )
	    TreatmentList[treat_count].ApplicationRate =
	      ConvertChemUnits(TreatmentList[treat_count].ApplicationRate,
			       TreatmentList[treat_count].Chemicals.UserChemType,
			       C_CHEMUNITS_LITERS_LANEKM,
			       C_CHEMUNITS_LBS_LANEMILE);
	  else
	    TreatmentList[treat_count].ApplicationRate =
	      ConvertChemUnits(TreatmentList[treat_count].ApplicationRate,
			       TreatmentList[treat_count].Chemicals.UserChemType,
			       app_rate_units,
			       C_CHEMUNITS_LBS_LANEMILE);
	}
      else
	{
	  if ( TreatmentList[treat_count].Chemicals.UserChemForm == C_CHEMFORM_LIQUID )
	    TreatmentList[treat_count].ApplicationRate =
	      ConvertChemUnits(TreatmentList[treat_count].ApplicationRate,
			       TreatmentList[treat_count].Chemicals.UserChemType,
			       C_CHEMUNITS_GALS_LANEMILE,
			       C_CHEMUNITS_LBS_LANEMILE);
	}


      // Store Treatment (if it lies in forecast time period)
      if (TreatmentList[treat_count].StartTime >= forc_time &&
	  TreatmentList[treat_count].StartTime < forc_time + num_fcst * SEC_PER_HOUR)
	{
	  TreatmentList[treat_count].StartTime = 
	    (int)(TreatmentList[treat_count].StartTime - forc_time) / SEC_PER_HOUR;

	  int RouteDelta = (int)(0.5+(ROP_Params.RouteRoundtripTiming+ROP_Params.TruckTurnAroundTime)/60.0);
	  TreatmentList[treat_count].EndTime = TreatmentList[treat_count].StartTime + RouteDelta;
	  TreatmentList[treat_count].EffectiveTime = TreatmentList[treat_count].StartTime + RouteDelta;

	  TreatmentList[treat_count].DoPlowing = 1;
	  if (TreatmentList[treat_count].ApplicationRate > 0)
	    TreatmentList[treat_count].ApplyChemicals = 1;
	  else
	    TreatmentList[treat_count].ApplyChemicals = 0;
	  
	  proc_log->write_time(2, "treatment %d  at t=%d  rate=%.1f  type = %d, form = %d\n", 
			       i, TreatmentList[treat_count].StartTime, 
			       TreatmentList[treat_count].ApplicationRate,
			       TreatmentList[treat_count].Chemicals.UserChemType,
			       TreatmentList[treat_count].Chemicals.UserChemForm);

	  TreatmentList[treat_count].ExplanationIndx = 17;
	  TreatmentList[treat_count].Explanation = treatmentExplanations[TreatmentList[treat_count].ExplanationIndx];

	  treat_count++;
	}
    }


  fclose(fp);
  return(treat_count);
}
