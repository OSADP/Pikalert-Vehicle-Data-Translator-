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

setenv NETCDF_LIB_DIR /usr/local/netcdf/lib
setenv HDF5_LIB_DIR    /usr/local/hdf5/lib
setenv ZLIB_LIB_DIR   /usr/local/zlib/lib
#
# Local destinations
#
setenv LOCAL_INC_DIR $HOME/include
setenv LOCAL_LIB_DIR $HOME/lib
setenv LOCAL_BIN_DIR $HOME/bin
setenv LOCAL_PYTHON_SCRIPTS_DIR $HOME/scripts/python
#
# Compile flags
#
setenv CCFLAGS "-g"
setenv RAL_CCFLAGS "-g"
#
# For accessing shared libraries at run-time
#
setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${LOCAL_LIB_DIR}:${NETCDF_LIB_DIR}:${HDF5_LIB_DIR}:${ZLIB_LIB_DIR}"
#
#
# Location where RCTM test output will be written
#
setenv RCTM_ROOT_DIR /usr/local/data/rctm
