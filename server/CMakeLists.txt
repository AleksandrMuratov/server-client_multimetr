cmake_minimum_required(VERSION 3.5)

project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    main.cpp
    multimetr.h
    multimetr.cpp
    test_runner.h
    server_multimetr.h
    server_multimetr.cpp
    parsing_requests_to_multimetr.h
    parsing_requests_to_multimetr.cpp)
