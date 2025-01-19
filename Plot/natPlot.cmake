set(NATPLOT_NAME natPlot)

file(GLOB NATPLOT_INC_GUI  ${MY_INC}/gui/*.h)
file(GLOB NATPLOT_INC_GUI_PLOT  ${MY_INC}/gui/plot/*.h)
file(GLOB NATPLOT_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB NATPLOT_INCLUDE  ${CMAKE_CURRENT_LIST_DIR}/inc/*.h)
file(GLOB NATPLOT_HEADERS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB NATPLOT_COMMON_SRC ${CMAKE_CURRENT_LIST_DIR}/../common/*.cpp)
file(GLOB NATPLOT_COMMON_H ${CMAKE_CURRENT_LIST_DIR}/../common/*.h)


source_group("inc"        FILES ${NATPLOT_INCLUDE})
source_group("inc\\gui"            FILES ${NATPLOT_INC_GUI})
source_group("inc\\gui\\plot"            FILES ${NATPLOT_INC_GUI_PLOT})
source_group("inc\\src"     FILES ${NATPLOT_HEADERS})
source_group("inc\\common"     FILES ${NATPLOT_COMMON_H})
source_group("src"         FILES ${NATPLOT_SOURCES})
source_group("src\\common"     FILES ${NATPLOT_COMMON_SRC})

add_library(${NATPLOT_NAME} SHARED  ${NATPLOT_INCLUDE} ${NATPLOT_SOURCES} ${NATPLOT_HEADERS} 
									${NATPLOT_INC_GUI} ${NATPLOT_INC_GUI_PLOT} 
									${NATPLOT_COMMON_SRC} ${NATPLOT_COMMON_H})

target_link_libraries(${NATPLOT_NAME} 		debug ${MU_LIB_DEBUG} 		optimized ${MU_LIB_RELEASE} 
										    debug ${NATGUI_LIB_DEBUG}   optimized ${NATGUI_LIB_RELEASE})

target_compile_definitions(${NATPLOT_NAME} PRIVATE NATPLOT_EXPORTS)

#setIDEPropertiesForLib(${NATPLOT_NAME})
#copyPBSharedLibToMyLib(${NATPLOT_NAME})

