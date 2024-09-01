# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\LeagueScreenshot_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\LeagueScreenshot_autogen.dir\\ParseCache.txt"
  "LeagueScreenshot_autogen"
  )
endif()
