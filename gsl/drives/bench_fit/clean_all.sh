GSL_PATH="/home/aaa/SMSE/gsl"
GSL_SPECFUNC_PATH=${GSL_PATH}"/specfunc/.libs"

# get all dictionary 
all_dicts=`ls -d */`

rm -f nohup*.txt

for dict in $all_dicts:
do
  work_dic=`echo ${dict%%:*}`
  
  # enter working dictionary
  cd ${work_dic}
  echo "Clean ==== > "${PWD}

  # remove pre running info
  rm -rf *_output *.bc klee-* *.profraw *.gcno *.gcov *.gcda

  rm -f ${GSL_SPECFUNC_PATH}/*.gcda ${GSL_SPECFUNC_PATH}/*.gcov
     
  # delete all exeutable
  executable_list=`ls -F`
  for executable in ${executable_list}:
  do 
    if [[ ${executable} != *".c"* ]];then
      rm -rf ${executable}
    fi
  done

  # exit working dictionary
  cd ..
done
