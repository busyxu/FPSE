###############################################################################
# Top contributors (to current version):
#   Mathias Preiner, Abdalrhman Mohamed, Mudathir Mohamed
#
# This file is part of the cvc5 project.
#
# Copyright (c) 2009-2022 by the authors listed in the file AUTHORS
# in the top-level source directory and their institutional affiliations.
# All rights reserved.  See the file COPYING in the top-level source
# directory for licensing information.
# #############################################################################
#
# The build system configuration.
##

set(CVC5_EXAMPLES_API
  bags
  bitvectors
  bitvectors_and_arrays
  combination
  datatypes
  extract
  floating_point_arith
  helloworld
  linear_arith
  quickstart.cpp
  relations
  sequences
  sets
  strings
  transcendentals
)

foreach(example ${CVC5_EXAMPLES_API})
  cvc5_add_example(${example} "" "api/cpp")
endforeach()

if(USE_COCOA)
  cvc5_add_example(finite_field "" "api/cpp")
endif()

set(SYGUS_EXAMPLES_API
  sygus-fun
  sygus-grammar
  sygus-inv
)

foreach(example ${SYGUS_EXAMPLES_API})
  cvc5_add_example(${example} "${example}.cpp utils.h utils.cpp" "api/cpp")
endforeach()

#cmake_minimum_required(VERSION 3.4)
#
#project(cvc5e)
#
#set(CMAKE_CXX_STANDARD 17)
#
#set(CVC5_LIBRARIES /home/aaa/cvc5/install/lib/libcvc5.so)
##set(CVC5_INCLUDE_DIRS /home/aaa/cvc5/install/include)
#
#link_directories(/home/aaa/cvc5/install/include)
#
##target_include_directories(cvc5e ${CVC5_INCLUDE_DIRS})
#add_executable(cvc5e linear_arith.cpp)
#
#target_link_libraries(cvc5e PUBLIC
#        ${CVC5_LIBRARIES}
#        )