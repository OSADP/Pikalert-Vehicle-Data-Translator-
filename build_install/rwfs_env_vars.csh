#
# Environment variables needed for building code with scons
#
#===========================================================================
#
# Third-party headers, libs
#
setenv NETCDF_INC_DIR /usr/local/netcdf/include
setenv HDF_INC_DIR    /usr/local/hdf5/include
setenv ZLIB_INC_DIR   /usr/local/zlib/include
setenv UDUNITS2_INC_DIR   /usr/local/udunits2/include

setenv NETCDF_LIB_DIR /usr/local/netcdf/lib
setenv HDF5_LIB_DIR    /usr/local/hdf5/lib
setenv ZLIB_LIB_DIR   /usr/local/zlib/lib
setenv UDUNITS2_LIB_DIR   /usr/local/udunits2/lib
#
# Local destinations
#
setenv LOCAL_INC_DIR $HOME/include
setenv LOCAL_LIB_DIR $HOME/lib
setenv LOCAL_BIN_DIR $HOME/bin
setenv LOCAL_PYTHON_SCRIPTS_DIR $HOME/scripts/python
setenv LOCAL_PERL_SCRIPTS_DIR $HOME/scripts/perl
#
# Compile flags
#
setenv CCFLAGS "-g"
setenv RAL_CCFLAGS "-g"
#
# For accessing shared libraries at run-time
#
setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${LOCAL_LIB_DIR}:${NETCDF_LIB_DIR}:${HDF5_LIB_DIR}:${ZLIB_LIB_DIR}:${UDUNITS2_LIB_DIR}"
#
#
# Location where RWFS test output will be written
#
setenv RWFS_ROOT_DIR /usr/local/data/rwfs
