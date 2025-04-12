cmake_minimum_required(VERSION 3.10)
project(ClothSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

add_executable(ClothSimulation
    main.cpp
    Cloth.cpp
    Cloth.h
)

target_link_libraries(ClothSimulation
    gdi32
    user32
) 