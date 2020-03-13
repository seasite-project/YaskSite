cmake_minimum_required(VERSION 2.8)

# project name
project(yaskSite_example CXX C)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_PREFIX_PATH ${yaskSite_DIR})
# find yaskSite
find_package(yaskSite REQUIRED CONFIG)
message(STATUS "Found yaskSite")

include_directories(${yaskSite_INCLUDE_DIR})
link_libraries(${yaskSite_LIBRARIES})
set(CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED On)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${yaskSite_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${yaskSite_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${yaskSite_CXX_FLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -xHOST")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -xHOST")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-write-strings")
endif()

find_package(OpenMP)

if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(perf src/perf.cpp src/parse.cpp)
add_executable(perf_wo_likwid src/perf_wo_likwid.cpp src/parse.cpp)
#add_executable(perf_optimizer src/perf_optimizer.cpp src/parse.cpp)
add_executable(INSPECT src/INSPECT_adapt.cpp src/parse.cpp)
add_executable(INSPECT_not_opt src/INSPECT_adapt_not_opt.cpp src/parse.cpp)
add_executable(perf_irk src/perf_irk.cpp src/parse.cpp)
add_executable(perf_irk_unrolled src/perf_irk_unrolled.cpp src/parse.cpp)
add_executable(perf_latency src/perf_latency_proof.cpp src/parse.cpp)
add_executable(codeGen src/codeGenExample.cpp)
add_executable(main src/main.cpp)
add_executable(perf_prefetch src/perf_prefetch.cpp src/parse.cpp)
add_executable(perf_y_prefetch src/perf_y_prefetch.cpp src/parse.cpp)
add_executable(perf_assoc src/perf_assoc.cpp src/parse.cpp)
add_executable(perf_assoc_2 src/perf_assoc_2.cpp src/parse.cpp)
add_executable(perf_prefetch_shared_private_cache src/perf_prefetch_shared_private_cache.cpp src/parse.cpp)
add_executable(run src/run.cpp src/parse.cpp)
add_executable(run_irk src/run_irk.cpp src/parse.cpp)
add_executable(run_irk_3 src/run_irk_3.cpp src/parse.cpp)
add_executable(offsite_adapter src/offsite_adapter.cpp src/offsite_parse.cpp)
file(COPY src/perf_y_prefetch.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/.)
file(COPY src/getBtwString.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/.)
file(COPY src/ys_offtune DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/.)
