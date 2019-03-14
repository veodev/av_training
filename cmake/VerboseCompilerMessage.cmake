message(STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})

# if this is set to ON, then all libraries are built as shared libraries by default.
message(STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS})

# the compiler used for C files
message(STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER})

# the compiler flags for compiling C sources
message(STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS})

# the compiler used for C++ files
message(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})

# the compiler flags for compiling C++ sources
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

# the tools for creating libraries
message(STATUS "CMAKE_AR: " ${CMAKE_AR})
message(STATUS "CMAKE_RANLIB: " ${CMAKE_RANLIB})

message(STATUS "CMAKE_CXX_LINK_EXECUTABLE: " ${CMAKE_CXX_LINK_EXECUTABLE})
message(STATUS "CMAKE_CXX_LINK_FLAGS: " ${CMAKE_CXX_LINK_FLAGS})

message(STATUS "CMAKE_CROSSCOMPILING: " ${CMAKE_CROSSCOMPILING})

message(STATUS "LINK_LIBRARIES: " ${LINK_LIBRARIES})

message(STATUS "SYSROOT: " ${SYSROOT})