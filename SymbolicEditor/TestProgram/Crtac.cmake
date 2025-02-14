include(${CMAKE_CURRENT_LIST_DIR}/../Graph/graph.cmake)

set(EXECUTABLE_NAME Plotter)

file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB INCS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

source_group("headers"            FILES ${INCS})
source_group("src"            FILES ${SOURCES})
source_group("src"            FILES ${COMMON})


# add executable
add_executable(${EXECUTABLE_NAME} ${INCS} ${SOURCES} ${COMMON})

target_link_libraries(${EXECUTABLE_NAME}	debug ${MU_LIB_DEBUG}   optimized ${MU_LIB_RELEASE}
										debug ${NATGUI_LIB_DEBUG}  optimized ${NATGUI_LIB_RELEASE} 
										${GRAPH_NAME})

set(CMAKE_WIN32_EXECUTABLE TRUE)

set(APPWNDMENUANDTB_PLIST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)
setTargetPropertiesForGUIApp(${EXECUTABLE_NAME} $(APPWNDMENUANDTB_PLIST)) 


setIDEPropertiesForGUIExecutable(${EXECUTABLE_NAME} ${CMAKE_CURRENT_LIST_DIR})

setPlatformDLLPath(${EXECUTABLE_NAME})


