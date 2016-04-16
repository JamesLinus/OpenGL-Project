cmake_minimum_required(VERSION 2.8)
project(ImageKit_305)

#set the name of the compiled program
set(TARGET_NAME "ImageDemo")

include(Image/SetEnv.cmake)

#add project source files
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${COMMON_LIBS})

include(Image/PostCommand.cmake)
