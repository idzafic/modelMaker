cmake_minimum_required(VERSION 3.18)

set(SOLUTION_NAME modelMaker) #Unijeti naziv solution-a

#BEGIN dio koji ide u sve solutions
project(${SOLUTION_NAME})
set(SOURCE_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(HOME_ROOT $ENV{HOME})
if (WIN32)
	string(REPLACE "\\" "/" HOME_ROOT "${HOME_ROOT}")
endif()
set(WORK_ROOT ${HOME_ROOT}/Work)
include(${WORK_ROOT}/DevEnv/Common.cmake) 
#END dio koji ide u sve solutions

include(${WORK_ROOT}/DevEnv/SQLite.cmake)
include(../../../mainUtils/dataProvider/dataProvider.cmake)

include(${WORK_ROOT}/DevEnv/DataProvider.cmake)
if (WIN32)
	include(../../../GUI/natGUI/winGUI.cmake)
elseif(APPLE)
	include(../../../GUI/natGUI/macGUI.cmake)
else()
	include(../../../GUI/natGUI/linGUI.cmake)
endif()

add_dependencies(${NATGUI_NAME} ${DP_NAME}) 

include(${WORK_ROOT}/DevEnv/natGUI.cmake)

include(../../../Plot/natPlot.cmake)
add_dependencies(${NATPLOT_NAME} ${NATGUI_NAME}) 

include(${WORK_ROOT}/DevEnv/SymbComp.cmake)
include(${WORK_ROOT}/DevEnv/natPlot.cmake)

include(../ModelMaker/modelMaker.cmake)
add_dependencies(${MODELMAKER_NAME} ${NATGUI_NAME} ${NATPLOT_NAME} ${SYMBSOLVER_NAME})