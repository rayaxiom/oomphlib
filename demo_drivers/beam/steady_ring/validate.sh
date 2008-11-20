#! /bin/sh

#Set the number of tests to be checked
NUM_TESTS=2

# Setup validation directory
#---------------------------
rm -rf Validation
mkdir Validation

#######################################################################

# Validation for steady buckling ring with displacement control
#--------------------------------------------------------------

cd Validation
mkdir RESLT_global
mkdir RESLT_no_global

echo "Running steady ring validation "
../steady_ring > OUTPUT_steady_ring

echo "done"
echo " " >> validation.log
echo "Steady ring validation" >> validation.log
echo "----------------------" >> validation.log
echo " " >> validation.log
echo "Validation directory: " >> validation.log
echo " " >> validation.log
echo "  " `pwd` >> validation.log
echo " " >> validation.log
cat RESLT_global/ring0.dat \
    RESLT_global/ring5.dat \
    RESLT_global/ring12.dat \
    RESLT_global/ring20.dat \
    RESLT_global/trace.dat\
    > ring_results.dat

cat RESLT_no_global/ring0.dat \
    RESLT_no_global/ring5.dat \
    RESLT_no_global/ring12.dat \
    RESLT_no_global/ring20.dat \
    RESLT_no_global/trace.dat\
    > ring_results2.dat
if test "$1" = "no_python"; then
  echo "dummy [OK] -- Can't run fpdiff.py because we don't have python" >> validation.log
else
  ../../../../bin/fpdiff.py ../validata/ring_results.dat.gz \
   ring_results.dat >> validation.log
  ../../../../bin/fpdiff.py ../validata/ring_results.dat.gz \
   ring_results2.dat >> validation.log
fi


# Append output to global validation log file
#--------------------------------------------
cat validation.log >> ../../../../validation.log

cd ..


#######################################################################


#Check that we get the correct number of OKs
OK_COUNT=`grep -c 'OK' Validation/validation.log`
if  [ $OK_COUNT -eq $NUM_TESTS ]; then
 echo " "
 echo "======================================================================"
 echo " " 
 echo "All tests in" 
 echo " " 
 echo "    `pwd`    "
 echo " "
 echo "passed successfully."
 echo " "
 echo "======================================================================"
 echo " " 
else
  if [ $OK_COUNT -lt $NUM_TESTS ]; then
   echo " "
   echo "======================================================================"
   echo " " 
   echo "Only $OK_COUNT of $NUM_TESTS test(s) passed; see"
   echo " " 
   echo "    `pwd`/Validation/validation.log"
   echo " " 
   echo "for details" 
   echo " " 
   echo "======================================================================"
   echo " "
  else 
   echo " "
   echo "======================================================================"
   echo " " 
   echo "More OKs than tests! Need to update NUM_TESTS in"
   echo " " 
   echo "    `pwd`/validate.sh"
   echo " "
   echo "======================================================================"
   echo " "
  fi
fi