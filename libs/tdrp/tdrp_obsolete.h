/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:35:09 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */

#ifndef TDRP_OBSOLETE_WAS_INCLUDED
#define TDRP_OBSOLETE_WAS_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/******************************************************************
 * tdrp_obsolete.c
 *
 * API for obsolete C TDRP functions.
 *
 * These are included for backward compatibility only.
 * Do not use these functions, they will later be
 * removed from the library.
 *
 * Mike Dixon, RAP, NCAR, POBox 3000, Boulder, CO, 80307-3000
 *
 * Sept 1998
 */

/*************
 * TDRP_read()
 *
 *   char *in_file: the parameter file to read in.
 *
 *   TDRPtable *table: table obtained from <mod>_tdrp_init().
 *
 *   void *user_struct: this is actually &<mod>_params,
 *     as declared in <mod>_tdrp.h.
 *     TDRP_read places the values of the parameters in this structure.
 * 
 *   char **override_list: A null-terminated list of overrides to the
 *     parameter file.
 *     An override string has exactly the format of the
 *     parameter file itself.
 *
 *  Returns TRUE on success, FALSE on failure.
 */

extern int TDRP_read(const char *param_file_path, TDRPtable *table,
		     void *user_struct, char **override_list);

/*********************
 * TDRP_print_params()
 * 
 * Print params file, with module name
 */

extern void TDRP_print_params(const TDRPtable *table, const void *user_struct,
			      const char *module, int print_comments);

/*********************
 * TDRP_print_struct()
 *
 * print out the values of the parameters in the <mod>_params structure.
 */

extern void TDRP_print_struct(TDRPtable *table, void *user_struct);

/********************
 * TDRP_print_table()
 *
 * Debugging printout of table.
 */
extern void TDRP_print_table(TDRPtable *table);

/*********************
 * TDRP_set_defaults()
 *
 * Set the parameters back to their default values.
 */

extern void TDRP_set_defaults(TDRPtable *table, void *user_struct);

/*********************
 * TDRP_check_is_set()
 * Prints out warning messages if any parameters are not set.
 * Return TRUE if all set, FALSE if not.
 */

extern int TDRP_check_is_set(TDRPtable *table, void *user_struct);

/**************************
 * TDRP_check_update_time()
 *
 * Return 1 if the parameter file has changed since the last time
 * the parameter structure was changed.
 * Return 0 if not changed.
 * Return -1 if error on opening file.
 */

extern int TDRP_check_update_time(char *in_file, void *user_struct);

#ifdef __cplusplus
}
#endif

#endif /* TDRP_OBSOLETE_WAS_INCLUDED */

