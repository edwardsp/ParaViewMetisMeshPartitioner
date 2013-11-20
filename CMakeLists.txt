cmake_minimum_required(VERSION 2.8)
project(ParaViewMetisMeshPartitioner)

## Set path for cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

## Find ParaView
find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})
include(ParaViewBranding)
include(ParaViewPlugins)

## Set plugin name and version
set(PLUGIN_NAME ParaViewMetisMeshPartitioner)
set(PLUGIN_VERSION "1.0")

## Find METIS
include(FindMETIS REQUIRED)

## Set include directories
include_directories(
    ${METIS_INCLUDE_DIRS}
    ${PARAVIEW_INCLUDE_DIRS}
    ${VTK_INCLUDE_DIR}
    )

## Set server-manager XML
set(SMXML
    resources/MetisMeshPartitioner.xml
    )

## Set server-manager sources
set(SMSRC
    vtkMetisMeshPartitioner.cxx
    )

## Set the GUI resource XML files
set(GUIXML
    resources/ParaViewMetisMeshPartitioner.xml
    )

## Set required libraries
set(RequiredLibs
    ${METIS_LIBRARIES}
    ${VTK_LIBRARIES}
    )

## Compile the paraview plugin
add_paraview_plugin(
    ${PLUGIN_NAME}
    ${PLUGIN_VERSION}
    GUI_RESOURCE_FILES ${GUIXML}
    SERVER_MANAGER_XML ${SMXML}
    SERVER_MANAGER_SOURCES ${SMSRC}
    )
target_link_libraries(${PLUGIN_NAME} ${RequiredLibs})
