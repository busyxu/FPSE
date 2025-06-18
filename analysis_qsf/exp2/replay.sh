##!/bin/bash
#set -x

# Basic tools path
LIBM_PATH="/home/aaa/fp-solver/klee-uclibc/lib/libm.a"
#KLEE_PATH="/home/aaa/klee-2.3"
GSL_PATH="/home/aaa/fp-solver/gsl"
GSL_RUNTIME_PATH="/home/aaa/fp-solver/gsl_runtime_lib"
KLEE_PATH="/home/aaa/fp-solver/klee-float-solver"

## KLEE : Sometimes must be defined by user ####
KLEE_EXE_PATH=${KLEE_PATH}"/build/bin/klee"
KLEE_REPLAY_PATH=${KLEE_PATH}"/build/bin/klee-replay"
KLEE_HEADFILE_PATH=${KLEE_PATH}"/include"

#### notice : 'gsl_config.json' must be set by user !!! ####
GSL_LIB_BC_PATH=${GSL_RUNTIME_PATH}"/libgsl-cblas.so.bc"
GSL_CONFIG_PATH=${GSL_RUNTIME_PATH}"/gsl_config.json"
GET_FUN_INFO_PY=${GSL_RUNTIME_PATH}"/get_func_info.py"
GET_BR_INFO_PY=${GSL_RUNTIME_PATH}"/get_func_branch_info.py"

## GSL : ####
GSL_HEADFILE_PATH=${GSL_PATH}
GSL_INCLUDE_PATH=${GSL_PATH}"/gsl"
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
GSL_MULTIFIT_PATH=${GSL_PATH}"/multifit/.libs"
GSL_MULTIFITNL_PATH=${GSL_PATH}"/multifit_nlinear/.libs"
GSL_MULTIMIN_PATH=${GSL_PATH}"/multimin/.libs"
GSL_MULTIROOT_PATH=${GSL_PATH}"/multiroots/.libs"
GSL_ODEIV2_PATH=${GSL_PATH}"/ode-initval2/.libs"
GSL_POLYNOMIAL_PATH=${GSL_PATH}"/poly/.libs"
GSL_ROOT_PATH=${GSL_PATH}"/roots/.libs"
GSL_SPECFUNC_PATH=${GSL_PATH}"/specfunc/.libs"
GSL_SIMAN_PATH=${GSL_PATH}"/siman/.libs"
#GSL_SORTING_PATH=${GSL_PATH}"/sort/.libs"

########################################################################

# touch new replay result file
#result_file=${PWD}/res_all_blas.txt
#cover_trend_file=${PWD}/cov_trend_blas.txt
#result_file=${PWD}/res_all_cdf.txt
#cover_trend_file=${PWD}/cov_trend_cdf.txt
#result_file=${PWD}/res_all_compAndopt.txt
#cover_trend_file=${PWD}/cov_trend_compAndopt.txt
#result_file=${PWD}/res_all_complex.txt
#cover_trend_file=${PWD}/cov_trend_complex.txt
#result_file=${PWD}/res_all_diffAndInteg.txt
#cover_trend_file=${PWD}/cov_trend_diffAndInteg.txt
#result_file=${PWD}/res_all_elementary.txt
#cover_trend_file=${PWD}/cov_trend_elementary.txt
#result_file=${PWD}/res_all_solveEqu.txt
#cover_trend_file=${PWD}/cov_trend_solveEqu.txt
result_file=${PWD}/res_all_0.txt
cover_trend_file=${PWD}/cov_trend_all_0.txt
rm -f ${result_file} ${cover_trend_file}
touch ${result_file} ${cover_trend_file}

# export gcov builded gsl.libs path as runtime lib
# export LD_LIBRARY_PATH=/home/aaa/gsl/cblas/.libs:/home/aaa/gsl/.libs:/home/aaa/gsl_runtime_lib
export LD_LIBRARY_PATH=${GSL_CBLAS_PATH}:${GSL_LIBS_PATH}:${GSL_RUNTIME_PATH}

