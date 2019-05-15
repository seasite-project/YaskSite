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
    src/codeGen.cpp
    src/types.cpp
    src/perfModel.cpp
    )

set (HEADERS
    include/yaskSite.h
    include/config.h
    include/macros.h
    include/yaskFork.h
    include/util.h
    include/MPI_Manager.h
    include/print.h
    include/codeGen.h
    include/types.h
    include/perfModel.h
    )

set(LOG_SYS_CALL 0 CACHE BOOL "Log system calls (default: disabled)")

#set(YASK_ABSOLUTE_PATH "not set" CACHE PATH "Set the absolute path to Intel YASK folder")
set(YASK_ABSOLUTE_PATH "${PROJECT_SOURCE_DIR}/yask") # currently I need a patched version of YASK
set(TOOL_DIR "${PROJECT_SOURCE_DIR}/src/tools")
set(LIB_TOOL_DIR "${CMAKE_INSTALL_PREFIX}/tools/yaskSite/tools")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(STENCIL_DIR "${PROJECT_SOURCE_DIR}/src/stencils")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")
set(CACHE_BUILD OFF CACHE BOOL "Cache the stencil builds for later runs. Enabling cache requires approx. 50 MB diskspace per stencil")
set(USE_LIKWID ON CACHE BOOL "Set the variable to ON if you need to use LIKWID, currently you have to ON it")
set(IACA_PATH "" CACHE PATH "Set the path of iaca.sh file, if not provided will try to download and install Intel IACA")
set(MLC_PATH "" CACHE PATH "Set the path of mlc file, if not provided will try to download and install Intel MLC")
set(LD_PORTS "2:D;3:D" CACHE PATH "Set the LOAD ports of the machine; (Intel-default)")
set(MODEL_BOUNDARY_EFFECT 1 CACHE BOOL "Considers effect from boundary when modelling")
set(MODEL_PREFETCH_EFFECT 1 CACHE BOOL "Considers effect from prefetching when modelling")
set(MODEL_LATENCY_EFFECT 1 CACHE BOOL "Considers effect from latency when modelling")
set(MODEL_ASSOCIATIVITY_EFFECT 1 CACHE BOOL "Uses k-way associative cache model instead of perfect LRU")
set(MODEL_REMAINDER_EFFECT 1 CACHE BOOL "Considers effect from remainder blocks when modelling")


if (${USE_LIKWID})
    set(LIKWID_INC_DIR_INIT "")
    if("$ENV{LIKWID_LIBDIR}" STREQUAL "")
        find_library(LIKWID_LIBRARIES likwid ENV LIBRARY_PATH)
    else()
        find_library(LIKWID_LIBRARIES likwid ENV LIKWID_LIBDIR)
    endif()
    if("${LIKWID_LIBRARIES}" STREQUAL "LIKWID_LIBRARIES-NOTFOUND")
        message(STATUS "LIKWID library not found please set manually")
    else()
        message(STATUS "Found LIKWID library: ${LIKWID_LIBRARIES}")
        get_filename_component(LIKWID_INC_DIR_INIT ${LIKWID_LIBRARIES}/../../include REALPATH)
    endif()
    find_path(LIKWID_INCLUDE_DIR likwid.h PATHS "${LIKWID_INC_DIR_INIT}" ENV CPATH)
    #    set(LIKWID_INCLUDE_DIR "${LIKWID_INC_DIR_INIT}" CACHE PATH "Location of likwid.h")
    if(NOT LIKWID_LIBRARIES)
        get_filename_component(LIKWID_LIBDIR ${LIKWID_INCLUDE_DIR}/../lib REALPATH)
        find_library(LIKWID_LIBRARIES likwid ${LIKWID_LIBDIR})
    endif()
    include_directories(${LIKWID_INCLUDE_DIR})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLIKWID_PERFMON")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIKWID_PERFMON")
endif()

set(TEMP_DIR "~/tmp_YaskSite" CACHE PATH "This is a temporary scratch and caching directory, Please ensure the directory is readable and writable")
file(MAKE_DIRECTORY ${TEMP_DIR})
set(TEMP_STENCIL_DIR "${TEMP_DIR}/stencils")

