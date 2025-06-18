# QSF
QSF is a satisfiability solver based on multi-objective optimization.

## Dependencies

This project depends on:

- [Z3] library for input file parsing and model validation, i.e., Z3 v3.6.2.
- [LLVM] for JIT execution of SMT formulas, i.e., LLVM v6.0.0.
- [NLopt2] for solving multi-objective optimization problems, MOCEA is developed based on NLopt.

## Building 

You can build the project using a command like, (Note: The paths of z3, llvm and NLopt2 need to be replaced with the paths in your environment)

```bash
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -Dlibz3-path=/path_libz3/libz3.so \
      -DZ3_INCLUDE_DIR=/path_z3/install/include \
      -Dlibnlopt-path=/path_nlopt2/build/libnlopt2.so \
      -DNLOPT_INCLUDE_DIR=/path_nlopt2/install/include \
      -DLLVM_DIR=/path_llvm/install/lib/cmake/llvm \
      ..
make
```

## Usage
After the build is successful, the executable program `qsf` is in `build/bin`. You can view the options through `./qsf -help`.

A simple example of solving smtlib format file:

```
./qsf -smtlib-output -f formula.smt2
```

## Model validation

In the case of `sat` result, it is possible to intruct `QSF` to externally validate the 
generated model using `z3`. This can be done by providing parameter `-c`. For example,

```bash
./qsf -c -f formula.smt2
```

So far, we have not encountered any unsound result. Please report to us if you 
find any such cases.


  [Z3]: <https://github.com/Z3Prover/z3>
  [LLVM]: <http://llvm.org/>
  [NLopt2]: <https://github.com/busyxu/nlopt-mocea>
