cmake_minimum_required(VERSION 3.1)

string(TIMESTAMP CURRENT_BUILD_TIME "%Y-%m-%d %H:%M")
message(STATUS "\n\n\n#######################################################\n")
message(STATUS "\t New build! Current time: " ${CURRENT_BUILD_TIME})
message(STATUS "\n#######################################################\n")
message(STATUS "==========================================================")
project(avicon CXX)
enable_language(CXX)
set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_CXX_STANDARD 14)
message(STATUS "Host system:")
message(STATUS "System: " ${CMAKE_SYSTEM})
message(STATUS "System name: " ${CMAKE_SYSTEM_NAME})
message(STATUS "System version: " ${CMAKE_SYSTEM_VERSION})
message(STATUS "System cpu: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "System host name: " ${CMAKE_HOST_SYSTEM_NAME})
message(STATUS "Toolchain file: " ${CMAKE_TOOLCHAIN_FILE})
if(CMAKE_TOOLCHAIN_FILE MATCHES ".(buildroot).")
    include(${CMAKE_TOOLCHAIN_FILE})
    set(TARGET_SELECTED_ARCH "buildroot-imx")
    add_definitions(-DIMX_DEVICE)
elseif(CMAKE_TOOLCHAIN_FILE MATCHES ".(android).")
    include(${CMAKE_TOOLCHAIN_FILE})
    set(TARGET_SELECTED_ARCH "android-nautiz-x8")
    add_definitions(-DANDROID)
else()
    set(TARGET_SELECTED_ARCH "desktop")
endif()
message(STATUS "==========================================================")

message(STATUS "Compiler:")
message(STATUS "CXX Compiler path: " ${CMAKE_CXX_COMPILER})
message(STATUS "CXX Compiler ABI: " ${CMAKE_CXX_COMPILER_ABI})
message(STATUS "CXX Compiler AR path: " ${CMAKE_CXX_COMPILER_AR})
message(STATUS "CXX Compiler ARCH_ID: " ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID})
message(STATUS "CXX Compiler RANLIB path: " ${CMAKE_CXX_COMPILER_RANLIB})
message(STATUS "CXX Compiler TARGET: " ${CMAKE_CXX_COMPILER_TARGET})
message(STATUS "CXX Compiler version: " ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS "CXX platform id: " ${CMAKE_CXX_PLATFORM_ID})
message(STATUS "CXX standard: " ${CMAKE_CXX_STANDARD})

message(STATUS "C Compiler path: " ${CMAKE_C_COMPILER})
message(STATUS "C Compiler ABI: " ${CMAKE_C_COMPILER_ABI})
message(STATUS "C Compiler AR path: " ${CMAKE_C_COMPILER_AR})
message(STATUS "C Compiler ARCH_ID: " ${CMAKE_C_COMPILER_ARCHITECTURE_ID})
message(STATUS "C Compiler RANLIB path: " ${CMAKE_C_COMPILER_RANLIB})
message(STATUS "C Compiler TARGET: " ${CMAKE_C_COMPILER_TARGET})
message(STATUS "C Compiler version: " ${CMAKE_C_COMPILER_VERSION})