message(STATUS "Detecting cache sizes")
#configure cache sizes
execute_process(COMMAND ${TOOL_DIR}/getCacheInfo.sh "L1d cache" OUTPUT_VARIABLE L1_val OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${TOOL_DIR}/getCacheInfo.sh "L2 cache" OUTPUT_VARIABLE L2_val OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${TOOL_DIR}/getCacheInfo.sh "L3 cache" OUTPUT_VARIABLE L3_val OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${TOOL_DIR}/getMemInfo.sh OUTPUT_VARIABLE MEM_val OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${TOOL_DIR}/threadPerSocket.sh OUTPUT_VARIABLE thread_per_socket_val OUTPUT_STRIP_TRAILING_WHITESPACE)


set(L1_val_type "${L1_val}; private")
set(L2_val_type "${L2_val}; private")
set(L3_val_type "${L3_val}; shared")
set(MEM_val_type "${MEM_val}; shared")


set(L1 "${L1_val_type}" CACHE STRING "L1 cache size")
set(L2 "${L2_val_type}" CACHE STRING "L2 cache size")
set(L3 "${L3_val_type}" CACHE STRING "L3 cache size")
set(MEM "${MEM_val_type}" CACHE STRING "MEM size")
set(THREAD_PER_SOCKET "${thread_per_socket_val}" CACHE STRING "Threads per socket")

set(CL_SIZE "8")


AutodetectHostArchitecture()
#OptimizeForArchitecture()
message(STATUS "Target Architecture detected is ${TARGET_ARCHITECTURE}")

#convert to YASK codes
if(TARGET_ARCHITECTURE STREQUAL "knl")
    set(TARGET_ARCH "knl")  # Knights Landing
elseif(TARGET_ARCHITECTURE STREQUAL "skylake-avx512")
    set(TARGET_ARCH "skx")  # SKYLAKE
elseif(TARGET_ARCHITECTURE STREQUAL "skylake")
    set(TARGET_ARCH "hsw")  # SKYLAKE-256 or HASWELL
elseif(TARGET_ARCHITECTURE STREQUAL "haswell" OR TARGET_ARCHITECTURE STREQUAL "broadwell")
    set(TARGET_ARCH "hsw")  # BROADWELL
elseif(TARGET_ARCHITECTURE STREQUAL "ivy-bridge")
    set(TARGET_ARCH "ivb")  # IVY-BRIDGE
elseif(TARGET_ARCHITECTURE STREQUAL "sandy-bridge")
    set(TARGET_ARCH "snb")
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
    execute_process(COMMAND which mpicc OUTPUT_VARIABLE MPI_C_COMPILER OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND which mpicxx OUTPUT_VARIABLE MPI_CXX_COMPILER OUTPUT_STRIP_TRAILING_WHITESPACE)
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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fPIC -std=c99 -Wall -Wextra -Werror -pedantic -D__PURE_INTEL_C99_HEADERS__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++0x -Wall -Wextra -Werror -pedantic -D__PURE_INTEL_C99_HEADERS__")
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

if("${IACA_PATH}" STREQUAL "")
    set(BUILTIN_IACA 0 CACHE INTERNAL "am i using built-in iaca")
endif()

