set(MODELMAKER_NAME modelMaker)

file(GLOB MODELMAKER_SRC  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB MODELMAKER_INC  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB MODELMAKER_INC_TD  ${MY_INC}/td/*.h)
file(GLOB MODELMAKER_INC_GUI ${MY_INC}/gui/*.h)
file(GLOB MODELMAKER_INC_GUI_PLOT ${MY_INC}/gui/plot/*.h)
file(GLOB MODELMAKER_INC_XML ${MY_INC}/xml/*.h)

include(${WORK_ROOT}/DevEnv/natGUI.cmake)

#include(${WORK_ROOT}/DevEnv/MatrixLib.cmake)
include(${WORK_ROOT}/DevEnv/SymbComp.cmake)
#include(${WORK_ROOT}/DevEnv/natPlot.cmake)
include(../../Plot/natPlot.cmake)

file(GLOB MODELMAKER_SRC_GUI  ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*/*.cpp ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*.cpp)
file(GLOB MODELMAKER_SRC_GUI_INC  ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*/*.h ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*.h)

file(GLOB MODELMAKER_SRC_TEXT  ${CMAKE_CURRENT_LIST_DIR}/src/textEditor/*.cpp)
file(GLOB MODELMAKER_INC_TEXT  ${CMAKE_CURRENT_LIST_DIR}/src/textEditor/*.h)

file(GLOB INC ${CMAKE_CURRENT_LIST_DIR}/../common/*.cpp)

set(MODELMAKER_PLIST  ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/AppIcon.plist)
if(WIN32)
	set(MODELMAKER_WINAPP_ICON ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/winAppIcon.rc)
else()
	set(MODELMAKER_WINAPP_ICON ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/winAppIcon.cpp)
endif()

source_group("inc"           	FILES ${MODELMAKER_INC})
source_group("inc\\td"           	FILES ${MODELMAKER_INC_TD})
source_group("inc\\gui"           FILES ${MODELMAKER_INC_GUI})
source_group("inc\\gui\\plot"      FILES ${MODELMAKER_INC_GUI_PLOT})
source_group("inc\\xml"           FILES ${MODELMAKER_INC_XML})

source_group("src\\text"           FILES ${MODELMAKER_INC_TEXT})

source_group("src\\main"           FILES ${MODELMAKER_SRC})
source_group("src\\gui"            FILES  ${MODELMAKER_SRC_GUI_INC} ${MODELMAKER_SRC_GUI})
source_group("src\\text"           FILES ${MODELMAKER_SRC_TEXT})

# add executable
add_executable(${MODELMAKER_NAME} ${MODELMAKER_INC} ${MODELMAKER_SRC} ${MODELMAKER_SRC_GUI_INC} ${MODELMAKER_SRC_GUI} 
                                ${MODELMAKER_INC_TEXT} ${MODELMAKER_SRC_TEXT} ${MODELMAKER_INC_TD} ${MODELMAKER_INC_GUI} 
                                ${MODELMAKER_INC_GUI_PLOT} ${MODELMAKER_INC_XML} ${MODELMAKER_WINAPP_ICON})

include(${CMAKE_CURRENT_LIST_DIR}/../EquationToXML/EqToXML.cmake)
include(${WORK_ROOT}/DevEnv/DataProvider.cmake)

target_link_libraries(${MODELMAKER_NAME}	
					debug ${MU_LIB_DEBUG}
					debug ${NATGUI_LIB_DEBUG} 
					debug ${SYMBCOMP_LIB_DEBUG}
					#debug ${NATPLOT_LIB_DEBUG}
					debug ${NATPLOT_NAME}
					debug ${DP_LIB_DEBUG}
					optimized ${NATGUI_LIB_RELEASE}
					optimized ${SYMBCOMP_LIB_RELEASE}
					optimized ${MU_LIB_RELEASE}
					#optimized ${NATPLOT_LIB_RELEASE}
					optimized ${NATPLOT_NAME}
					optimized ${DP_LIB_RELEASE}
					${EqToXML_LIB_NAME})

setTargetPropertiesForGUIApp(${MODELMAKER_NAME} ${MODELMAKER_PLIST})
setAppIcon(${MODELMAKER_NAME} ${CMAKE_CURRENT_LIST_DIR}) 
setIDEPropertiesForGUIExecutable(${MODELMAKER_NAME} ${CMAKE_CURRENT_LIST_DIR})
setIDEPropertiesForExecutable(${MODELMAKER_NAME})

setPlatformDLLPath(${MODELMAKER_NAME})




