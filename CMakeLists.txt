cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(DIST_MILP)
set (CMAKE_CXX_FLAGS "-ggdb3 -Wall -std=c++11")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(MPI REQUIRED)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
else()
	find_library(wiringPi_LIB wiringPi)
endif ()

include_directories(include
			apps
			lib
		    ${EIGEN3_INCLUDE_DIRS}
			${MPI_INCLUDE_PATH}
#		    "/opt/ibm/ILOG/CPLEX_Studio129/cplex/include"
#		    "/opt/ibm/ILOG/CPLEX_Studio129/concert/include"
			)

#link_directories("/opt/ibm/ILOG/CPLEX_Studio129/cplex/lib/x86-64_linux/static_pic"
#		 "/opt/ibm/ILOG/CPLEX_Studio129/concert/lib/x86-64_linux/static_pic")
#set (CPLEX_LINK_LIBS libilocplex.a libconcert.a libcplex.a libcplexdistmip.a m Threads::Threads dl)

file(GLOB SOURCES "src/*.cpp")
file(GLOB APPS "apps/*.cpp")
file(GLOB LIBS "lib/*.cpp")

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	list(REMOVE_ITEM LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/PhidgetHelperFunctions.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/PhidgetHelperFunctions.hpp)
else()
endif ()

list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/CPLEX_SOLVER.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/NOC_CPLEX.cpp) # not using CPLEX for now

add_definitions(-DIL_STD)
add_executable(${PROJECT_NAME} ${SOURCES} ${APPS} ${LIBS})

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
#	target_link_libraries(${PROJECT_NAME} ${CPLEX_LINK_LIBS} ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} libpugixml.a ${wiringPi_LIB} -lrt -lcrypt -lglpk -lphidget22)
	target_link_libraries(${PROJECT_NAME} ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} -lglpk)
else()
	target_link_libraries(${PROJECT_NAME} ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} ${wiringPi_LIB} -lrt -lcrypt -lglpk -lphidget22)
endif ()
