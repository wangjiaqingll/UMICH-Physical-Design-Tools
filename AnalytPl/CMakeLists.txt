# AnalytPl CMAKE Compile Settings
project(AnalytPl
  LANGUAGES CXX 
)

cmake_minimum_required(VERSION 2.6)

set(ANALYTPL_HOME ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "AnalytPl_home: ${ANALYTPL_HOME}")

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

############################################################
# Create a library for ANALYTPL
############################################################
set( ANALYTPL_SRC
  src/analytPl.cxx
  src/AnalytPlTest0.cxx
)

set (ABKCOMMON_HEADERS
  src/analytPl.h
)

add_library( AnalytPl STATIC ${ANALYTPL_SRC} )

set_target_properties( AnalytPl
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${ANALYTPL_HOME}/lib
  LIBRARY_OUTPUT_DIRECTORY ${ANALYTPL_HOME}/lib
  RUNTIME_OUTPUT_DIRECTORY ${ANALYTPL_HOME}/lib
)

# include directory settings
target_include_directories( AnalytPl
  PUBLIC
  ${ANALYTPL_HOME}
  src/
  )

add_executable( analytpl
  ${ANALYTPL_SRC}
)

# add_dependencies( cABKCommonzlib cABKCommon )

######################################################
# install

install(FILES ${ANALYTPL_HEADERS} DESTINATION include)
install(TARGETS AnalytPl DESTINATION lib)

