cmake_minimum_required(VERSION 3.20)
project(Test C ASM_NASM)

enable_language(ASM_NASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_custom_target(codestyle
    COMMAND ./apps/ci/ci-codestyle.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running codestyle checks"
)

add_executable(${CMAKE_PROJECT_NAME}
    src/main.c
    asm/_start.asm
    asm/sys_strcpy.asm
    asm/sys_strlen.asm
    asm/sys_timestamp.asm
    asm/sys_write.asm
)

add_dependencies(${CMAKE_PROJECT_NAME} codestyle)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(${CMAKE_PROJECT_NAME}  PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -ffreestanding
        -fno-builtin
        -fno-stack-protector
        -Wall
        -Wextra
        -O2
        -g
    >
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    -static
    -Wl,-e,_start
    -Wl,--build-id=none
)
