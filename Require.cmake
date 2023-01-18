include_directories(${CMAKE_CURRENT_LIST_DIR}/include )


file(GLOB_RECURSE CURRENT_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
        )

set(CLAID_SOURCES ${CLAID_SOURCES} ${CURRENT_SOURCES})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/jbind11 ${CMAKE_CURRENT_BINARY_DIR}/JavaCLAID)
include_directories(${CMAKE_CURRENT_LIST_DIR}/jbind11/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/jbind11/include/jbind11)

add_definitions(-D__JAVA_WRAPPERS__)
