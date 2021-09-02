cmake_minimum_required (VERSION 2.8)

project ( dll_injector )

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

include_directories ( ${PARAMKIT_DIR}/include )

set (srcs
	main.cpp
	dll_injection.cpp
	process_privilege.cpp
	actions.cpp
)
set (hdrs
	params.h
	dll_injection.h
	process_privilege.h
	actions.h
)

add_executable ( ${PROJECT_NAME} ${hdrs} ${srcs} )

# link with paramkit.lib
target_link_libraries ( ${PROJECT_NAME} ${PARAMKIT_LIB} )

#dependencies:
add_dependencies( ${PROJECT_NAME} paramkit )

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ${PROJECT_NAME} )
