cmake_minimum_required(VERSION 3.10)
project(blog_api CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

include_directories(src/controllers src/models)

add_executable(blog_api 
    main.cpp 
    src/controllers/Controllers.cpp
)
target_link_libraries(blog_api PRIVATE Drogon::Drogon)