#Try if IACA_PATH is working
execute_process(COMMAND ${IACA_PATH}/iaca.sh OUTPUT_VARIABLE iaca_out OUTPUT_STRIP_TRAILING_WHITESPACE)
if ("${iaca_out}" STREQUAL "")
    execute_process(COMMAND mkdir -p ${LIB_TOOL_DIR}/licences)
    execute_process(COMMAND ${TOOL_DIR}/lic_agree.sh "Intel IACA" "https://software.intel.com/en-us/articles/intel-architecture-code-analyzer" ${LIB_TOOL_DIR}/licences/iaca_lic_agree.txt)
    file(READ ${LIB_TOOL_DIR}/licences/iaca_lic_agree.txt iaca_agree)

    if ( "${iaca_agree}" MATCHES ".*1.*" AND ${BUILTIN_IACA} EQUAL 0)
        message(STATUS "Could not find iaca.sh. Trying to download IACA...")
        execute_process(COMMAND mkdir -p ${LIB_TOOL_DIR})
        #TODO: Download from site and make available
        execute_process(COMMAND cp -r ${TOOL_DIR}/IACA ${LIB_TOOL_DIR}/.)
        #extract iaca
        execute_process(COMMAND unzip -q -o ${LIB_TOOL_DIR}/IACA/iaca-version-2.3-lin64.zip -d ${LIB_TOOL_DIR}/IACA)
        #execute_process(COMMAND lynx -cmd_script=${TOOL_DIR}/iaca_download_step.lynx -accept_all_cookies=1 https://software.intel.com/protected-download/267266/157552
        #    WORKING_DIRECTORY ${LIB_TOOL_DIR}/IACA )
        #execute_process(COMMAND unzip iaca-version-2.3-lin64.zip)
        #not working with .zip files
        #ExternalProject_Add(IACA
        #    PREFIX ${LIB_TOOL_DIR}/IACA
        #    URL https://software.intel.com/system/files/managed/d4/c3/iaca-version-2.3-lin64.zip
            #SOURCE_DIR ${PROJECT_BINARY_DIR}/IACA/src/hwloc
            #CONFIGURE_COMMAND ./configure --enable-static --disable-shared --prefix=${PROJECT_BINARY_DIR}/hwloc-inst
            #CONFIGURE_COMMAND CXXFLAGS=-fPIC CFLAGS=-fPIC ./configure --prefix=${PROJECT_BINARY_DIR}/hwloc-inst
            #BUILD_COMMAND make
            #BUILD_IN_SOURCE 1
            #INSTALL_COMMAND make install
        #)


        set(BUILTIN_IACA 1 CACHE INTERNAL "am i using built-in iaca")
        set(IACA_PATH "${LIB_TOOL_DIR}/IACA/iaca-lin64/bin" CACHE INTERNAL "Set the path of iaca.sh file, if not provided will try to download and install Intel IACA")

        #test whether working
        execute_process(COMMAND ${IACA_PATH}/iaca.sh OUTPUT_VARIABLE iaca_out OUTPUT_STRIP_TRAILING_WHITESPACE)
        if ("${iaca_out}" STREQUAL "")
            message(STATUS "Error in downloading and installing IACA please set IACA_PATH manually")
        endif()

    endif()
endif()

if("${MLC_PATH}" STREQUAL "")
    set(BUILTIN_MLC 0 CACHE INTERNAL "am i using built-in mlc")
endif()

#Now try if Intel MLC is working
execute_process(COMMAND ${MLC_PATH}/mlc -h ERROR_VARIABLE mlc_err OUTPUT_VARIABLE mlc_out OUTPUT_STRIP_TRAILING_WHITESPACE)
if ("${mlc_out}" STREQUAL "")
    execute_process(COMMAND mkdir -p ${LIB_TOOL_DIR}/licences)
    execute_process(COMMAND ${TOOL_DIR}/lic_agree.sh "Intel MLC" "https://software.intel.com/en-us/articles/intelr-memory-latency-checker" ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt)
    file(READ ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt mlc_agree)

    if ( "${mlc_agree}" MATCHES ".*1.*" AND ${BUILTIN_MLC} EQUAL 0)
        message(STATUS "Could not find mlc. Trying to download Intel MLC...")
        execute_process(COMMAND mkdir -p ${LIB_TOOL_DIR}/MLC)
        #TODO: Download from site and make available
        execute_process(COMMAND cp -r ${TOOL_DIR}/latency_bench/mlc ${LIB_TOOL_DIR}/MLC/.)

        set(BUILTIN_MLC 1 CACHE INTERNAL "am i using built-in mlc")
        set(MLC_PATH "${LIB_TOOL_DIR}/MLC/mlc" CACHE INTERNAL "Set the path of mlc file, if not provided will try to download and install Intel MLC")
        #test whether working
        execute_process(COMMAND ${MLC_PATH}/mlc -h  ERROR_VARIABLE mlc_err OUTPUT_VARIABLE mlc_out OUTPUT_STRIP_TRAILING_WHITESPACE)
        if ("${mlc_out}" STREQUAL "")
            message(STATUS "Error in downloading and installing Intel MLC please set MLC_PATH manually")
        endif()
    endif()
