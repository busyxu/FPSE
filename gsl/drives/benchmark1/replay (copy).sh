# Basic tools path
LIBM_PATH="/home/aaa/klee-uclibc/lib/libm.a"
KLEE_PATH="/home/aaa/klee-2.3"
GSL_PATH="/home/aaa/gsl"
GSL_RUNTIME_PATH="/home/aaa/gsl_runtime_lib"

## KLEE : Sometimes must be defined by user ####
KLEE_EXE_PATH=${KLEE_PATH}"/build/bin/klee"
KLEE_REPLAY_PATH=${KLEE_PATH}"/build/bin/klee-replay"
KLEE_HEADFILE_PATH=${KLEE_PATH}"/include"

#### notice : 'gsl_config.json' must be set by user !!! ####
GSL_LIB_BC_PATH=${GSL_RUNTIME_PATH}"/libgsl-cblas.so.bc"
GSL_CONFIG_PATH=${GSL_RUNTIME_PATH}"/gsl_config.json"
GET_FUN_INFO_PY=${GSL_RUNTIME_PATH}"/get_func_info.py"

## GSL : ####
GSL_HEADFILE_PATH=${GSL_PATH}
GSL_LIBS_PATH=${GSL_PATH}"/.libs"
GSL_CBLAS_PATH=${GSL_PATH}"/cblas/.libs"

GSL_BLAS_PATH=${GSL_PATH}"/blas/.libs"
GSL_CDF_PATH=${GSL_PATH}"/cdf/.libs"
GSL_COMPLEXNUM_PATH=${GSL_PATH}"/complex/.libs"
GSL_DERIV_PATH=${GSL_PATH}"/deriv/.libs"
GSL_MATHFUNC_PATH=${GSL_PATH}"/sys/.libs"
GSL_FIT_PATH=${GSL_PATH}"/fit/.libs"
GSL_MIN_pATH=${GSL_PATH}"/min/.libs"
GSL_INTEGRATION_PATH=${GSL_PATH}"/integration/.libs"
GSL_INTERPOLATION_PATH=${GSL_PATH}"/interpolation/.libs"
GSL_MONTE_PATH=${GSL_PATH}"/monte/.libs"
GSL_MULTIFIT_PATH=${GSL_PATH}"/multifit_nlinear/.libs"
GSL_MULTIMIN_PATH=${GSL_PATH}"/multimin/.libs"
GSL_MULTIROOT_PATH=${GSL_PATH}"/multiroots/.libs"
GSL_ODEIV2_PATH=${GSL_PATH}"/ode-initval/.libs"
GSL_POLYNOMIAL_PATH=${GSL_PATH}"/poly/.libs"
GSL_ROOT_PATH=${GSL_PATH}"/roots/.libs"
GSL_SPECFUNC_PATH=${GSL_PATH}"/specfunc/.libs"
GSL_SIMAN_PATH=${GSL_PATH}"/siman/.libs"
#GSL_SORTING_PATH=${GSL_PATH}"/sort/.libs"


########################################################################

# get all dictionary 
#all_dicts=`ls -d */`

# or we can manual define which dictionary to work:
all_dicts="elementary"

# touch new replay result file
result_file=${PWD}/res_all.txt
cover_trend_file=${PWD}/cov_trend.txt
rm -f ${result_file} ${cover_trend_file}
touch ${result_file} ${cover_trend_file}

# export gcov builded gsl.libs path as runtime lib
export LD_LIBRARY_PATH=${GSL_CBLAS_PATH}:${GSL_LIBS_PATH}:${GSL_RUNTIME_PATH}

