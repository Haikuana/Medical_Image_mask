cmake_minimum_required(VERSION 3.3...3.12 FATAL_ERROR)
foreach(p
    CMP0071 # 3.10: Let AUTOMOC and AUTOUIC process GENERATED files
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()

project(ImageMask)

find_package(OpenCV REQUIRED)
if(CMAKE_VERSION VERSION_LESS "2.8.11")
  # Add OpenCV headers location to your include paths
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if("${VTK_QT_VERSION}" STREQUAL "")
  message(FATAL_ERROR "VTK was not built with Qt")
endif()

# Use the include path and library for Qt that is used by VTK.
include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

include(opengl.cmake)

#openmp
find_package(OpenMP)

include_directories(./QGLViewer)
link_directories(./QGLViewer)

# Set your files and resources here
set( Srcs src/main.cpp src/allmask_window.cpp src/auxfun.cpp
	src/every5_window.cpp src/ground_window.cpp src/mainwindow.cpp src/markingprocess.cpp
	src/operation_window.cpp src/viewer.cpp src/maskvoxels_window.cpp)

set( Hdrs src/datatype.h src/auxfun.h src/viewer.h src/operation_window.h src/markingprocess.h src/mainwindow.h
 src/ground_window.h src/every5_window.h src/allmask_window.h src/maskvoxels_window.h)

set( MOC_Hdrs src/viewer.h src/operation_window.h src/markingprocess.h src/mainwindow.h
 src/ground_window.h src/every5_window.h src/allmask_window.h src/maskvoxels_window.h)

set( UIs src/mainwindow.ui )

set( QRCs src/mainwindow.qrc )


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(CGAL REQUIRED COMPONENTS Core)
include(${CGAL_USE_FILE})

# The rest should just work (sure...)
# We have ui files, this will bring in the macro: qt5_wrap_ui
find_package(Qt5 COMPONENTS Widgets REQUIRED QUIET)
find_package(Qt5 COMPONENTS Core REQUIRED QUIET)
find_package(Qt5 COMPONENTS OpenGL REQUIRED QUIET)
find_package(Qt5 COMPONENTS Xml REQUIRED QUIET)
qt5_wrap_ui(UI_Srcs ${UIs})
qt5_add_resources(QRC_Srcs ${QRCs} )

source_group("Resources" FILES
  ${UIs}
  ${QRCs}
  ${EXE_ICON} # Not present
)

source_group("Generated" FILES
  ${UI_Srcs}
  ${MOC_Srcs}
  ${QRC_Srcs}
)

add_executable(ImageMask WIN32 
  ${Srcs} ${Hdrs} ${UI_Srcs} ${MOC_Hdrs} ${QRC_Srcs} src/exeicon.rc)
target_link_libraries(ImageMask Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Xml)
target_link_libraries(ImageMask ${VTK_LIBRARIES})
target_link_libraries(ImageMask ${OpenCV_LIBS})
target_link_libraries(ImageMask CGAL::CGAL CGAL::CGAL_Core)
target_link_libraries(ImageMask debug QGLViewerd2 optimized QGLViewer2)
target_link_libraries (ImageMask opengl32)
if(OpenMP_CXX_FOUND)
    target_link_libraries(ImageMask OpenMP::OpenMP_CXX)
endif()
