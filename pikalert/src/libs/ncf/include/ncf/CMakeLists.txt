#/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
#/* ** Copyright UCAR (c) 1992 - 2010 */
#/* ** University Corporation for Atmospheric Research(UCAR) */
#/* ** National Center for Atmospheric Research(NCAR) */
#/* ** Research Applications Laboratory(RAL) */
#/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
#/* ** 2010/10/7 23:12:33 */
#/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
cmake_minimum_required(VERSION 2.8)

project(ncf)

INSTALL(FILES 
	ncf.hh
	gtime.h
	ncf_template.hh
        DESTINATION $ENV{GTG_INC_DIR}/ncf
)
				
