SET(QEMU_M55_CMAKE_ASM_FLAGS "-mcpu=cortex-m55")
SET(QEMU_M55_CMAKE_C_FLAGS "-mcpu=cortex-m55")
SET(QEMU_M55_CMAKE_CXX_FLAGS "-mcpu=cortex-m55")
SET(QEMU_M55_CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m55")

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    list(APPEND QEMU_M55_CMAKE_ASM_FLAGS "-Wno-sign-conversion;-Wno-aggregate-return;-Wno-conversion")
    list(APPEND QEMU_M55_CMAKE_C_FLAGS "-Wno-sign-conversion;-Wno-aggregate-return;-Wno-conversion")
    list(APPEND QEMU_M55_CMAKE_CXX_FLAGS "-Wno-sign-conversion;-Wno-aggregate-return;-Wno-conversion")
    list(APPEND QEMU_M55_CMAKE_EXE_LINKER_FLAGS "-T;RTE/Device/SSE-300-MPS3/linker_SSE300MPS3_secure.ld;--specs=nosys.specs")
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    list(APPEND QEMU_M55_CMAKE_EXE_LINKER_FLAGS "-T;RTE/Device/SSE-300-MPS3/linker_SSE300MPS3_secure.lld")
endif()

SET(QEMU_M55_CMAKE_DEFINITIONS "QEMU_M55;_RTE_")

# source directories
file(GLOB_RECURSE HAL_Files
    ${CMAKE_CURRENT_LIST_DIR}/Board/*.c
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/*.c
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS_Driver/*.c
    ${CMAKE_CURRENT_LIST_DIR}/Device/*.c
    ${CMAKE_CURRENT_LIST_DIR}/RTE/*.c
    ${CMAKE_CURRENT_LIST_DIR}/retarget.c
)

target_sources(hal_lib PRIVATE ${HAL_Files})

target_compile_options(hal_lib
    PUBLIC
    $<$<CONFIG:Debug>:${QEMU_M55_DEBUG_FLAGS}>
    $<$<CONFIG:Release>:${QEMU_M55_RELEASE_FLAGS}>
    $<$<CONFIG:MinSizeRel>:${QEMU_M55_MIN_SIZE_RELEASE_FLAGS}>
    $<$<CONFIG:RelWithDebInfo>:${QEMU_M55_RELEASE_WITH_DEBUG_INFO}>)

target_compile_options(hal_lib PUBLIC $<$<COMPILE_LANGUAGE:ASM>:${QEMU_M55_CMAKE_ASM_FLAGS}>)
target_compile_options(hal_lib PUBLIC $<$<COMPILE_LANGUAGE:C>:${QEMU_M55_CMAKE_C_FLAGS}>)
target_compile_options(hal_lib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${QEMU_M55_CMAKE_CXX_FLAGS}>)

target_link_options(hal_lib PUBLIC ${QEMU_M55_CMAKE_EXE_LINKER_FLAGS})

target_compile_definitions(hal_lib PUBLIC ${QEMU_M55_CMAKE_DEFINITIONS})

target_include_directories(hal_lib
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Device/Include
    ${CMAKE_CURRENT_LIST_DIR}/Device/Config
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Core/Include
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Driver/Include
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS_Driver/Config
    ${CMAKE_CURRENT_LIST_DIR}/Board/Device_Definition
    ${CMAKE_CURRENT_LIST_DIR}/Board/Platform
    ${CMAKE_CURRENT_LIST_DIR}/RTE
)

target_link_directories(hal_lib
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
