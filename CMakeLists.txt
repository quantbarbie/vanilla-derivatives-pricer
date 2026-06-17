cmake_minimum_required(VERSION 3.10)
project(pricer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(pricer
    main.cpp
    Date.cpp
    Market.cpp
    RateCurve.cpp
    VolCurve.cpp
)

# Copy the trade file next to the binary so ./pricer finds it.
configure_file(${CMAKE_SOURCE_DIR}/trades.csv ${CMAKE_BINARY_DIR}/trades.csv COPYONLY)
