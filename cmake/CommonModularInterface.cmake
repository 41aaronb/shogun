MACRO(GENERATE_MODULAR_TARGET MODULAR_NAME MODULAR_DIR MODULAR_LIBARIES)

if(${MODULAR_NAME} STREQUAL "python")
	SET(PREPEND_TARGET "_")
endif()

set(modular_files)
FILE(GLOB_RECURSE MODULAR_FILES ${COMMON_MODULAR_SRC_DIR}/*.i)
FILE(GLOB_RECURSE CUSTOM_MODULAR_FILES ${MODULAR_DIR}/*.i)
LIST(APPEND MODULAR_FILES ${CUSTOM_MODULAR_FILES})
FOREACH(file ${MODULAR_FILES})
	STRING(REGEX REPLACE ".*/(.*)$" "\\1" fname "${file}")
	list(APPEND modular_files ${fname})
	ADD_CUSTOM_COMMAND(OUTPUT ${fname}
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${file} ${fname}
	)
ENDFOREACH()

ADD_CUSTOM_TARGET(${MODULAR_NAME}_modular_src 
	DEPENDS ${modular_files}
	COMMENT "copying SWIG files")

INCLUDE(${SWIG_USE_FILE})
SET_SOURCE_FILES_PROPERTIES(modshogun.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(modshogun.i PROPERTIES SWIG_FLAGS ${TARGET_SWIGFLAGS})
SWIG_ADD_MODULE(${MODULAR_NAME}_modular ${MODULAR_NAME} modshogun.i sg_print_functions.cpp)
SWIG_LINK_LIBRARIES(${MODULAR_NAME}_modular shogun ${MODULAR_LIBARIES})
ADD_DEPENDENCIES(${PREPEND_TARGET}${MODULAR_NAME}_modular ${MODULAR_NAME}_modular_src)

IF(DOXYGEN_FOUND)
	configure_file(${COMMON_MODULAR_SRC_DIR}/modshogun.doxy.in modshogun.doxy)

	ADD_CUSTOM_COMMAND( 
    	OUTPUT    modshogun
    	COMMAND   ${DOXYGEN_EXECUTABLE}
    	ARGS	  modshogun.doxy
    	COMMENT   "Generating doxygen doc"
	)

	ADD_CUSTOM_COMMAND( 
    	OUTPUT    modshogun_doxygen.i
    	COMMAND   ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/.doxy2swig.py
    	ARGS	  --quiet --no-function-definition modshogun/doxygen_xml/index.xml modshogun_doxygen.i
    	DEPENDS   modshogun
	)
	ADD_CUSTOM_TARGET(${MODULAR_NAME}_doxy2swig DEPENDS modshogun_doxygen.i)
	ADD_DEPENDENCIES(${PREPEND_TARGET}${MODULAR_NAME}_modular ${MODULAR_NAME}_doxy2swig)
ELSE()
	#TODO add scrubing
ENDIF()

ENDMACRO()