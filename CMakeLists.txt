cmake_minimum_required(VERSION 3.10)

project(task CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(SOURCES
    main.cpp
    computer_club.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})