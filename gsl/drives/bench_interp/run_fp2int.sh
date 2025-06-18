# Basic tools path
LIBM_PATH="/home/aaa/klee-uclibc/lib/libm.a"
GSL_PATH="/home/aaa/gsl"
GSL_RUNTIME_PATH="/home/aaa/gsl_runtime_lib"
KLEE_PATH="/home/aaa/klee-float-solver"

## KLEE : Sometimes must be defined by user ####
KLEE_EXE_PATH=${KLEE_PATH}"/cmake-build-debug/bin/klee"
KLEE_REPLAY_PATH="/home/aaa/klee-2.3/build/bin/klee-replay"
KLEE_HEADFILE_PATH=${KLEE_PATH}"/include"

#### notice : 'gsl_config.json' must be set by user !!! ####
GSL_LIB_BC_PATH=${GSL_RUNTIME_PATH}"/libgsl.so.bc"
GSL_CONFIG_PATH=${GSL_RUNTIME_PATH}"/gsl_config.json"
GET_FUN_INFO_PY=${GSL_RUNTIME_PATH}"/get_func_info.py"
SOFTFLOAT_BC_PATH=${GSL_RUNTIME_PATH}"/softfloat.bc"

## GSL : ####
GSL_HEADFILE_PATH=${GSL_PATH}
GSL_LIBS_PATH=${GSL_PATH}"/.libs"
GSL_SPECFUNC_PATH=${GSL_PATH}"/specfunc/.libs"
GSL_CBLAS_PATH=${GSL_PATH}"/cblas/.libs"

#### SMSE running config ####
MAX_EXE_TIME=300
MAX_LOOP_TIME=3
MAX_CONCRETE=0
SOLVER_TYPE="smt"
SEARCH="bfs"

########################################################################

# get all dictionary ./rep
all_dicts=`ls -d */`

# or we can manual define which dictionary to work:
#all_dicts="gegenbauer"

# or get dictionaries from cmdline
#all_dicts=$*

# export gcov builded gsl.libs path as runtime lib
export LD_LIBRARY_PATH=${GSL_CBLAS_PATH}:${GSL_LIBS_PATH}:${GSL_RUNTIME_PATH}

for dict in $all_dicts:
do
  work_dic=`echo ${dict%%:*}`
  
  # enter working dictionary
  cd ${work_dic}
  echo "==== Enter ==== > "${PWD}
  echo ${work_dic}`find . -name "*.c" | wc -l`

  #echo ${work_dic}`find . -name "*.c" | wc -l`
  #cd ..
  #continue

  dirvers=`ls *.c`
  for dirv in ${dirvers}:
  do
    driver_name=`echo ${dirv%.*}`

    if [ ${driver_name} == "gsl_sf_bessel_y1_e" ];then
    echo "     Running ==== > "${driver_name}

    KLEE_OUT_DIR=${PWD}/${driver_name}"_output"
    TIME_LOG_TXT=${KLEE_OUT_DIR}/"execute_time.txt"
    
    # remove pre running info
    rm -rf ${KLEE_OUT_DIR} *.bc 

    # get driver.bccd 
    clang -emit-llvm -I${GSL_HEADFILE_PATH} -I${KLEE_HEADFILE_PATH} -c ${driver_name}.c
    
    # link with libgsl.bc, to generate driver_gsl.bc
    llvm-link ${driver_name}.bc ${GSL_LIB_BC_PATH} -o ${driver_name}"_gsl.bc"

    # run klee to get Ktest inputs
    start_second=$(date +%s)

    ${KLEE_EXE_PATH} -watchdog  --max-solver-time=30 \
          --search=${SEARCH} \
          --fp2int-lib=${SOFTFLOAT_BC_PATH} \
          --max-concrete-instructions=${MAX_CONCRETE} \
          -max-time=${MAX_EXE_TIME}  \
          --max-loop-time=${MAX_LOOP_TIME} \
          --solver-type=${SOLVER_TYPE}  \
          --json-config-path=${GSL_CONFIG_PATH} \
          --link-llvm-lib=${LIBM_PATH} \
          -output-dir=${KLEE_OUT_DIR} \
          ${driver_name}"_gsl.bc"

    end_second=$(date +%s)
    touch ${TIME_LOG_TXT}
    echo " "$((end_second-start_second))" " > ${TIME_LOG_TXT}

    # delete 'assembly.ll','run.istats' in klee-output, because their size are too large
    rm -f ${KLEE_OUT_DIR}/assembly.ll  ${KLEE_OUT_DIR}/run.istats

    # delete driver_gsl.bc
    rm -f ${driver_name}"_gsl.bc"
  
    fi
 
  done

  # exit working dictionary
  cd ..
done

