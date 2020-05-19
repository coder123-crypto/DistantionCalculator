cmake_minimum_required(VERSION 3.5)

project(DistantionCalculator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(libexif/)
add_subdirectory(libexif/)

add_executable(DistantionCalculator
  main.cpp
  MainWindow.cpp
  MainWindow.h
  DrawingLabel.cpp
  DrawingLabel.h
  ExifHelpers.h
)

target_link_libraries(DistantionCalculator PRIVATE Qt5::Widgets PUBLIC libexif)
