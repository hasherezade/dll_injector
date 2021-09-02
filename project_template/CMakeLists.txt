cmake_minimum_required (VERSION 2.8)

project ( project_template )

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

include_directories ( ${PARAMKIT_DIR}/include )

set (srcs
#put your sources here
)

# general headers - they will be used for both EXE and DLL:
set (hdrs
#put your headers here
)

set (rsrc
#put your resources here
)

add_executable ( ${PROJECT_NAME} ${exe_hdrs} ${srcs} ${rsrc} main.cpp )

# link with paramkit.lib
target_link_libraries ( ${PROJECT_NAME} ${PARAMKIT_LIB} )

#dependencies:
add_dependencies( ${PROJECT_NAME} paramkit )

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ${PROJECT_NAME} )
