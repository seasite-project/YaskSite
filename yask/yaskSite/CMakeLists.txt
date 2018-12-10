cmake_minimum_required (VERSION 2.8)
project (yaskSite)

set(yaskSite_VERSION_MAJOR 0)
set(yaskSite_VERSION_MINOR 0)
set(yaskSite_VERSION_REVISION 0)
set(yaskSite_VERSION_IDENTIFIER)
set(yaskSite_VERSION "${yaskSite_VERSION_MAJOR}.${yaskSite_VERSION_MINOR}.${yaskSite_VERSION_REVISION}")

include(ExternalProject)
include(${PROJECT_SOURCE_DIR}/cmake/OptimizeForArchitecture.cmake)

set(yaskSite_VERBOSITY 1 CACHE STRING "Level of verbosity information. Valid values: 0 (be totally silent), 1 (print warnings, errors and further important information) 2 (print YASK outputs)")
set(yaskSite_PRINT_ONLYFIRST 0 CACHE BOOL "Print all errors, warnings and info only once for each occurence")

if (NOT BUILD_SHARED_LIBS)
    message(STATUS "No library type specified. Setting it to shared.")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Set to ON/OFF for shared/static library build.")
endif()


set (SOURCES
    src/yaskSite.cpp
    src/yaskFork.cpp
    src/util.cpp
    src/MPI_Manager.cpp
    )

set (HEADERS
    include/yaskSite.h
    include/config.h
    include/macros.h
    include/yaskFork.h
    include/util.h
    include/MPI_Manager.h
    include/print.h
    )

set(LOG_SYS_CALL 0 CACHE BOOL "Log system calls (default: disabled)")

set(YASK_ABSOLUTE_PATH "not set" CACHE PATH "Set the absolute path to Intel YASK folder")
set(TOOL_DIR "${PROJECT_SOURCE_DIR}/src/tools")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")
set(CACHE_BUILD OFF  CACHE BOOL "Cache the stencil builds for later runs. Enabling cache requires approx. 50 MB diskspace per stencil")

message(STATUS "Detecting cache sizes")
#configure cache sizes
execute_process(COMMAND ${TOOL_DIR}/getCacheInfo.sh "L1d cache" OUTPUT_VARIABLE L1_val OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${TOOL_DIR}/getCacheInfo.sh "L2 cache" OUTPUT_VARIABLE L2_val OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${TOOL_DIR}/getCacheInfo.sh "L3 cache" OUTPUT_VARIABLE L3_val OUTPUT_STRIP_TRAILING_WHITESPACE)

set(L1_val "${L1_val}; private")
set(L2_val "${L2_val}; private")
set(L3_val "${L3_val}; shared")

set(L1 "${L1_val}" CACHE STRING "L1 cache size")
set(L2 "${L2_val}" CACHE STRING "L2 cache size")
set(L3 "${L3_val}" CACHE STRING "L3 cache size")

AutodetectHostArchitecture()
#OptimizeForArchitecture()
message(STATUS "Target Architecture detected is ${TARGET_ARCHITECTURE}")

#convert to YASK codes
if(TARGET_ARCHITECTURE STREQUAL "knl")
    set(TARGET_ARCH "knl")  # Knights Landing
elseif(TARGET_ARCHITECTURE STREQUAL "skylake-avx512")
    set(TARGET_ARCH "skx")  # SKYLAKE
elseif(TARGET_ARCHITECTURE STREQUAL "skylake" OR TARGET_ARCHITECTURE EQUAL "haswell")
    set(TARGET_ARCH "hsw")  # SKYLAKE-256 or HASWELL
elseif(TARGET_ARCHITECTURE STREQUAL "ivy-bridge")
    set(TARGET_ARCH "ivb")  # IVY-BRIDGE
elseif(TARGET_ARCHITECTURE STREQUAL "sandy-bridge")
    set(TARGET_ARCH "snb")  # IVY-BRIDGE
else()
    message(WARNING "TARGET ARCHITECTURE could not be detected, setting to intel64")
    set(TARGET_ARCH "intel64")
endif()

set(TARGET_ARCH "${TARGET_ARCH}" CACHE STRING "Target architecture detected by default; if needed to build for another CPU, please change this")

message(STATUS "Target Architecture detected is ${TARGET_ARCH}")

set(ENABLE_LOOPADAPT 0 CACHE BOOL "Enable loop adapt")

if(ENABLE_LOOPADAPT)
    set(LOOPADAPT_INCLUDE_DIR NOTFOUND CACHE PATH "Location of loop_adapt.h")
    set(LOOPADAPT_LIB_DIR NOTFOUND CACHE PATH "Location of loop_adapt library")

    if(LOOPADAPT_INCLUDE_DIR STREQUAL NOTFOUND)
        message(STATUS "Could not find loop_adapt.h; will install a local version")

        set(PREFIX_PATH "${PROJECT_BINARY_DIR}/loop_adapt-inst")
        file(WRITE "${PROJECT_BINARY_DIR}/config.txt" "PREFIX=${PREFIX_PATH}")

        ExternalProject_Add(loop_adapt
            PREFIX ${PROJECT_BINARY_DIR}/loop_adapt
            SOURCE_DIR ${PROJECT_BINARY_DIR}/loop_adapt
            URL ${PROJECT_SOURCE_DIR}/loop_adapt
            CONFIGURE_COMMAND mkdir -p "${PROJECT_BINARY_DIR}/loop_adapt-inst/lib" "${PROJECT_BINARY_DIR}/loop_adapt-inst/include" && mv "${PROJECT_BINARY_DIR}/config.txt" "${PROJECT_BINARY_DIR}/loop_adapt/config.mk"
            BUILD_COMMAND make
            BUILD_IN_SOURCE 1
            INSTALL_COMMAND make install
            )
        set(LOOPADAPT_INCLUDE_DIR "${PROJECT_BINARY_DIR}/loop_adapt-inst/include")
        set(LOOPADAPT_LIB_DIR "${PROJECT_BINARY_DIR}/loop_adapt-inst/lib/libloop_adapt.so")
    endif()
