cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(DIST_MILP)
set (CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(MPI REQUIRED)

include_directories(include
		    ${EIGEN3_INCLUDE_DIRS}
			${MPI_INCLUDE_PATH}
		    "/opt/ibm/ILOG/CPLEX_Studio129/cplex/include"
		    "/opt/ibm/ILOG/CPLEX_Studio129/concert/include")

link_directories("/opt/ibm/ILOG/CPLEX_Studio129/cplex/lib/x86-64_linux/static_pic"
		 "/opt/ibm/ILOG/CPLEX_Studio129/concert/lib/x86-64_linux/static_pic")
set (CPLEX_LINK_LIBS libilocplex.a libconcert.a libcplex.a libcplexdistmip.a m Threads::Threads dl)

file(GLOB SOURCES "src/*.cpp")

add_definitions(-DIL_STD) 
add_executable(${PROJECT_NAME} ${SOURCES})
#add_executable(${PROJECT_NAME} "src/ilomipex1.cpp")
#add_executable(get_test_cut "src/wrtnode.c")

target_link_libraries(${PROJECT_NAME} ${CPLEX_LINK_LIBS} ${EIGEN3_INCLUDE_DIRS} ${MPI_INCLUDE_PATH} libpugixml.a)
#target_link_libraries(get_test_cut ${CPLEX_LINK_LIBS})
