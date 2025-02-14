set(EqToXML_LIB_NAME Eq2ToXMLLib)
set(EqToXML_EXE_NAME modlToXML)

file(GLOB EQ2XML_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/library/*.cpp)
file(GLOB EQ2XML_INCLUDE  ${CMAKE_CURRENT_LIST_DIR}/inc/*.h)
file(GLOB EQ2XML_MAIN  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

add_library(${EqToXML_LIB_NAME} STATIC ${EQ2XML_INCLUDE} ${EQ2XML_SOURCES})

add_executable(${EqToXML_EXE_NAME} ${EQ2XML_MAIN})

target_link_libraries(${EqToXML_LIB_NAME}	debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE})

target_link_libraries(${EqToXML_EXE_NAME}	debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE}  ${EqToXML_LIB_NAME})

source_group("inc"        FILES ${EQ2XML_INCLUDE})
source_group("src"        FILES ${EQ2XML_MAIN})
source_group("src\\lib"   FILES ${EQ2XML_SOURCES})

#setIDEPropertiesForLib(${EqToXML_LIB_NAME})