endif()


add_custom_command(
    OUTPUT SeasiteStencil
    COMMAND cmake -E echo
    )

add_custom_target(updateStencil ALL
    DEPENDS
    SeasiteStencil
    )

#1. create temp folder
#2. copy existing stencils to temp_folder
#2. update stencils
add_custom_command(TARGET updateStencil
    POST_BUILD
    COMMAND mkdir -p ${TEMP_DIR}/stencils && cp -r ${STENCIL_DIR}/*.hpp "${TEMP_DIR}/stencils/." && ${TOOL_DIR}/updateStencil.sh ${TEMP_STENCIL_DIR} ${YASK_ABSOLUTE_PATH} ${TEMP_DIR}
    )


add_custom_command(
    OUTPUT MachineCalibration
    COMMAND cmake -E echo
    )


add_custom_target(calibrate
    DEPENDS
    MachineCalibration
    )

list(GET L1 1 L1_type)
list(GET L2 1 L2_type)
list(GET L3 1 L3_type)
list(GET MEM 1 MEM_type)

if(L1_type MATCHES ".*shared.*")
    math(EXPR L1_private "${L1_val}/${THREAD_PER_SOCKET}")
else()
    math(EXPR L1_private "${L1_val}")
endif()

if(L2_type MATCHES ".*shared.*")
    math(EXPR L2_private "${L2_val}/${THREAD_PER_SOCKET}")
else()
    math(EXPR L2_private "${L2_val}")
endif()

if(NOT ${L3_val} STREQUAL "")
    if( L3_type MATCHES ".*shared.*")
        math(EXPR L3_private "${L3_val}/${THREAD_PER_SOCKET}")
    else()
        math(EXPR L3_private "${L3_val}")
    endif()
endif()

if(MEM_type MATCHES ".*shared.*")
    math(EXPR MEM_private "${MEM_val}/${THREAD_PER_SOCKET}")
else()
    math(EXPR MEM_private "${MEM_val}")
endif()


if(NOT ${L3_val} STREQUAL "")
    math(EXPR mem_w_sf "${L3_val}/1024 + 10*1024")
else()
    math(EXPR mem_w_sf "${L2_val}/1024 + 10*1024")
endif()

#in between L3 and L2
if(NOT ${L3_val} STREQUAL "")
    math(EXPR l3_w_sf "(${L2_private}+${L3_private})/2048")
endif()
#in between L2 and L1
math(EXPR l2_w_sf "(${L1_private}+${L2_private})/2048")
#in L1
math(EXPR l1_w_sf "(${L1_private})/2048")

ExternalProject_Add(LDST-bench
    PREFIX ${PROJECT_BINARY_DIR}/LDST-bench
    URL https://github.com/christiealappatt/LDST-bench/zipball/master
    DOWNLOAD_NAME LDST-bench.zip
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

get_filename_component(CMAKE_CXX_COMPILER_SHORT ${CMAKE_CXX_COMPILER} NAME)

if(NOT ${L3_val} STREQUAL "")
    add_custom_command(TARGET calibrate
        POST_BUILD
        COMMAND mkdir -p ${TEMP_DIR}/bench_results && ${LIB_TOOL_DIR}/bw_bench/bench_scan.sh ${PROJECT_BINARY_DIR}/LDST-bench/src/LDST-bench ${CMAKE_CXX_COMPILER_SHORT} ${LIB_TOOL_DIR} 1 10 200 ${TEMP_DIR}/bench_results/bw_load_ratio.txt
        COMMAND mkdir -p ${TEMP_DIR}/bench_results/latency
        COMMAND cp -r ${TOOL_DIR}/latency_bench/wrapper/* ${LIB_TOOL_DIR}/MLC/mlc/. && cp ${TOOL_DIR}/latency_bench/latency_scan.sh ${LIB_TOOL_DIR}/MLC/.
        COMMAND ${LIB_TOOL_DIR}/MLC/latency_scan.sh ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt ${mem_w_sf} -1 ${MEM_type} ${TEMP_DIR}/bench_results/latency/MEM.txt
        COMMAND ${LIB_TOOL_DIR}/MLC/latency_scan.sh ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt ${l3_w_sf} -1 ${L3_type} ${TEMP_DIR}/bench_results/latency/L3.txt
        COMMAND ${LIB_TOOL_DIR}/MLC/latency_scan.sh ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt ${l2_w_sf} -1 ${L2_type} ${TEMP_DIR}/bench_results/latency/L2.txt
        COMMAND ${LIB_TOOL_DIR}/MLC/latency_scan.sh ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt ${l1_w_sf} -1 ${L1_type} ${TEMP_DIR}/bench_results/latency/L1.txt
    )
else()
    add_custom_command(TARGET calibrate
        POST_BUILD
        #       COMMAND mkdir -p ${TEMP_DIR}/bench_results && ${LIB_TOOL_DIR}/bw_bench/bench_scan.sh  ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results 1 10 100 ${TEMP_DIR}/bench_results/bw_load_ratio.txt
        COMMAND mkdir -p ${TEMP_DIR}/bench_results && ${LIB_TOOL_DIR}/bw_bench/bench_scan.sh ${PROJECT_BINARY_DIR}/LDST-bench/src/LDST-bench ${CMAKE_CXX_COMPILER_SHORT} ${LIB_TOOL_DIR} 1 10 300 ${TEMP_DIR}/bench_results/bw_load_ratio.txt
        COMMAND mkdir -p ${TEMP_DIR}/bench_results/latency
        COMMAND cp -r ${TOOL_DIR}/latency_bench/wrapper/* ${LIB_TOOL_DIR}/MLC/mlc/. && cp ${TOOL_DIR}/latency_bench/latency_scan.sh ${LIB_TOOL_DIR}/MLC/.
        COMMAND ${LIB_TOOL_DIR}/MLC/latency_scan.sh ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt ${mem_w_sf} -1 ${MEM_type} ${TEMP_DIR}/bench_results/latency/MEM.txt
        COMMAND ${LIB_TOOL_DIR}/MLC/latency_scan.sh ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt ${l2_w_sf} -1 ${L2_type} ${TEMP_DIR}/bench_results/latency/L2.txt
        COMMAND ${LIB_TOOL_DIR}/MLC/latency_scan.sh ${LIB_TOOL_DIR} ${TEMP_DIR}/bench_results ${LIB_TOOL_DIR}/licences/mlc_lic_agree.txt ${l1_w_sf} -1 ${L1_type} ${TEMP_DIR}/bench_results/latency/L1.txt
        )
endif()

add_dependencies(calibrate LDST-bench)

if("${L3_val}" STREQUAL "")
    set(L3 "${L2_val_type}")
    message(STATUS "L3 = ${L3}")
endif()

configure_file (
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/config.h"
    )

configure_file(${PROJECT_SOURCE_DIR}/yasksite-config.cmake.in ${PROJECT_BINARY_DIR}/yasksite-config.cmake @ONLY)


include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(yaskSite ${SOURCES})

if(ENABLE_LOOPADAPT)
    message(STATUS "check ${LOOPADAPT_LIB_DIR}")
    target_link_libraries(yaskSite "${LOOPADAPT_LIB_DIR}")
    include_directories(SYSTEM "${LOOPADAPT_INCLUDE_DIR}")
    add_dependencies(yaskSite loop_adapt)
endif()

install(FILES ${PROJECT_BINARY_DIR}/yasksite-config.cmake DESTINATION lib/yaskSite)

install(FILES ${HEADERS} DESTINATION include/yaskSite/)

#copy tools folder also
install(DIRECTORY ${TOOL_DIR}
    FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE
    DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE
    DESTINATION tools/yaskSite)

if (${BUILD_SHARED_LIBS})
    install(TARGETS yaskSite EXPORT yaskSite-targets LIBRARY DESTINATION lib/yaskSite)
else ()
    install(TARGETS yaskSite EXPORT yaskSite-targets ARCHIVE DESTINATION lib/yaskSite)
endif()