for dict in $all_dicts:
do
  work_dic=`echo ${dict%%:*}`
  
  # enter working dictionary
  cd ${work_dic}

  echo ${work_dic}"  "`find . -name "*.c" | wc -l`

  result_dicts=`ls -d */`
  
  #result_dicts="gsl_log1p&smt&dfs_output" "gsl_log1p&smt&bfs_output"
  echo ${result_dicts}
  #cp *.c ..
  #cd ..
  #continue

  #dirvers=`ls *.c`
  #for dirv in ${dirvers}:
  for rdict in $result_dicts:
  do
    
    #echo ">>>>>"${rdict}
    IFS="&" read -ra arr <<< ${rdict}
    length=${#arr[@]}
    #echo "-------------------------:>>"${length}
    if [ $length -lt 2 ];then
      #echo "goodgoodgood=="
      continue
    fi
    # get absolute driver.c path
    #driver_name=`echo ${dirv%.*}`
    # gsl_ldexp%dreal-is%bfs_output
    driver_name=${arr[0]}

    #if [ ${driver_name} = "gsl_sf_bessel_jl_e" ];then
    #echo "     Running ==== > "${driver_name}
    echo "     Running ==== > "${rdict}

    #KLEE_OUT_DIR=${PWD}/${driver_name}"_output"
    #fileName=${driver_name}"_"$3"_"$4"_output"
    KLEE_OUT_DIR=${PWD}/${rdict}
    TIME_LOG_TXT=${KLEE_OUT_DIR}"execute_time.txt"
 
    # remove pre run info
    #rm -f *.gcda *.gcov ${driver_name}
    #rm -f ${GSL_SPECFUNC_PATH}/*.gcda ${GSL_SPECFUNC_PATH}/*.gcov
   
    echo "====  Replay Ktest ===="
    KTEST_LIST=${KLEE_OUT_DIR}"*.ktest"
    KTEST_CHECK_EXIST=${KLEE_OUT_DIR}"test000001.ktest"
    echo ${KTEST_CHECK_EXIST}
    # output driver name into coverage trendency file
    echo "=== TestCase : "${rdict} >> ${cover_trend_file}

    # check there is any input Ktest generated ?
    if [ -f ${KTEST_CHECK_EXIST} ];then
      echo "CHECK:  KTests have been generated !"    
    else
      echo "CHECK:  KTest not exists, continue !"

      # output end flag into coverage trendency file
      echo "=== End" >> ${cover_trend_file}

      # get execution time 
      read -d "_" execute_time < ${TIME_LOG_TXT} 
# 将结果文件夹中的execute_time.txt中的运行时间结果写入到execute_time变量中，然后再写入result_file文件中
      # output driver function coverage into txt file 
      dirv_result=${rdict}" , 0,"${execution_time}
      echo ${dirv_result} >> ${result_file}

      continue
    fi

    
    # build gcov driver object

    #clang -fprofile-arcs -ftest-coverage \
    #      -I/home/aaa/gsl -I/home/aaa/klee-2.3/include\
    #      -L/home/aaa/gsl_runtime_lib     -lgcov_preload \
    #      -L/home/aaa/gsl/cblas/.libs       -lgslcblas \
    #      -L/home/aaa/gsl/.libs        -lgsl \
    #      -L/home/aaa/gsl_runtime_lib     -lkleeRuntest \
    #      -o gsl_deriv_backward gsl_deriv_backward.c

    clang -fprofile-arcs -ftest-coverage \
          -I${KLEE_HEADFILE_PATH}  -I${GSL_HEADFILE_PATH} \
          -L${GSL_RUNTIME_PATH}     -lgcov_preload \
          -L${GSL_CBLAS_PATH}       -lgslcblas \
          -L${GSL_LIBS_PATH}        -lgsl \
          -L${GSL_RUNTIME_PATH}     -lkleeRuntest \
          -o ${driver_name} ${driver_name}.c -lm


    # get driver function belong to which file
    python_res=$(python3 ${GET_FUN_INFO_PY} ${driver_name} getFile)

    echo "*************"${python_res}"******************"

    gcov_res_path=${PWD}/${python_res}".gcov"
    
    gcov_file_specfunc=${GSL_SPECFUNC_PATH}/${python_res}
    gcov_file_blas=${GSL_BLAS_PATH}/${python_res}
    #gcov_file_cblas=${GSL_CBLAS_PATH}/${python_res}
    gcov_file_complexnum=${GSL_COMPLEXNUM_PATH}/${python_res}
    gcov_file_mathfunc=${GSL_MATHFUNC_PATH}/${python_res}
    gcov_file_polynomial=${GSL_POLYNOMIAL_PATH}/${python_res}
    gcov_file_deriv=${GSL_DERIV_PATH}/${python_res}
    gcov_file_fit=${GSL_FIT_PATH}/${python_res}
    gcov_file_min=${GSL_MIN_pATH}/${python_res}
    gcov_file_integration=${GSL_INTEGRATION_PATH}/${python_res}
    gcov_file_interpolation=${GSL_INTERPOLATION_PATH}/${python_res}
    gcov_file_monte=${GSL_MONTE_PATH}/${python_res}
    gcov_file_multifit=${GSL_MULTIFIT_PATH}/${python_res}
    gcov_file_multimin=${GSL_MULTIMIN_PATH}/${python_res}
    gcov_file_multiroot=${GSL_MULTIROOT_PATH}/${python_res}
    gcov_file_odeiv2=${GSL_ODEIV2_PATH}/${python_res}
    gcov_file_root=${GSL_ROOT_PATH}/${python_res}
    gcov_file_siman=${GSL_SIMAN_PATH}/${python_res}
    gcov_file_cdf=${GSL_CDF_PATH}/${python_res}
    
    #gcov_file_path_list="${gcov_file_cdf} ${gcov_file_specfunc} ${gcov_file_blas} ${gcov_file_complexnum} ${gcov_file_mathfunc} ${gcov_file_polynomial} ${gcov_file_deriv} ${gcov_file_fit} ${gcov_file_min} ${gcov_file_integration} ${gcov_file_interpolation} ${gcov_file_monte} ${gcov_file_multifit} ${gcov_file_multimin} ${gcov_file_multiroot} ${gcov_file_odeiv2} ${gcov_file_root} ${gcov_file_siman}"
    
    gcov_file=${gcov_file_mathfunc}
    #echo "===>"${gcov_file}
    case ${driver_name} in
      gsl_blas_*)
        gcov_file=${gcov_file_blas}
        ;;
      gsl_cdf_*)
        gcov_file=${gcov_file_cdf}
        ;;
      gsl_complex_*)
        gcov_file=${gcov_file_complexnum}
        ;;
      gsl_deriv_*)
        gcov_file=${gcov_file_deriv}
        ;;
      gsl_fit_*)
        gcov_file=${gcov_file_fit}
        ;;
      gsl_min_*)
        gcov_file=${gcov_file_min}
        ;;
      gsl_integration_*)
        gcov_file=${gcov_file_integration}
        ;;
      gsl_interp*)
        gcov_file=${gcov_file_interpolation}
        ;;
      gsl_monte_*)
        gcov_file=${gcov_file_monte}
        ;;
      gsl_multifit_*)
        gcov_file=${gcov_file_multifit}
        ;;
      gsl_multimin_*)
        gcov_file=${gcov_file_multimin}
        ;;
      gsl_multiroot_*)
        gcov_file=${gcov_file_multiroot}
        ;;
      gsl_odeiv2_*)
        gcov_file=${gcov_file_odeiv2}
        ;;
      gsl_poly_*)
        gcov_file=${gcov_file_polynomial}
        ;;
      gsl_root_*)
        gcov_file=${gcov_file_root}
        ;;
      gsl_sf_*)
        gcov_file=${gcov_file_specfunc}
        ;;
      gsl_siman_*)
        gcov_file=${gcov_file_siman}
        ;;
      # gsl_*)
      #  gcov_file=${gcov_file_mathfunc}
      #  ;;
    esac
    
    #echo "***"${gcov_file}
    length=${#gcov_file}
    echo ${gcov_file:0:length-2}".gcno"
    if [ -f ${gcov_file:0:length-2}".gcno" ]; then
      echo "file is exit"
      gcov_file_path=${gcov_file}
      #break
    else
      echo "file is no exit"
    fi

    echo "===>"${gcov_file_path}
    
    #echo "*****************************"
    #for gcov_file in ${gcov_file_path_list}
    #do
    #    length=${#gcov_file}
    #    echo ${gcov_file:0:length-2}".gcno"
    #    if [ -f ${gcov_file:0:length-2}".gcno" ]; then
    #      echo "file is exit"
    #      gcov_file_path=${gcov_file}
    #      break
    #    else
    #      echo "file is no exit"
    #    fi
    #done
    #echo "----------------------------"
    
    #echo ${gcov_file_path}
    
    for ktest in ${KTEST_LIST}
    do
      echo "KTest : "${ktest}
      export KTEST_FILE=${ktest}
      # give a fake ktest file to activate replay tool
      ${KLEE_REPLAY_PATH} ./${driver_name} ${KTEST_CHECK_EXIST}

      # get ktest name
      ktest_time_log=`echo ${ktest%.*}`".time"
      
      # get ktest generate time 
      read -d "_" generate_time < ${ktest_time_log}

      # get .gcov report
      #llvm-cov gcov ${gcov_file_path}
      touch temp.out
      gcov_res=`llvm-cov gcov ${gcov_file_path}`
      echo "======= "${gcov_res} > temp.out

      # get target function coverage in gsl.lib
      cover_res=$(python3 ${GET_FUN_INFO_PY} ${driver_name} ${gcov_res_path})
      echo  ${cover_res}
      #rm temp.out
      
      now_trend_info=${generate_time}", "${cover_res}
      echo ${now_trend_info} >> ${cover_trend_file}
    done

    # output end flag into coverage trendency file
    echo "=== End" >> ${cover_trend_file}

    #llvm-cov gcov ${gcov_file_path}

    # get final.gcov report
    touch temp.out
    gcov_res=`llvm-cov gcov ${gcov_file_path}`
    echo "======= "${gcov_res} > temp.out

    # get target function coverage in gsl.lib
    fin_cover_res=$(python3 ${GET_FUN_INFO_PY} ${driver_name} ${gcov_res_path})
    echo  ${fin_cover_res}
    #rm temp.out

    # get execution time 
    read -d "_" execute_time < ${TIME_LOG_TXT}
      
    # output driver function coverage and excution time into txt file 
    dirv_result=${rdict}" , "${fin_cover_res}", "${execute_time}
    echo ${dirv_result} >> ${result_file}
 
    #fi
  done

  # exit working dictionary
  cd ..
done