# get all dictionary
all_dicts=($(ls -d */))

# or we can manual define which dictionary to work:
#all_dicts=("blas")
#all_dicts=("cdf")
#all_dicts=("compAndopt")
#all_dicts=("complex")
#all_dicts=("diffAndInteg")
#all_dicts=("elementary")
#all_dicts=("sf")
#all_dicts=("solveEqu")

for dict in "${all_dicts[@]}"
do
  # enter working dictionary
  cd ${dict}
  echo ${dict}"  "$(find . -name "*.c" | wc -l)
  result_dicts=($(ls -d */))
#  result_dicts=("solveEqu&gsl_root_test_interval&smt-dreal&dfs_output/")

#  result_dicts="gsl_log1p&smt&dfs_output" "gsl_log1p&smt&bfs_output"
#  for rdict in $result_dicts
  for rdict in "${result_dicts[@]}"
  do
    echo ${rdict}
  done

  # result work dictionary eg.*output/
#  for rdict in $result_dicts
#  for ((i=0; i<25; i++))
  for rdict in "${result_dicts[@]}"
  do
#    rdict=${result_dicts[i]}
    #echo ">>>>>"${rdict}
    IFS="&" read -ra arr <<< ${rdict}
    length=${#arr[@]}
    #echo "-------------------------:>>"${length}
    if [ $length -lt 2 ];then
      #echo "this is fuzz/ dictionary"
      continue
    fi
    # get absolute driver.c path
    #driver_name=`echo ${dirv%.*}`
    # gsl_ldexp%dreal-is%bfs_output
    driver_name=${arr[1]}

    #if [ ${driver_name} = "gsl_sf_bessel_jl_e" ];then
    #echo "     Running ==== > "${driver_name}
    echo "\n     Running ==== > "${rdict}

    #KLEE_OUT_DIR=${PWD}/${driver_name}"_output"
    #fileName=${driver_name}"_"$3"_"$4"_output"
    KLEE_OUT_DIR=${PWD}/${rdict}
    TIME_LOG_TXT=${KLEE_OUT_DIR}"execute_time.txt"

    # remove pre run info
    rm -f *.gcda *.gcov ${driver_name}

    echo "====  Replay Ktest ===="
    KTEST_LIST=${KLEE_OUT_DIR}"*.ktest"
    KTEST_CHECK_EXIST=${KLEE_OUT_DIR}"test000001.ktest"
    echo "===>"${KTEST_CHECK_EXIST}
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
    #      -o gsl_deriv_backward gsl_deriv_backward.c -lm


    if [ -f ${driver_name} ]; then
      echo "execution driver_name is exit"
    else
      /home/aaa/fp-solver/llvm-6/install/bin/clang -fprofile-arcs -ftest-coverage \
          -I${KLEE_HEADFILE_PATH}  -I${GSL_HEADFILE_PATH} \
          -L${GSL_RUNTIME_PATH}     -lgcov_preload \
          -L${GSL_CBLAS_PATH}       -lgslcblas \
          -L${GSL_LIBS_PATH}        -lgsl \
          -L${GSL_RUNTIME_PATH}     -lkleeRuntest \
          -o ${driver_name} ${driver_name}.c -lm
    fi

    # get driver function belong to which file
    python_res=$(python3 ${GET_FUN_INFO_PY} ${driver_name} getFile "")

    echo "===>python_res: "${python_res}

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
    gcov_file_multifitnl=${GSL_MULTIFITNL_PATH}/${python_res}
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
        rm -f ${GSL_BLAS_PATH}/*.gcda ${GSL_BLAS_PATH}/*.gcov
        ;;
      gsl_cdf_*)
        gcov_file=${gcov_file_cdf}
        rm -f ${GSL_CDF_PATH}/*.gcda ${GSL_CDF_PATH}/*.gcov
        ;;
      gsl_complex_*)
        gcov_file=${gcov_file_complexnum}
        rm -f ${GSL_COMPLEXNUM_PATH}/*.gcda ${GSL_COMPLEXNUM_PATH}/*.gcov
        ;;
      gsl_deriv_*)
        gcov_file=${gcov_file_deriv}
        rm -f ${GSL_DERIV_PATH}/*.gcda ${GSL_DERIV_PATH}/*.gcov
        ;;
      gsl_fit_*)
        gcov_file=${gcov_file_fit}
        rm -f ${GSL_FIT_PATH}/*.gcda ${GSL_FIT_PATH}/*.gcov
        ;;
      gsl_min_*)
        gcov_file=${gcov_file_min}
        rm -f ${GSL_MIN_pATH}/*.gcda ${GSL_MIN_pATH}/*.gcov
        ;;
      gsl_integration_*)
        gcov_file=${gcov_file_integration}
        rm -f ${GSL_INTEGRATION_PATH}/*.gcda ${GSL_INTEGRATION_PATH}/*.gcov
        ;;
      gsl_interp*)
        gcov_file=${gcov_file_interpolation}
        rm -f ${GSL_INTERPOLATION_PATH}/*.gcda ${GSL_INTERPOLATION_PATH}/*.gcov
        ;;
      gsl_spline*)
        gcov_file=${gcov_file_interpolation}
        rm -f ${GSL_INTERPOLATION_PATH}/*.gcda ${GSL_INTERPOLATION_PATH}/*.gcov
        ;;
      gsl_monte_*)
        gcov_file=${gcov_file_monte}
        rm -f ${GSL_MONTE_PATH}/*.gcda ${GSL_MONTE_PATH}/*.gcov
        ;;
      gsl_multifit_nlinear*)
        gcov_file=${gcov_file_multifitnl}
        rm -f ${GSL_MULTIFITNL_PATH}/*.gcda ${GSL_MULTIFITNL_PATH}/*.gcov
        ;;
      gsl_multifit_*)
        gcov_file=${gcov_file_multifit}
        rm -f ${GSL_MULTIFIT_PATH}/*.gcda ${GSL_MULTIFIT_PATH}/*.gcov
        ;;
      gsl_multimin_*)
        gcov_file=${gcov_file_multimin}
        rm -f ${GSL_MULTIMIN_PATH}/*.gcda ${GSL_MULTIMIN_PATH}/*.gcov
        ;;
      gsl_multiroot_*)
        gcov_file=${gcov_file_multiroot}
        rm -f ${GSL_MULTIROOT_PATH}/*.gcda ${GSL_MULTIROOT_PATH}/*.gcov
        ;;
      gsl_odeiv2_*)
        gcov_file=${gcov_file_odeiv2}
        rm -f ${GSL_ODEIV2_PATH}/*.gcda ${GSL_ODEIV2_PATH}/*.gcov
        ;;
      gsl_poly_*)
        gcov_file=${gcov_file_polynomial}
        rm -f ${GSL_POLYNOMIAL_PATH}/*.gcda ${GSL_POLYNOMIAL_PATH}/*.gcov
        ;;
      gsl_root_*)
        gcov_file=${gcov_file_root}
        rm -f ${GSL_ROOT_PATH}/*.gcda ${GSL_ROOT_PATH}/*.gcov
        ;;
      gsl_sf_*)
        gcov_file=${gcov_file_specfunc}
        rm -f ${GSL_SPECFUNC_PATH}/*.gcda ${GSL_SPECFUNC_PATH}/*.gcov
        ;;
      gsl_siman_*)
        gcov_file=${gcov_file_siman}
        rm -f ${GSL_SIMAN_PATH}/*.gcda ${GSL_SIMAN_PATH}/*.gcov
        ;;
      gsl_*)
        gcov_file=${gcov_file_mathfunc}
        rm -f ${GSL_MATHFUNC_PATH}/*.gcda ${GSL_MATHFUNC_PATH}/*.gcov
        ;;
    esac

#    echo "************"${gcov_file}
    length=${#gcov_file}
    echo "===>gcno: "${gcov_file:0:length-2}".gcno"
    if [ -f ${gcov_file:0:length-2}".gcno" ]; then
      echo "gcno file is exit"
      gcov_file_path=${gcov_file}
      #break
    else
      echo "gcno file is no exit"
    fi

#    echo "===>"${gcov_file_path}

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
      timeout 5 ${KLEE_REPLAY_PATH} ./${driver_name} ${KTEST_CHECK_EXIST}
      #exe_command=${KLEE_REPLAY_PATH}" ./"${driver_name}" "${KTEST_CHECK_EXIST}
      #timeout 5 ${exe_command}
      if [ $? -eq 124 ]; then
        echo "Command execution took too long."
        continue
      fi
#      echo "=====================>1 point"
      # get ktest name
      ktest_time_log=`echo ${ktest%.*}`".time"
      echo "===>ktest_time_log: "${ktest_time_log}
      # get ktest generate time
      read -d "_" generate_time < ${ktest_time_log}
#      echo "=====================>2 point"
      # get .gcov report
      #llvm-cov gcov ${gcov_file_path}
#      tempout=${rdict%/}"_"temp.out
#      touch ${tempout}
#      touch temp.out
#      echo "=================>3 point:"${gcov_file_path}
      gcov_res=$(/home/aaa/fp-solver/llvm-6/install/bin/llvm-cov gcov ${gcov_file_path})
      echo "======= "${gcov_res} > temp.out
      # get target function coverage in gsl  .lib
#      cover_res=$(python3 ${GET_FUN_INFO_PY} ${driver_name} ${PWD}/${obj_gcov} ${PWD}/"temp.out")
      cover_line_res=$(python3 ${GET_FUN_INFO_PY} ${driver_name} ${gcov_res_path} ${PWD}/temp.out)
      echo "===>cover line res:"${cover_line_res}
      rm temp.out

      # get the branches taken info
      /home/aaa/fp-solver/llvm-6/install/bin/llvm-cov gcov ${gcov_file_path} -b > temp.out
      cover_branch_res=$(python3 ${GET_BR_INFO_PY} ${driver_name} ${gcov_res_path} ${PWD}/temp.out)
      echo "===>cover branch res:"${cover_branch_res}
      rm temp.out

      now_trend_info=${generate_time}", "${cover_line_res}", "${cover_branch_res}
      echo ${now_trend_info} >> ${cover_trend_file}
    done

    # output end flag into coverage trendency file
    echo "=== End" >> ${cover_trend_file}

    #llvm-cov gcov ${gcov_file_path}

    # get final.gcov report
#    tempout=${rdict%/}"_"temp.out
#    touch ${tempout}
    # creating .gcov
    gcov_res=$(/home/aaa/fp-solver/llvm-6/install/bin/llvm-cov gcov ${gcov_file_path})
    echo "======= "${gcov_res} > temp.out
    # get target function coverage in gsl.lib
    fin_cover_line_res=$(python3 ${GET_FUN_INFO_PY} ${driver_name} ${gcov_res_path} ${PWD}/temp.out)
    echo "===>final cover line res:"${cover_line_res}
    rm temp.out

    # get total branch info
    /home/aaa/fp-solver/llvm-6/install/bin/llvm-cov gcov ${gcov_file_path} -b > temp.out
    fin_cover_branch_res=$(python3 ${GET_BR_INFO_PY} ${driver_name} ${gcov_res_path} ${PWD}/temp.out)
    echo "===>final cover branch res:"${fin_cover_branch_res}
    rm temp.out

    # get execution time
    read -d "_" execute_time < ${TIME_LOG_TXT}

    # output driver function coverage and excution time into txt file
    dirv_result=${rdict}" , "${fin_cover_line_res}", "${fin_cover_branch_res}", "${execute_time}
    echo "===>writing:"${dirv_result}
    echo ${dirv_result} >> ${result_file}

    #fi
  done

  # exit working dictionary
  cd ..
done

#set +x