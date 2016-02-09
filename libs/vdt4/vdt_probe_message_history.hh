//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_probe_message_history.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014-02-21 18:13:00 $
//
//==============================================================================

/**
 * @file vdt_probe_message_history.hh
 *
 * @class vdt_probe_message_history
 */

#ifndef VDT_PROBE_MESSAGE_HISTORY_HH
#define VDT_PROBE_MESSAGE_HISTORY_HH

#include <unordered_map>
#include <vector>
#include "vdt_probe_message_qc.hh"

using std::unordered_map;
using std::vector;

/**
 * @brief map each non-null vehicle identifier in the input probe messages to the set of messages having that identifier. If input probe messages have null vehicle identifiers, they will be ignored and will not be inserted into the output probe_message_history_map.
 *
 * @param[in] probe_message_input vector of input qc probe messages
 * @param[out] probe_message_history_map maps the non-null identifiers of the input probe messages to all probe messages having that identifier.
 * @post The vectors of messages in probe_message_history_map will be sorted in time decreasing order (i.e., the first message in the vector will be the most recent message). The time utilized for the sort order will be the observation time, not the arrival time.
 */
void vdt_probe_message_history(const vector<vdt_probe_message_qc> &probe_message_input, unordered_map<string, vector<vdt_probe_message_qc> > &probe_message_history_map);

#endif /* VDT_PROBE_MESSAGE_HISTORY_HH */
