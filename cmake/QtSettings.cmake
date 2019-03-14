# Qt Framework

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC TRUE)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH $ENV{QTDIR}/lib/cmake)

find_package(Qt5Core REQUIRED)
#find_package(Qt5Quick REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
