#Usage: change environmet vars to the desired path and the run
#       source pikalert_env_vars.csh 

setenv VDT_ROOT     /d2/vii
setenv VDT_DATA     $VDT_ROOT/data
setenv VDT_CODE     $VDT_ROOT/code
setenv VDT_BIN_DIR  $VDT_CODE/bin
setenv VDT_LIB_DIR  $VDT_CODE/lib
setenv VDT_INC_DIR  $VDT_CODE/include
setenv VDT_CCFLAGS  "-Wall -g -O"
setenv BUILD        $VDT_ROOT/build

setenv LOCAL_ROOT ${VDT_ROOT}
setenv LOCAL_INC_DIR ${LOCAL_ROOT}/include
setenv LOCAL_LIB_DIR ${LOCAL_ROOT}/lib
setenv LOCAL_BIN_DIR ${LOCAL_ROOT}/bin
setenv RAL_CCFLAGS "-O -std=c++0x -Wall -g -DNO_NCBYTE"

setenv RAP_SHARED_INC_DIR 
setenv RAP_SHARED_LIB_DIR 
setenv RAL_INC_DIR /d2/local/include
setenv RAL_LIB_DIR /d2/local/lib
setenv NETCDF_ROOT_DIR /usr/local/netcdf
setenv NETCDF_INC_DIR $NETCDF_ROOT_DIR/include
setenv NETCDF_LIB_DIR $NETCDF_ROOT_DIR/lib
setenv NETCDF_BIN_DIR $NETCDF_ROOT_DIR/bin
setenv HDF_ROOT_DIR /usr/local/hdf5
setenv HDF_INC_DIR $HDF_ROOT_DIR/include
setenv HDF_LIB_DIR $HDF_ROOT_DIR/lib
setenv HDF5_LIB_DIR $HDF_LIB_DIR
setenv HDF_BIN_DIR $HDF_ROOT_DIR/bin
setenv ZLIB_ROOT_DIR /usr/local/zlib
setenv ZLIB_INC_DIR $ZLIB_ROOT_DIR/include
setenv ZLIB_LIB_DIR $ZLIB_ROOT_DIR/lib
setenv ZLIB_BIN_DIR $ZLIB_ROOT_DIR/bin
setenv SZIP_INC_DIR /usr/local/szip/include
setenv SZIP_LIB_DIR /usr/local/szip/lib
setenv UDUNITS2_INC_DIR /usr/local/udunits2/include
setenv UDUNITS2_LIB_DIR /usr/local/udunits2/lib

setenv CFLAGS  -fPIC
setenv CCFLAGS $CFLAGS

#This may be ANN or ann depending on the version
setenv LIB_ANN_NAME ann
