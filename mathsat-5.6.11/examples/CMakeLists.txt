cmake_minimum_required(VERSION 3.17)
project(examples)

set(CMAKE_CXX_STANDARD 14)

include_directories(/home/aaa/mathsat-5.6.11/include)

#link_directories(/usr/local/lib)
set(MathSAT5_LIBRARIES /home/aaa/mathsat-5.6.11/lib/libmathsat.so)

add_executable(examples test.cpp)

#add_executable(examples api_example_yx.c)

target_link_libraries(examples PUBLIC
        ${MathSAT5_LIBRARIES}
  )

#add_executable(examples
#        extsat/main.cpp
#        extsat/mathsat.h
#        extsat/mathsataig.h
#        extsat/mathsatll.h
#        extsat/msatcleaneling.cpp
#        extsat/msatcleaneling.h
#        extsat/msatexistelim.h
#        extsat/msatminisat.cpp
#        extsat/msatminisat.h
#        api_example.c
#        api_example_allsat.c
#        api_example_lowlevel.cpp
#        api_example_named_list.c
#        api_example_proofs.c
#        api_external_unsat_core_extractor.c)
