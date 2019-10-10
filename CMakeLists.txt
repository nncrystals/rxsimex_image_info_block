cmake_minimum_required(VERSION 3.15)
project(rxsimex_image_info_block)

set(CMAKE_CXX_STANDARD 17)

include(RxSimex/CMakeLists.txt)
set(SOURCES image_info_block.cpp)
register_target(image_info_block ${SOURCES} "" "")