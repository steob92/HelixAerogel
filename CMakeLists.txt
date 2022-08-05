#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(HLXAERO)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})

# Define what physics to use
add_definitions(-DALL_PHYSICS)
# Define putting glass plane infront of CCD
# add_definitions(-DREAL_CCD)
# Some definitions for quick turing on/off of components
add_definitions(-DDIVERGENCE)       # Beam divergence
add_definitions(-DGAUSSIAN)         # Gaussian beam profile 
#add_definitions(-DVARENERGY)        # Variable beam energy
#add_definitions(-DDEVMODE)         # Dev mode for new features 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(HLX ${PROJECT_SOURCE_DIR}/src/HLX.cc ${sources} ${headers})
target_link_libraries(HLX ${Geant4_LIBRARIES} ROOT::Core)

add_executable(analyzeRuns ${PROJECT_SOURCE_DIR}/src/analyzeRuns.cc )
target_link_libraries(analyzeRuns ${ROOT_LIBRARIES} ROOT::Core)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build HLXAERO. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(HLX_MACROS
  init_vis.mac
  init.mac
  nrc.mac
  vis.mac
  nrc_batch.mac
  )

set(HLX_SCRIPTS
  batchRunSim.sh
  sourceGeant4.sh
)


set(HLX_DATA
  thickness.dat
  surface.dat
)

foreach(_script ${HLX_MACROS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/macros/${_script}
    ${PROJECT_BINARY_DIR}/macros/${_script}
    COPYONLY
    )
endforeach()


foreach(_script ${HLX_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


foreach(_script ${HLX_DATA})
  configure_file(
    ${PROJECT_SOURCE_DIR}/data/${_script}
    ${PROJECT_BINARY_DIR}/data/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(HLXAERO DEPENDS HLX)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS HLX DESTINATION bin)
#install(TARGETS analyzeRuns DESTINATION bin)
