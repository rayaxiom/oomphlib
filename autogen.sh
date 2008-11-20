#! /bin/sh





#====================================================================
# A few helper functions
#====================================================================

# A little function 'borrowed' from the tecplot installation script...
OptionPrompt() 
{ 
 printf "%s " "$1" 
}

# Another little function 'borrowed' from the tecplot installation script...
OptionRead()
{
 read Opt
 if test "$Opt" = "" ; then
  Opt=$1
 fi
 echo $Opt
}

#This little function takes the input, removes anything following a #
#deletes blanks lines and then replaces all newlines by spaces
ProcessOptionsFile()
{
echo `cat $1 | sed 's/#.*$//' | sed '/^$/d' | tr '\012' ' '`
}


#This function returns a match (non-null string) if the input string 
#contains a long option (starting with --) after a short option 
#(no --)
CheckOptions()
{
 echo `echo -n $@ | sed 's/^/ /' | sed -n '/[ ].[^-].* --/p'`
}

#This little function echo's the usage information
EchoUsage()
{
        echo "Usage: "
        echo "------ "
        echo " "
        echo "[without flags]: Normal \"./configure; make; make install; make check\" sequence."
        echo " "
        echo " --rebuild     : Complete re-configure, followed by normal build sequence."
        echo " "
        echo "--jobs[=N]     :  Run N make jobs simultaneously."
        echo "                  Useful for speeding up the build on multi-core processors." 
        exit
}




#====================================================================
# Start Q/A session
#====================================================================

echo " "
echo "============================================================= "
echo "              oomph-lib installation script" 
echo "============================================================= "
echo " "

# Do you want to rebuild from scratch?
#-------------------------------------
#If so specify --rebuild as command line argument. Default is 
# to just do the normal ".configure, make, make install, make check" sequence.

#Bail out if more than two command line arguments
if (test $# -gt 2); then 
 EchoUsage 
fi   

#Process the command line options
raw_build=false;
make_options=" ";
while (test $# -gt 0)
do
   case "$1" in
     #Set the rebuild flag
     --rebuild) 
      echo "             [Doing complete rebuild from scratch.]"
      raw_build=true;;
     #Set the jobs flag
     --jobs*)
      make_options="$1";;
     #Anything else bail out     
      *)  
       EchoUsage;;
   esac
   shift
done

if (test "$raw_build" = "false"); then
   echo "                     [Doing normal build.]"
fi   


# Read out root install directory
#--------------------------------
MY_HOME_WD=`pwd`



# If this is a rebuild: Check for helper scripts 
#-----------------------------------------------
if $raw_build; then

  SCRIPT_LIST=`echo config.guess config.sub depcomp install-sh ltmain.sh missing aclocal.m4 mkinstalldirs `
  SCRIPTS_EXIST="no"
  for script in $SCRIPT_LIST
   do
    if (test -e $script); then
        SCRIPTS_EXIST="yes"
    fi 
  done
  if test "$SCRIPTS_EXIST" = "yes" ; then 
    echo " "
    echo "You may wipe the symbolic links to the autoconf/automake helper scripts"
    echo " "
    for script in $SCRIPT_LIST
      do
        if (test -e $script); then
          echo "   " $script
        fi 
      done
    echo " "
    echo "[This is recommended if you have moved the sources to a different"
    echo " machine without packaging them up with make dist. The symbolic "
    echo " links tend to be machine-specific so it's best to force "
    echo " autoconf/automake to rebuild them on the new machine]."
    echo " "
    echo " Do you want to wipe the helper scripts [y/n -- default: n]"
    reply=`OptionRead`
    if test "$reply" = "y" -o "$reply" = "Y" ; then 
        echo " "
        echo "As a backup: Here are the old symbolic links:"
        echo " "
        for script in $SCRIPT_LIST
          do
          if (test -L $script); then
              ls -L $script
              ls -l $script > old_symbolic_links.txt
          fi
        done
        echo " "
        echo "We have stored this information in old_symbolic_links.txt"
        echo " "
        echo "Wiping them..."
        rm -f  $SCRIPT_LIST
        echo "Done"
    fi   
else
    echo " "
    echo "[No autoconf/automake helper scripts to be wiped...]"
    echo " "
fi
fi





# Set the build directory (for lib,include), relative to root
#------------------------------------------------------------
build_sub_dir=build

# Suggested build directory
#--------------------------
build_dir=$MY_HOME_WD/$build_sub_dir

