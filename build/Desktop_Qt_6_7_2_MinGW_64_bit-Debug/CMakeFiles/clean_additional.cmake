# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\LeagueScreenSaver_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\LeagueScreenSaver_autogen.dir\\ParseCache.txt"
  "LeagueScreenSaver_autogen"
  )
endif()
