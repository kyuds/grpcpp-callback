cmake_minimum_required(VERSION 3.15)

# Basic setup

project(
    grpcpp-callback 
    VERSION 0.1
    DESCRIPTION "grpc callback api"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# The following cmake file will generate code based on
# proto files under src/generated. All files are created
# under the single directory.

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)

# gRPC configurations
set(_LIBPROTOBUF libprotobuf)
set(_GRPC_GRPC gRPC::grpc)
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_REFLECTION gRPC::grpc++_reflection)

# finding compiler
get_target_property(
    _GRPC_CPP_PLUGIN_EXECUTABLE
    gRPC::grpc_cpp_plugin LOCATION
)
find_program(_PROTOC protoc)

add_custom_command(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/service.pb.h"
        "${CMAKE_CURRENT_BINARY_DIR}/service.pb.cc"
        "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h"
        "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc"
    COMMAND ${_PROTOC}
    ARGS
        --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${CMAKE_SOURCE_DIR}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${CMAKE_SOURCE_DIR}/service.proto"
    DEPENDS "${CMAKE_SOURCE_DIR}/service.proto")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(grpc_proto
    ${CMAKE_CURRENT_BINARY_DIR}/service.pb.h
    ${CMAKE_CURRENT_BINARY_DIR}/service.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h
    ${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc
)

target_link_libraries(grpc_proto
    ${_GRPC_GRPC}
    ${_GRPC_GRPCPP}
    ${_GRPC_REFLECTION}
    ${_PROTOBUF_LIBPROTOBUF}
)

add_executable(client
    client.cc
)

add_executable(server
    server.cc
)

target_include_directories(client PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_include_directories(server PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(client
    PUBLIC
        grpc_proto
)

target_link_libraries(server
    PUBLIC
        grpc_proto
)

