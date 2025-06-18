extract-bc -b ../.libs/libgsl.a
for file in *.c
do
  if [ "$file" = "solve_cyc_tridiag.c" ];then
    wllvm -I ../linalg/ -I ../../klee-cache-rate/klee-float/klee/include -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone $file
  elif [ "$file" = "solve_cyc_tridiag_nonsym.c" ];then
    wllvm -I ../linalg/ -I ../../klee-cache-rate/klee-float/klee/include -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone $file
  elif [ "$file" = "bisect.c" ];then
    wllvm -I ../cdf/ -I ../../klee-cache-rate/klee-float/klee/include -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone $file
  elif [ "$file" = "beta_cont_frac.c" ];then
    wllvm -I ../cdf/ -I ../../klee-cache-rate/klee-float/klee/include -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone $file
  else
    wllvm -I ../ -I ../../klee-cache-rate/klee-float/klee/include -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone $file
  fi
  size=${#file}
#  echo $size
#  echo ${file:0:${size}-2}".bc"
#  echo ${file:0:${size}-2}"_gsl.bc"
  echo $file
  /opt/llvm-6/bin/llvm-link ../.libs/libgsl.a.bc ${file:0:$size-2}".bc" -o ${file:0:$size-2}"_gsl.bc"
done
