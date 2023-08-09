cmake_minimum_required(VERSION 3.26)
project(IPC_TCP)

set(CMAKE_CXX_STANDARD 17)

include(functions.cmake)
setHomeEnv()

# user.cmake

set(PCL_INCLUDEDIR "$ENV{HOME}/source/repos/GIT/3rdParty/PCL-1.11.0_msvc2017_64")
set(BOOST_INCLUDEDIR "${PCL_INCLUDEDIR}/3rdParty/Boost/")

# -----------------------------------------------------------------------------
# boost specific configuration

include_pcl_package( PCL_INCLUDEDIR ) # pcl first, then boost inside pcl
include_boost_package( BOOST_INCLUDEDIR )

include_directories(${Boost_INCLUDE_DIRS})
#link_directories(${BOOST_LIBRARY_DIRS})
link_libraries(${Boost_LIBRARIES})

#add_executable(IPC_exemple main.cpp)
#target_include_directories(IPC_srv PRIVATE ${BOOST_INCLUDE_DIRS})

add_executable(IPC_server IPC_server.cpp)
add_executable(IPC_client IPC_client.cpp)

add_executable(TCP_client TCP_client.cpp)
add_executable(TCP_srv_sync TCP_server_sync.cpp)