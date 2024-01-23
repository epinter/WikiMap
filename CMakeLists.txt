cmake_minimum_required(VERSION 3.25)

project(
        WikiMap
        VERSION 1.0.0
        LANGUAGES CXX)
set(PROJECT_LICENSE "LGPLv3")
set(PROJECT_FRIENDLY_NAME ${PROJECT_NAME})
set(PROJECT_LONG_NAME "Wiki and Map")
set(PROJECT_DESCRIPTION ${PROJECT_LONG_NAME})
set(PROJECT_COPYRIGHT "Emerson Pinter")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(addpluginsources)
include(pluginconfig)
include(plugintarget)
include(pluginpackage)
include("${PROJECT_SOURCE_DIR}/local.cmake" OPTIONAL)

######## dependencies
set(Boost_NO_WARN_NEW_VERSIONS 1)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED  ON)
find_package(Boost REQUIRED COMPONENTS url)
if(Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
endif()


