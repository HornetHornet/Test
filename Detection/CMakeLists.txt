cmake_minimum_required(VERSION 3.5)
project(Label-detection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")


#find_package(OpenCV 3 REQUIRED)
find_package(OpenCV 4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Boost COMPONENTS regex thread filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(
		label-detection
		LogoDetection/LogoDetection/Source.cpp
		LogoDetection/LogoDetection/Logger.h
		LogoDetection/LogoDetection/Logger.cpp
		LogoDetection/LogoDetection/GetFiles.h
		LogoDetection/LogoDetection/Geometry.h
		LogoDetection/LogoDetection/GeneralTransforms.h
		LogoDetection/LogoDetection/Detectors.h
		LogoDetection/LogoDetection/Detectors.cpp
)

target_link_libraries(
		label-detection
		${OpenCV_LIBS}
		${Boost_LIBRARIES}
		pthread
		rt
)