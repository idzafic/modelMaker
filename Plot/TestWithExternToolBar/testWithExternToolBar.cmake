
set(GRAPHTEST_WETB_NAME testWithExternToolBar)
set(GRAPHTEST_WETB_PLIST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)
file(GLOB GRAPHTEST_WETB_INC_GUI  ${MY_INC}/gui/*.h)
file(GLOB GRAPHTEST_WETB_INC_GUI_GRAPH  ${MY_INC}/gui/plot/*.h)
file(GLOB GRAPHTEST_WETB_INC_TD  ${MY_INC}/td/*.h)
file(GLOB GRAPHTEST_WETB_INC_CNT  ${MY_INC}/cnt/*.h)
file(GLOB GRAPHTEST_WETB_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB GRAPHTEST_WETB_INCS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

source_group("inc"            FILES ${GRAPHTEST_WETB_INCS})
source_group("inc\\gui"            FILES ${GRAPHTEST_WETB_INC_GUI})
source_group("inc\\gui\\plot"            FILES ${GRAPHTEST_WETB_INC_GUI_GRAPH})
source_group("inc\\td"            FILES ${GRAPHTEST_WETB_INC_TD})
source_group("inc\\cnt"            FILES ${GRAPHTEST_WETB_INC_CNT})
source_group("src"            FILES ${GRAPHTEST_WETB_SOURCES})


# add executable
add_executable(${GRAPHTEST_WETB_NAME} ${GRAPHTEST_WETB_INCS} ${GRAPHTEST_WETB_SOURCES} ${GRAPHTEST_WETB_INC_GUI}  
								${GRAPHTEST_WETB_INC_GUI_GRAPH} ${GRAPHTEST_WETB_INC_TD} ${GRAPHTEST_WETB_INC_CNT} )

target_link_libraries(${GRAPHTEST_WETB_NAME}	debug ${MU_LIB_DEBUG}   optimized ${MU_LIB_RELEASE}
												debug ${NATGUI_LIB_DEBUG}  optimized ${NATGUI_LIB_RELEASE} 
												debug ${NATPLOT_NAME}  optimized ${NATPLOT_NAME} )

set(CMAKE_WIN32_EXECUTABLE TRUE)

set(APPWNDMENUANDTB_PLIST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)
setTargetPropertiesForGUIApp(${GRAPHTEST_WETB_NAME} $(APPWNDMENUANDTB_PLIST)) 

setTargetPropertiesForGUIApp(${GRAPHTEST_WETB_NAME} ${GRAPHTEST_WETB_PLIST})

setIDEPropertiesForGUIExecutable(${GRAPHTEST_WETB_NAME} ${CMAKE_CURRENT_LIST_DIR})

setPlatformDLLPath(${GRAPHTEST_WETB_NAME})


