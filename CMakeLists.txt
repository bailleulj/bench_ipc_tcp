cmake_minimum_required(VERSION 3.26)
project(IPC_TCP)

set(CMAKE_CXX_STANDARD 17)

include(functions.cmake)
setHomeEnv()

# user.cmake must exist in repo, cf doc for example
if (EXISTS ${CMAKE_SOURCE_DIR}/user.cmake)
    include(user.cmake)
else()
    message(FATAL_ERROR "user.cmake not found at ${CMAKE_SOURCE_DIR} , please create one similar as one in doc/")
endif ()


# -----------------------------------------------------------------------------
# boost configuration

include_pcl_package( PCL_INCLUDEDIR ) # pcl first, then boost inside pcl
include_boost_package( BOOST_INCLUDEDIR )

include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

# -----------------------------------------------------------------------------


add_executable(IPC_server src/IPC_server.cpp)
add_executable(IPC_client src/IPC_client.cpp)

add_executable(TCP_client src/TCP_client.cpp)
add_executable(TCP_srv_sync src/TCP_server_sync.cpp)