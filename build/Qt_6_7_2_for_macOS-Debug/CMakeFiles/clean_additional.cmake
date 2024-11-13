# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/rvSim_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/rvSim_autogen.dir/ParseCache.txt"
  "rvSim_autogen"
  )
endif()
