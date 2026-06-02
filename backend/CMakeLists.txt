cmake_minimum_required(VERSION 3.16)
project(smartcampus VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SOURCES
    "src/persistence/*.cpp"
    "src/application/*.cpp"
    "src/presentation/*.cpp"
    "src/main.cpp"
)

add_executable(smartcampus ${SOURCES})

target_include_directories(smartcampus PRIVATE
    src/
    /usr/local/include
    /usr/include/nlohmann
)

target_link_libraries(smartcampus PRIVATE
    pqxx pq
    OpenSSL::SSL OpenSSL::Crypto
    pthread
)
