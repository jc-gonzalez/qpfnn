#!/bin/bash

REGEXP='([A-Z]+)_([A-Z0-9]+)_([^_]+)_([0-9]+T[\.0-9]+Z)[_]*([0-9]*\.*[0-9]*)'
#REGEXP='([A-Z]+)_([A-Z0-9]+)_([^_]+)_([0-9]+T[\.0-9]+Z)(_[0-9]{2}\.[0-9]{2})*'
ASSIGN='%M=%1;%F=%2;%P=%3;%S=%2+_+%3;%D=%4;%f=%4;%v=%5'
TEMPLT='%M_%F_%P_%f_%v'

TESTFS_EXE='../../build/tests/testfs'

TESTFS="${TESTFS_EXE} -r ${REGEXP} -a ${ASSIGN} -t ${TEMPLT}"

files=""
files="$files EUC_SOC_VIS-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_SOC_NIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_HK_W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_SIM_VIS-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_SIM_NIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_SIM_SIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_LE1_VIS-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_LE1_NIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_LE1_SIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_QLA_SIM-VIS-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_QLA_SIM-NIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_QLA_SIM-SIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_QLA_LE1-VIS-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_QLA_LE1-NIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_QLA_LE1-SIR-W-21034-1_20191212T101212.0Z_03.04.fits"
files="$files EUC_VIS_D-12000-4_20211212T101212.0Z_04.03.fits"
#files="$files EUC_VIS_D-CALIB-12000-1_20211212T101212.0Z_04.03.fits"
#files="$files EUC_VIS_C-CALIB-12000-1_20211212T101212.0Z_04.03.fits"
#files="$files EUC_VIS_D-STACK-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_VIS_C-STACK-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_VIS_D-PSF-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_VIS_D-CAT-12000_20211212T101212.0Z_04.03.fits"
files="$files EUC_NIR_D-12000-2-J_20211212T101212.0Z_04.03.fits"
files="$files EUC_NIR_C-12000-4-H_20211212T101212.0Z_04.03.fits"
#files="$files EUC_NIR_D-CALIB-12000-2-J_20211212T101212.0Z_04.03.fits"
#files="$files EUC_NIR_D-STACK-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_NIR_C-STACK-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_NIR_D-PSF-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_NIR_C-PSF-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_NIR_D-CAT-12000_20211212T101212.0Z_04.03.fits"
files="$files EUC_SIR_D-12000-2_20211212T101212.0Z_04.03.fits"
#files="$files EUC_SIR_D-CALIB-12000-2_20211212T101212.0Z_04.03.fits"
#files="$files EUC_SIR_D-SPE1D-12000-2_20211212T101212.0Z_04.03.fits"
#files="$files EUC_SIR_D-SPE2D-12000-2_20211212T101212.0Z_04.03.fits"
#files="$files EUC_SIR_D-SPEX-12000-2_20211212T101212.0Z_04.03.fits"
#files="$files EUC_MER_CAT-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_MER_CAT-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_PHZ_CAT-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_SHE_CAT-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_NIR_TRANS-12000_20211212T101212.0Z_04.03.fits"
#files="$files EUC_MER_TRANS-12000_20211212T101212.0Z_04.03.fits"

# Initialize scratch files
bigfile=$(ls -s | sort -nr|head -1|cut -d" " -f2)
for file in $files; do
    ln $bigfile $file
done

# Run test
$TESTFS $files 2>&1 | tee testfs.out

# Remove files
for file in $files; do
    rm -f $file
done


