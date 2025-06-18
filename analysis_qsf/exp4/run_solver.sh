# Basic tools path
LIBM_PATH="/home/aaa/fp-solver/klee-uclibc/lib/libm.a"
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
SOFTFLOAT_BC_PATH=${GSL_RUNTIME_PATH}"/softfloat.bc"

## GSL : ####
GSL_HEADFILE_PATH=${GSL_PATH}
GSL_LIBS_PATH=${GSL_PATH}"/.libs"
GSL_SPECFUNC_PATH=${GSL_PATH}"/specfunc/.libs"
GSL_CBLAS_PATH=${GSL_PATH}"/cblas/.libs"

#echo ${GSL_CBLAS_PATH}
#echo ${GSL_LIBS_PATH}
#echo ${GSL_RUNTIME_PATH}

#### SMSE running config ####
MAX_EXE_TIME=3600
MAX_LOOP_TIME=3
MAX_CONCRETE=0
SOLVER_TYPE=$3
SEARCH=$4

########################################################################

# get all dictionary ./rep
#all_dicts=`ls -d */`

# or we can manual define which dictionary to work:
#all_dicts="gegenbauer"

# or get dictionaries from cmdline
#all_dicts=$*

# export gcov builded gsl.libs path as runtime lib
export LD_LIBRARY_PATH=${GSL_CBLAS_PATH}:${GSL_LIBS_PATH}:${GSL_RUNTIME_PATH}

work_dic=$1  # 传入的参数1
dirv=$2      # 传入的参赛2

# enter working dictionary
cd ${work_dic}

driver_name=`echo ${dirv%.*}`
echo "     Running ==== > "${driver_name}", "$3", "$4

KLEE_OUT_DIR=${PWD}/$1"&"${driver_name}"&"$3"&"$4"_output"
TIME_LOG_TXT=${KLEE_OUT_DIR}/"execute_time.txt"
    
# remove pre running info
rm -rf ${KLEE_OUT_DIR} 

# get driver.bccd
#/home/aaa/llvm-6/install/bin/clang -emit-llvm -I/home/aaa/gsl -I/home/aaa/klee-float-solver_1/include -c gsl_sf_clausen_e.c
#clang -emit-llvm -I${GSL_HEADFILE_PATH} -I${KLEE_HEADFILE_PATH} -c ${driver_name}.c
/home/aaa/fp-solver/llvm-6/install/bin/clang -emit-llvm -I${GSL_HEADFILE_PATH} -I${KLEE_HEADFILE_PATH} -c ${driver_name}.c

# link with libgsl.bc, to generate driver_gsl.bc
#/home/aaa/llvm-6/install/bin/llvm-link ${driver_name}.bc ${GSL_LIB_BC_PATH} -o ${driver_name}"_gsl.bc"
gslbc=${driver_name}"_gsl.bc"
if [ -e "$gslbc" ]; then
    echo "$gslbc exist."
else
    #echo "文件 $filename 不存在."
#    /home/aaa/llvm-6/install/bin/llvm-link "gsl_sf_clausen_e.bc" "/home/aaa/gsl_runtime_lib/libgsl-cblas.so.bc" -o "gsl_sf_clausen_e_gsl.bc"
#    llvm-link ${driver_name}.bc ${GSL_LIB_BC_PATH} -o ${driver_name}"_gsl.bc"
    /home/aaa/fp-solver/llvm-6/install/bin/llvm-link ${driver_name}.bc ${GSL_LIB_BC_PATH} -o ${driver_name}"_gsl.bc"
fi
# run klee to get Ktest inputs
start_second=$(date +%s)
#runlogout=${driver_name}"&"$3"&"$4".runlog"
#rm ${runlogout}

if [ ${SOLVER_TYPE} = "fp2int" ]; then
     ${KLEE_EXE_PATH} -watchdog  --max-solver-time=60 \
          --search=${SEARCH} \
          --fp2int-lib=${SOFTFLOAT_BC_PATH} \
          --max-concrete-instructions=${MAX_CONCRETE} \
          -max-time=${MAX_EXE_TIME}  \
          --max-loop-time=${MAX_LOOP_TIME} \
          --solver-type="smt"  \
          --json-config-path=${GSL_CONFIG_PATH} \
          --link-llvm-lib=${LIBM_PATH} \
          -output-dir=${KLEE_OUT_DIR} \
          ${driver_name}"_gsl.bc" > ${driver_name}"&"$3"&"$4".runlog" 2>&1
elif [ ${SOLVER_TYPE} = "z3" ] || [ ${SOLVER_TYPE} = "bitwuzla" ] || [ ${SOLVER_TYPE} = "mathsat5" ] || [ ${SOLVER_TYPE} = "cvc5" ]; then
    ${KLEE_EXE_PATH} -watchdog  --max-solver-time=60 \
          --search=${SEARCH} \
          --max-concrete-instructions=${MAX_CONCRETE} \
          -max-time=${MAX_EXE_TIME}  \
          --max-loop-time=${MAX_LOOP_TIME} \
          --solver-type=${SOLVER_TYPE}  \
          --json-config-path=${GSL_CONFIG_PATH} \
          --link-llvm-lib=${LIBM_PATH} \
          -output-dir=${KLEE_OUT_DIR} \
          ${driver_name}"_gsl.bc" > ${driver_name}"&"$3"&"$4".runlog" 2>&1
else
    ${KLEE_EXE_PATH} -watchdog  --max-solver-time=60 \
          --search=${SEARCH} \
          --max-concrete-instructions=${MAX_CONCRETE} \
          -max-time=${MAX_EXE_TIME}  \
          --max-loop-time=${MAX_LOOP_TIME} \
          --solver-type=${SOLVER_TYPE}  \
          --json-config-path=${GSL_CONFIG_PATH} \
          -output-dir=${KLEE_OUT_DIR} \
          ${driver_name}"_gsl.bc" > ${driver_name}"&"$3"&"$4".runlog" 2>&1
fi

end_second=$(date +%s)
touch ${TIME_LOG_TXT}
echo " "$((end_second-start_second))" " > ${TIME_LOG_TXT}

# delete 'assembly.ll','run.istats' in klee-output, because their size are too large
rm -f ${KLEE_OUT_DIR}/assembly.ll  ${KLEE_OUT_DIR}/run.istats

# delete driver_gsl.bc
# rm -f ${driver_name}"_gsl.bc"

#prefix="${driver_name}"  # 指定要计数的前缀
#count=$(find . -maxdepth 1 -type d -name "${prefix}*" | grep -c '^')
#if [ $count -ge 16 ]; then
#    rm ${driver_name}"_gsl.bc"
#    echo "已删除"${driver_name}"_gsl.bc""文件."
#else
#    echo "文件夹计数不足 16，不执行删除操作."

#fi
  
# exit working dictionary

cd ..

