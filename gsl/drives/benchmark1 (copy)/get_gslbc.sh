# Basic tools path
LIBM_PATH="/home/aaa/klee-uclibc/lib/libm.a"
GSL_PATH="/home/aaa/gsl"
GSL_RUNTIME_PATH="/home/aaa/gsl_runtime_lib"
KLEE_PATH="/home/aaa/klee-float-solver"

## KLEE : Sometimes must be defined by user ####
KLEE_EXE_PATH=${KLEE_PATH}"/build/bin/klee"
KLEE_REPLAY_PATH="/home/aaa/klee-2.3/build/bin/klee-replay"
KLEE_HEADFILE_PATH=${KLEE_PATH}"/include"

#### notice : 'gsl_config.json' must be set by user !!! ####
GSL_LIB_BC_PATH=${GSL_RUNTIME_PATH}"/libgsl-cblas.so.bc"
GSL_CONFIG_PATH=${GSL_RUNTIME_PATH}"/gsl_config.json"
GET_FUN_INFO_PY=${GSL_RUNTIME_PATH}"/get_func_info.py"

## GSL : ####
GSL_HEADFILE_PATH=${GSL_PATH}
GSL_LIBS_PATH=${GSL_PATH}"/.libs"
GSL_SPECFUNC_PATH=${GSL_PATH}"/specfunc/.libs"
GSL_CBLAS_PATH=${GSL_PATH}"/cblas/.libs"

echo ${GSL_CBLAS_PATH}
echo ${GSL_LIBS_PATH}
echo ${GSL_RUNTIME_PATH}

#### SMSE running config ####
MAX_EXE_TIME=400
MAX_LOOP_TIME=3
MAX_CONCRETE=0
SOLVER_TYPE="gosat"
SEARCH="bfs"

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
echo "     Running-compile ==== > "${driver_name}

KLEE_OUT_DIR=${PWD}/${driver_name}"_output"
TIME_LOG_TXT=${KLEE_OUT_DIR}/"execute_time.txt"

# get driver.bccd 
clang -emit-llvm -I${GSL_HEADFILE_PATH} -I${KLEE_HEADFILE_PATH} -c ${driver_name}.c
    
# link with libgsl.bc, to generate driver_gsl.bc
llvm-link ${driver_name}.bc ${GSL_LIB_BC_PATH} -o ${driver_name}"_gsl.bc"

cd ..