message(STATUS "Strip path: " ${CMAKE_STRIP})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Build tool: " ${CMAKE_BUILD_TOOL})
message(STATUS "Crosscompiling: " ${CMAKE_CROSSCOMPILING})
message(STATUS "Compiler type: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "==========================================================")

message(STATUS "Android:")
message(STATUS "Android ARCH: " ${ANDROID_ARCH})
message(STATUS "Android API: " ${ANDROID_API})
message(STATUS "Android ABI: " ${ANDROID_ABI})
message(STATUS "Android toolchain machine: " ${CMAKE_CXX_ANDROID_TOOLCHAIN_MACHINE})
message(STATUS "Android toolchain prefix: " ${CMAKE_CXX_ANDROID_TOOLCHAIN_PREFIX})
message(STATUS "Android STL type: " ${ANDROID_STL_TYPE})
message(STATUS "==========================================================")

message(STATUS "Architecture:")
message(STATUS "Selected architecture: " ${TARGET_SELECTED_ARCH})
message(STATUS "==========================================================")

execute_process(
  COMMAND ./get_hash.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Git hash: " ${GIT_HASH})

execute_process(
  COMMAND ./get_defcore_hash.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_SUBMODULE_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Git submodule hash: " ${GIT_SUBMODULE_HASH})

execute_process(
  COMMAND git symbolic-ref --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Git branch: " ${GIT_BRANCH})

execute_process(
  COMMAND ./get_tag.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Git tag: " ${GIT_TAG})
message(STATUS "==========================================================")

add_definitions(-DAPP_VERSION=\"${GIT_TAG}\")
add_definitions(-DAVICON_HASH=\"${GIT_HASH}\")
add_definitions(-DDEFCORE_HASH=\"${GIT_SUBMODULE_HASH}\")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_BUILD_RPATH "/usr/local/qwt-release/lib:/usr/lib")
message(STATUS "Cmake:")
message(STATUS "Source dir: " ${CMAKE_SOURCE_DIR})
message(STATUS "Binary dir: " ${CMAKE_BINARY_DIR})
message(STATUS "Module path: " ${CMAKE_MODULE_PATH})
message(STATUS "Qmake path: " ${QT_QMAKE_EXECUTABLE})
message(STATUS "RPath:" ${CMAKE_INSTALL_RPATH})
message(STATUS "==========================================================")
#set(TOOLCHAIN $ENV{TOOLCHAIN})
#if(DEFINED TOOLCHAIN)
#    include(cmake/toolchain/${TOOLCHAIN}.cmake)
#endif()

#include(cmake/VerboseCompilerMessage.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(qwt REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/core/models)
include_directories(${CMAKE_SOURCE_DIR}/core/powermanagement)
include_directories(${CMAKE_SOURCE_DIR}/ui)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/styles)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/lateralpanels)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/internationalization)
include_directories(${CMAKE_SOURCE_DIR}/defcore)

message(STATUS "Library locations:")
get_target_property(QtCore_location Qt5::Core LOCATION)
message(STATUS "Qt core:" ${QtCore_location})
get_target_property(QtWidgets_location Qt5::Widgets LOCATION)
message(STATUS "Qt widgets:" ${QtWidgets_location})
message(STATUS "==========================================================")

message(STATUS "Build flags:")

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fsanitize=undefined -fsanitize=integer -fsanitize=nullability ")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS " -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -O0 -g ")
#set(CMAKE_CXX_FLAGS " -O0 -g ")
#set(CMAKE_CXX_FLAGS " -pg ")
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

#set (CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEFCORE_DEBUG -fno-omit-frame-pointer -fstack-protector-all -fsanitize=undefined -fsanitize=integer -fsanitize=nullability ")
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEFCORE_DEBUG -fno-omit-frame-pointer -fstack-protector-all -fsanitize=undefined -fsanitize=integer -fsanitize=nullability ")
#set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -O0 -g -DDEFCORE_DEBUG -fno-omit-frame-pointer -fstack-protector-all -fsanitize=undefined -fsanitize=integer -fsanitize=nullability ")

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEFCORE_DEBUG -fno-omit-frame-pointer -fstack-protector-all -fno-exceptions -fno-rtti ")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEFCORE_DEBUG -fno-omit-frame-pointer -fstack-protector-all -fno-exceptions -fno-rtti ")
set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -O0 -g -DDEFCORE_DEBUG -fno-omit-frame-pointer -fstack-protector-all -fno-exceptions -fno-rtti ")

# -march=armv7-a -mfpu=vfpv3
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=Cortex-A8 -march=armv7-a -mfpu=neon -O3 -ftree-vectorize -fno-exceptions -fno-rtti ")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mcpu=Cortex-A8 -march=armv7-a -mfpu=neon -O3 -ftree-vectorize -fno-exceptions -fno-rtti ")
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s -fno-exceptions")

#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DDEFCORE_NO_DEBUG ")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DDEFCORE_NO_DEBUG ")
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -DDEFCORE_NO_DEBUG -s")

message(STATUS "CXX_FLAGS:" ${CMAKE_CXX_FLAGS})
message(STATUS "CC Compiler:" ${CMAKE_CC_COMPILER})

message(STATUS "==========================================================")

message(STATUS "Subdirectory core:")
add_subdirectory(core)
message(STATUS "==========================================================")
message(STATUS "Subdirectory defcore:")
add_subdirectory(defcore)
message(STATUS "==========================================================")
message(STATUS "Subdirectory ui:")
add_subdirectory(ui)
message(STATUS "==========================================================")


add_executable(avicon31 "${CMAKE_SOURCE_DIR}/ui/widgets/main.cpp" ui/resources/resources.qrc ui/resources/schemes_avicon31.qrc ui/widgets/internationalization/internationalization.qrc)
target_compile_definitions(avicon31 PUBLIC -DTARGET_AVICON31)
target_link_libraries(avicon31 core_31 ui_31 defcore_31 qwt Qt5::Core Qt5::Widgets -static-libstdc++ dl)

add_executable(avicondb "${CMAKE_SOURCE_DIR}/ui/widgets/main.cpp" ui/resources/resources.qrc ui/resources/schemes_db.qrc ui/widgets/internationalization/internationalization.qrc)
target_compile_definitions(avicondb PUBLIC -DTARGET_AVICONDB)
target_link_libraries(avicondb core_db ui_db defcore_db qwt Qt5::Core Qt5::Widgets -static-libstdc++ dl)

add_executable(avicondbhs "${CMAKE_SOURCE_DIR}/ui/widgets/main.cpp" ui/resources/resources.qrc ui/resources/schemes_db.qrc ui/widgets/internationalization/internationalization.qrc)
target_compile_definitions(avicondbhs PUBLIC -DTARGET_AVICONDBHS)
target_link_libraries(avicondbhs core_dbhs ui_dbhs defcore_dbhs qwt Qt5::Core Qt5::Widgets -static-libstdc++ dl)

add_executable(avicon15 "${CMAKE_SOURCE_DIR}/ui/widgets/main.cpp" ui/resources/resources.qrc ui/resources/schemes_avicon15.qrc ui/widgets/internationalization/internationalization.qrc)
target_compile_definitions(avicon15 PUBLIC -DTARGET_AVICON15)
target_link_libraries(avicon15 core_15 ui_15 defcore_15 qwt Qt5::Core Qt5::Widgets -static-libstdc++ dl)

add_executable(avicon31e "${CMAKE_SOURCE_DIR}/ui/widgets/main.cpp" ui/resources/resources.qrc ui/widgets/internationalization/internationalization.qrc)
target_compile_definitions(avicon31e PUBLIC -DTARGET_AVICON31E)
target_link_libraries(avicon31e core_31e ui_31e defcore_31e qwt Qt5::Core Qt5::Widgets -static-libstdc++ dl)