# Check build directory with user
#--------------------------------
echo " "
echo " "
echo "I'm going to install the distribution (the lib and include directories)"
echo "in:"
echo " "
echo "    " $build_dir
echo " "
echo " "
OptionPrompt " Is this OK? [y/n -- default: n]"
reply=`OptionRead`
if test "$reply" != "y" -a "$reply" != "Y" ; then 
   OptionPrompt "Specify build directory [e.g. /home/joe_user] :"
   build_dir=`OptionRead`
else
    echo "It's ok"
fi


# Summary of build info
#----------------------
echo " "
echo "============================================================= "
echo " "
echo "Build directory is: " 
echo " " 
echo "     " $build_dir
echo " " 
echo "--> The include directory will be in: "
echo " " 
echo "    " $build_dir"/include" 
echo " " 
echo "--> The lib directory will be in: "
echo " " 
echo "    " $build_dir"/lib" 
echo " "
echo "etc.       " 
echo " "
echo "============================================================= "
echo " "


# Wipe previous builds
#---------------------
if (test -d  $build_dir); then 
    echo " "
    echo "Build directory exists"
    OptionPrompt " Do you want to wipe it [y/n -- default: n]"
    reply=`OptionRead`
    if test "$reply" = "y" -o "$reply" = "Y" ; then 
       echo "Wiping it..."
       rm -f -r $build_dir
       echo "Done"
    fi
fi




# Create configure options file
#------------------------------

# Process configure options from the file config/configure_options/current
# Ignore any line that starts with "#";
# Add continuation slash at the end of each
# line. Check that all options (starting with "--") 
# come first. 

#Continue asking if the options are OK until approved
accept_configure_options=0
full_list="false"
list_changed="false"
while (test $accept_configure_options -eq 0)
do


#Read the options from the file and convert them into a single one-line string
configure_options=`ProcessOptionsFile config/configure_options/current`


#Check that the options are in the correct order
if test "`CheckOptions $configure_options`" != ""; then

  echo " "
  echo "==============================================================="
  echo "Error message from autogen.sh:"
  echo " " 
  echo "        Configure options (starting with \"--\") must be specified"
  echo "        before any flags!"
  reply="n"
  OptionRead #This is just a pause

#If the options are in the correct order, ask whether they are OK
else

  echo " "
  echo "Configure options are: "
  echo 
  echo $configure_options
  echo 
  if test $list_changed = "false"; then
   OptionPrompt "Is this OK? [y/n -- default: y]"
   reply=`OptionRead`
  else
   reply="n"
   list_changed="false"
  fi
fi


#If it's not OK, then read in alternative options from a file, or
#specify on command line
if test "$reply" = "n" -o "$reply" = "N"; then
 
  #Remove the current symbolic link (or file)
  #rm -f config/configure_options/current   

  # Link in the private ones:
  return_dir_before_link_in_private=`pwd`
  # Kill stray symlinks
  cd config/configure_options
  find . -type l -exec rm {} \; 
  if test  "$full_list" = "true"; then
   cd private_configure_options
   private_configure_option_files=`ls `
   cd ..
   for file in $private_configure_option_files; do ln -s private_configure_options/$file ; done
  fi
  cd $return_dir_before_link_in_private

  # Ooops: Non-portable gnu extension to ls
  #configure_option_files=`ls --ignore=private_configure_options config/configure_options`

  # Thanks for this fix, Andy!
  configure_option_files=`ls config/configure_options | grep -v  private_configure_options` 

  echo " "
  echo "======================================================================"
  echo 
  echo "Choose an alternative configuration file "
  #Loop over files and display a menu
  count=0
  for file in $configure_option_files
   do
    #Increase the counter
    count=`expr $count + 1`
    echo $count ": " $file
   done #End of loop over files in config/configure_options
 echo

  echo "Enter the Desired configuration file [1-"$count"]"
  if test $full_list = "false"; then
   echo "Enter -1 to show an extended list of options"
  else 
   echo "Enter -1 to show a short list of options"
  fi
  echo "Enter 0 to specify the options on the command line"
  #Read in the Desired File
  file_number=`OptionRead`

  #If options are to be read from the command line then store the#
  #options in the file config/configure_options/current
  if (test $file_number -eq 0); then
   echo 
   echo "Enter options"
   configure_options=`OptionRead`  
   echo $configure_options > config/configure_options/current

  #Otherwise copy the desired options file to config/configure_options/current
  elif (test $file_number -eq -1); then
   list_changed="true"
   if test $full_list = "true"; then
    full_list="false"
   else
    full_list="true"
   fi
  else   
   #Reset the counter
   count=0
   #Loop over the files until the counter equals the chosen file_number
   for file in $configure_option_files
     do
     #Increase the counter
     count=`expr $count + 1`
     if (test $count -eq $file_number); then
        cp -f config/configure_options/$file config/configure_options/current
        break
     fi
   done #End of loop over files
   fi #End of create symbolic link code

