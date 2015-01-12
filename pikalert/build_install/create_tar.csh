#!/bin/csh

# Create vdt_topr4 distribution tar file
rm -rf cvs_dir
rm -rf vdt4
./checkout.py manifest cvs_dir
./create_dist_dir.py manifest cvs_dir vdt4
cd vdt4
find . -name CVS -exec rm -rf '{}' \;
cd ..
rm vdt_topr4.tar*
tar cvf vdt_topr4.tar vdt4
gzip vdt_topr4.tar
rm -rf cvs_dir


