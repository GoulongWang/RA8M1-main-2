# Please see README.md for build instructions

cmake_minimum_required(VERSION 3.16.4)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo MinSizeRel)

cmake_minimum_required(VERSION 3.16.4)

# Project configuration
project(Helium
	VERSION 1.0.0
	LANGUAGES C CXX ASM)

set(TARGET "RA8M1" CACHE STRING "Target: either QEMU_M55 or RA8M1")
set(CMAKE_CROSSCOMPILING "TRUE")
set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE")

if(${TARGET} STREQUAL "QEMU_M55")
	message(STATUS "Building for QEMU_M55")
	set(TEST_ELF "test_qemu_m55.elf")
	set(BENCH_ELF "bench_qemu_m55.elf")
elseif(${TARGET} STREQUAL "RA8M1")
	message(STATUS "Building for RA8M1")
	set(TEST_ELF "test_ra8m1.elf")
	set(BENCH_ELF "bench_ra8m1.elf")
else()
	message(FATAL_ERROR "Invalid target: ${TARGET}")
endif()

SET(COMMON_CMAKE_ASM_FLAGS "-mfloat-abi=hard;-Wunused;-Wuninitialized;-Wall;-Wextra;-Wno-missing-declarations;-Wconversion;-Wpointer-arith;-Wshadow;-Waggregate-return;-Wno-float-equal;-fmessage-length=0;-fsigned-char;-ffunction-sections;-fdata-sections;-mthumb;-x;assembler-with-cpp;-MP")
SET(COMMON_CMAKE_C_FLAGS "-mfloat-abi=hard;-Wunused;-Wuninitialized;-Wall;-Wextra;-Wno-missing-declarations;-Wconversion;-Wpointer-arith;-Wshadow;-Waggregate-return;-Wno-float-equal;-fmessage-length=0;-fsigned-char;-ffunction-sections;-fdata-sections;-mthumb;-MP")
SET(COMMON_CMAKE_CXX_FLAGS "-mfloat-abi=hard;-Wunused;-Wuninitialized;-Wall;-Wextra;-Wno-missing-declarations;-Wconversion;-Wpointer-arith;-Wshadow;-Waggregate-return;-Wno-float-equal;-fmessage-length=0;-fsigned-char;-ffunction-sections;-fdata-sections;-mthumb;-std=c++11;-MP")
SET(COMMON_CMAKE_EXE_LINKER_FLAGS "-mfloat-abi=hard;-Wunused;-Wuninitialized;-Wall;-Wextra;-Wno-missing-declarations;-Wconversion;-Wpointer-arith;-Wshadow;-Waggregate-return;-Wno-float-equal;-fmessage-length=0;-fsigned-char;-ffunction-sections;-fdata-sections;-mthumb;-Wl,--gc-sections;-Wl,-Map,${PROJECT_NAME}.map")

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	list(APPEND COMMON_CMAKE_ASM_FLAGS "-mfpu=auto;-Wlogical-op;-MMD")
	list(APPEND COMMON_CMAKE_C_FLAGS "-mfpu=auto;-Wlogical-op;-MMD")
	list(APPEND COMMON_CMAKE_CXX_FLAGS "-mfpu=auto;-Wlogical-op;-MMD")
	list(APPEND COMMON_CMAKE_EXE_LINKER_FLAGS "-mfpu=auto;-Wlogical-op")
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	list(APPEND COMMON_CMAKE_ASM_FLAGS "--target=arm-none-eabi;-mfpu=fp-armv8-fullfp16-d16;-Wno-parentheses-equality;-fshort-enums;-fno-unroll-loops;-mlittle-endian")
	list(APPEND COMMON_CMAKE_C_FLAGS "--target=arm-none-eabi;-mfpu=fp-armv8-fullfp16-d16;-Wno-parentheses-equality;-fshort-enums;-fno-unroll-loops;-mlittle-endian")
	list(APPEND COMMON_CMAKE_CXX_FLAGS "--target=arm-none-eabi;-mfpu=fp-armv8-fullfp16-d16;-Wno-parentheses-equality;-fshort-enums;-fno-unroll-loops;-mlittle-endian")
	list(APPEND COMMON_CMAKE_EXE_LINKER_FLAGS "--target=arm-none-eabi;-mfpu=fp-armv8-fullfp16-d16;-Wno-parentheses-equality;-fshort-enums;-fno-unroll-loops;-mlittle-endian;-Wl,--cref;-Wl,--icf=none")
