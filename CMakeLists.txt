cmake_minimum_required ( VERSION 2.8 )

project ( dll_injector )

# modules:
set ( M_PARAMKIT_LIB "paramkit" )

# modules paths:
set (PARAMKIT_DIR "${CMAKE_SOURCE_DIR}/${M_PARAMKIT_LIB}/${M_PARAMKIT_LIB}" CACHE PATH "ParamKit main path" )
add_subdirectory ( ${PARAMKIT_DIR} )
set ( PARAMKIT_LIB $<TARGET_FILE:paramkit> CACHE PATH "ParamKit library path" )

# main project
add_subdirectory ( dll_injector )
