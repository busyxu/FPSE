file=$1
rm -f -R ${file:0:$size-2}"_gsl.bc"
rm -f -R ${file:0:$size-2}".bc"
clang -I ../ -I ../../klee-cache-rate/klee-float/klee/include -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone ${file}
llvm-link ../libgsl.bc ${file:0:$size-2}".bc" -o ${file:0:$size-2}"_gsl.bc"
cp ${file:0:$size-2}"_gsl.bc" ../../klee-cache-rate/klee-float/klee/gsl/