endif()

# ADD COMPILE FLAGS FOR GCC version >= 12.2
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 12.2)
	list(INSERT COMMON_CMAKE_C_FLAGS 0 "--param=min-pagesize=0;-Wno-format-truncation;-Wno-stringop-overflow")
	list(INSERT COMMON_CMAKE_CXX_FLAGS 0 "--param=min-pagesize=0;-Wno-format-truncation;-Wno-stringop-overflow")
endif()

SET(COMMON_DEBUG_FLAGS "-g;-Og")
SET(COMMON_RELEASE_FLAGS "-O3")
SET(COMMON_MIN_SIZE_RELEASE_FLAGS "-Os")
SET(COMMON_RELEASE_WITH_DEBUG_INFO "-g;-O2")

file(GLOB_RECURSE Lib_Files
	${CMAKE_CURRENT_LIST_DIR}/lib/*.c
)

add_library(hal_lib OBJECT ${Lib_Files})

target_compile_options(hal_lib
	PUBLIC
	$<$<CONFIG:Debug>:${COMMON_DEBUG_FLAGS}>
	$<$<CONFIG:Release>:${COMMON_RELEASE_FLAGS}>
	$<$<CONFIG:MinSizeRel>:${COMMON_MIN_SIZE_RELEASE_FLAGS}>
	$<$<CONFIG:RelWithDebInfo>:${COMMON_RELEASE_WITH_DEBUG_INFO}>)

target_compile_options(hal_lib PUBLIC $<$<COMPILE_LANGUAGE:ASM>:${COMMON_CMAKE_ASM_FLAGS}>)
target_compile_options(hal_lib PUBLIC $<$<COMPILE_LANGUAGE:C>:${COMMON_CMAKE_C_FLAGS}>)
target_compile_options(hal_lib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${COMMON_CMAKE_CXX_FLAGS}>)

target_link_options(hal_lib PUBLIC ${COMMON_CMAKE_EXE_LINKER_FLAGS})

target_include_directories(hal_lib
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/lib/utils
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/test/unity
)

file(GLOB_RECURSE Source_Files
	${CMAKE_CURRENT_LIST_DIR}/src/*.c
	${CMAKE_CURRENT_LIST_DIR}/src/*.S)

file(GLOB_RECURSE Benchmarks_Files
	${CMAKE_CURRENT_LIST_DIR}/benchmarks/*.c
)

file(GLOB_RECURSE Tests_Files
	${CMAKE_CURRENT_LIST_DIR}/test/*.c
)

add_executable(${BENCH_ELF}
	${Source_Files}
	${Benchmarks_Files}
)
target_link_libraries(${BENCH_ELF} PRIVATE hal_lib)

add_executable(${TEST_ELF}
	${Tests_Files}
	${Source_Files}
)
target_link_libraries(${TEST_ELF} PRIVATE hal_lib)
target_compile_definitions(${TEST_ELF} PRIVATE UNIT_TEST)
target_compile_options(${TEST_ELF} PRIVATE -std=gnu99)

if(${TARGET} STREQUAL "QEMU_M55")
	include(${CMAKE_CURRENT_LIST_DIR}/QEMU_M55/QEMU_M55.cmake)

	include(CTest)
	add_test(NAME test_QEMU_M55 COMMAND test_qemu_m55.elf)
elseif(${TARGET} STREQUAL "RA8M1")
	include(${CMAKE_CURRENT_LIST_DIR}/RA8M1/RA8M1.cmake)
endif()
