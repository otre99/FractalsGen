cmake_minimum_required(VERSION 3.5)

project(FractalsGen LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)
if(NOT TARGET OpenMP::OpenMP_CXX)
    find_package(Threads REQUIRED)
    add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
    set_property(TARGET OpenMP::OpenMP_CXX PROPERTY INTERFACE_COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
    # Only works if the same flag is passed to the linker; use CMake 3.9+ otherwise (Intel, AppleClang)
    set_property(TARGET OpenMP::OpenMP_CXX PROPERTY INTERFACE_LINK_LIBRARIES ${OpenMP_CXX_FLAGS} Threads::Threads)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS DataVisualization REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

add_executable(Fractals
    main.cpp
    mainwindow.cpp
    mainwindow.h
    renderthread.cpp
    renderthread.h
    mainwindow.ui
    qcustomplot.cpp
    selectoutputimagesizedialog.cpp
    selectoutputimagesizedialog.ui
    juliatset.cpp
    doubleslider.cpp
    )

target_link_libraries(Fractals PRIVATE Qt5::Widgets Qt5::PrintSupport -pthread PUBLIC OpenMP::OpenMP_CXX)