#If the configuration is OK, accept it
else
 echo " " 
 echo "Configure options have been accepted."
 accept_configure_options=1
fi

done #End of while loop over customisation of configure options


# Undo links to private configure options
return_dir_before_link_in_private=`pwd`
cd config/configure_options
for file in $private_configure_option_files; do rm -f $file; done
cd $return_dir_before_link_in_private


echo " " 
echo "=============================================================" 
echo " " 



# Automatic run of self tests? 
#-----------------------------
echo " " 
echo "It is possible to execute the library's extensive self-test"
echo "when the build is complete. Depending on the speed of your"
echo "computer, this may take a long time as more than 300 test "
echo "codes are run and validated."
echo " " 
echo "Do you want to build the demo codes and run the self-tests at the end "
OptionPrompt "of the build procedure? [y/n -- default: n]"
want_self_tests=`OptionRead`

if test "$want_self_tests" = "y" -o "$want_self_tests" = "Y" ; then 
  echo " "
  echo "\"make check\" will be run at the end of the build procedure"
  echo "to build/self-test the demo codes."
  echo " "
  echo "The results of the self-tests will be written to the"
  echo "file validation.log in the top-level oomph-lib directory."
  echo " "
  if (test -e validation.log); then 
    echo " " 
    OptionPrompt "The file validation.log exists. Is it OK to wipe it? [y/n -- default: n]"
    reply=`OptionRead`
    if test "$reply" != "y" -a "$reply" != "Y" ; then 
       echo " "
       echo "In that case I am disabling the automatic self-test that you requested earlier."
       want_self_tests="n"
       echo " "
       OptionPrompt "Hit enter to acknowledge"
       tmp=`OptionRead`
    else
       rm -f validation.log
       echo "I have wiped the validation.log file."
    fi
  fi
else
  echo " "
  echo "The self-test procedure will not be run automatically."
  echo "You may initiate the self-tests manually by typing \"make check\" "
  echo "in the top-level oomph-lib directory when the build process "
  echo "is complete."
  echo " "
fi



echo " "
echo "==================================================================="
echo " "
echo " "
echo "End of customisation -- the actual build process is about to start."
echo "This may take a while... No user intervention is required during"
echo "the build procedure, so go and take the dog for a walk..."
echo " " 
OptionPrompt "Hit enter to continue."
tmp=`OptionRead`





#====================================================================
# Start actual build process
#====================================================================


# Raw build: Go through all the initialisation procedures
#--------------------------------------------------------
if $raw_build; then
 $MY_HOME_WD/bin/regenerate_config_files.sh $MY_HOME_WD
fi

# Now do the actual configure:
#------------------------------
# - prefix sets directory where lib and install directories get placed:
# - CXX =  C++ compiler (defaults to gcc/g++)
# - CC =   C compiler (defaults to gcc)
# - F77 =  F77 compiler (defaults to gcc/g77)
# 
# Options: "--enable-MPI" includes all MPI sources into the build
#


# Run configure command
echo " "
echo "Running ./configure --prefix $build_dir $configure_options"
echo " " 
/bin/sh -c "./configure --prefix $build_dir $configure_options"

echo " " 
echo " " 
echo "done running ./configure"
echo " " 
echo " " 


# Make all libraries
#-------------------
echo " "
echo "Running make $make_options" 
make $make_options
echo "done"


# Install the libraries (in build directory specified above)
#-----------------------------------------------------------
echo " "
echo "running make $make_options install"
make $make_options install
echo "done" 



# Make the demo codes and run the ones that are listed in the TESTS
#------------------------------------------------------------------
# declaration in the "Makefile.am"s
#-----------------------------------
if test "$want_self_tests" = "y" -o "$reply" = "Y" ; then 
  echo " "
  echo "Running check to build/self-test the demo codes."
  echo "y" | make $make_options check
  echo "Done self test"
else
  echo " "
  echo "The build process is complete. You may now "
  echo "initiate the self-tests by typing \"make check\". "
  echo " "
fi


echo " "
echo "autogen.sh has finished! If you can't spot any error messages" 
echo "above this, oomph-lib should now be ready to use... " 
echo " " 
echo "If you encounter any problems, please study the installation" 
echo "instructions and the FAQ before contacting the developers. " 
echo " " 
