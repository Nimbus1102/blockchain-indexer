cmake_minimum_required(VERSION 3.10)
project(BlockchainIndexer
    LANGUAGES C CXX
)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 14 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use -std=cxx/-std=c++xx instead of -std=gxx/-std=g++xx
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Place all binaries into a single directory for easy deployment.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(UNIX)
    add_definitions(-DLINUX_OS)
endIf()

include(CMakeDependentOption)

find_package(Threads REQUIRED)
find_package(leveldb REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS
    filesystem
    thread
    serialization
)

add_executable(blockchain_indexer
    src/BlockchainIndexer.cpp
    src/BlockchainIndexer.h
    src/BlockchainIndexerMain.cpp
    src/BlockchainReader.cpp
    src/BlockchainReader.h
    src/BlockchainTypes.h
    src/BlockchainUtils.cpp
    src/BlockchainUtils.h
    src/CacheDatabase.cpp
    src/CacheDatabase.h
    src/ConfigFileReader.cpp
    src/ConfigFileReader.h
    src/ProjectConfig.h
    src/SimpleMiddleware.cpp
    src/SimpleMiddleware.h
    src/TestIndexer.cpp
    src/TestIndexer.h
)

target_link_libraries(blockchain_indexer
    Threads::Threads
    leveldb::leveldb
    nlohmann_json::nlohmann_json
)