endif()

set(yaskSite_USE_OpenMP 1 CACHE BOOL "Build with OpenMP")

if(yaskSite_USE_OpenMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(yaskSite_HAVE_OpenMP 1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    else()
        message(FATAL_ERROR "Couldn't find OpenMP")
        set(yaskSite_HAVE_OpenMP 0)
    endif()
else ()
    message(STATUS "no check ${OpenMP_CXX_FLAGS}")
    set(yaskSite_HAVE_OpenMP 0)
endif()

set(yaskSite_USE_MPI 1 CACHE BOOL "Build with MPI" FORCE)

if(yaskSite_USE_MPI)
    set(MPI_LIBRARIES "-lmpi_mt")
    find_package(MPI)
    if (MPI_FOUND)
        include_directories(SYSTEM ${MPI_INCLUDE_PATH})
        foreach(item ${MPI_LIBRARIES})
            string(REGEX MATCH ".*libmpi.so" match ${item})
            if(match)
                get_filename_component(MPI_LIBDIR ${match} PATH)
                if (EXISTS "${MPI_LIBDIR}/release_mt/")
                    list(REMOVE_ITEM MPI_LIBRARIES ${item})
                    list(APPEND MPI_LIBRARIES
                        ${MPI_LIBDIR}/release_mt/libmpi.so)
                endif()
                if(EXISTS "${MPI_LIBDIR}/libmpi_mt.so")
                    list(REMOVE_ITEM MPI_LIBRARIES ${item})
                    list(APPEND MPI_LIBRARIES ${MPI_LIBDIR}/libmpi_mt.so)
                endif()
            endif()
        endforeach()
        get_filename_component(MPI_LIBDIR "${MPI_LIBRARY}" PATH)
        find_library(LIBMPI_CXX mpi_cxx ${MPI_LIBDIR})
        set(yaskSite_HAVE_MPI 1)
        mark_as_advanced(MPI_EXTRA_LIBRARY MPI_LIBRARY LIBMPI_CXX)
        mark_as_advanced(CLEAR MPI_C_INCLUDE_PATH)
    else ()
        message(FATAL_ERROR "I couldn't find MPI; YASK needs it")
        set(yaskSite_HAVE_MPI 0)
    endif()
else ()
    message(FATAL_ERROR "YASK needs MPI please enable yaskSite_USE_MPI via CMAKE")
    set(yaskSite_HAVE_MPI 0)
endif()

set(yaskSite_PRINT_RANK 0 CACHE STRING "MPI Print RANK")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fPIC -std=c99 -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++0x -Wall -Wextra -Werror -pedantic")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ") # Not present in Cmake 2.8.11.2
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ") # Not present in Cmake 2.8.11.2
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-variable -save-temps -fsource-asm -w3 -wd981")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable -save-temps -fsource-asm -w3 -wd981 -wd383")
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-alias")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-alias")
    endif()
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") 
        message(STATUS "NOTE: by default, we only set the -save-temps compiler flag with Intel compilers right now. You may want to use CC=\"ccache gcc\" etc to speed up the compiliation69.")
        if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -fstrict-aliasing")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -fstrict-aliasing")
        endif()
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND NOT "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.8"))
            set(SANITIZER "" CACHE STRING "What should be sanitized by Clang/GCC for a build? (address, thread, memory, undefined or leave empty)")
            if (NOT "${SANITIZER}" STREQUAL "")
                string(TOLOWER "${SANITIZER}" SANITIZER)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=${SANITIZER} -fno-omit-frame-pointer")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=${SANITIZER} -fno-omit-frame-pointer")
                if (GHOST_HAVE_CUDA)
                    message(WARNING "Sanitizers are probably not working together with CUDA!")
                endif()
            endif()
        endif()
    endif()
endif()

configure_file (
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/config.h"
)

add_custom_command(
    OUTPUT SeasiteStencil
    COMMAND cmake -E echo
    )

add_custom_target(updateStencil ALL
    DEPENDS
    SeasiteStencil
    )

add_custom_command(TARGET updateStencil
    POST_BUILD
    COMMAND ${TOOL_DIR}/updateStencil.sh ${SRC_DIR} ${YASK_ABSOLUTE_PATH}
    )

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(yaskSite ${SOURCES})

if(ENABLE_LOOPADAPT)
    message(STATUS "check ${LOOPADAPT_LIB_DIR}")
    target_link_libraries(yaskSite "${LOOPADAPT_LIB_DIR}")
    include_directories(SYSTEM "${LOOPADAPT_INCLUDE_DIR}")
    add_dependencies(yaskSite loop_adapt)
endif()

install(FILES ${HEADERS} DESTINATION include/yaskSite/)

if (${BUILD_SHARED_LIBS})
    install(TARGETS yaskSite EXPORT yaskSite-targets LIBRARY DESTINATION lib/yaskSite)
else ()
    install(TARGETS yaskSite EXPORT yaskSite-targets ARCHIVE DESTINATION lib/yaskSite)
endif()

