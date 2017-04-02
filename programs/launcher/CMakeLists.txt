set(module_id vsxu_launcher)

message("configuring            " ${module_id})

project (${module_id})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets)

include_directories(${Qt5Widgets_INCLUDE_DIRS})

if(VSXU_DEBUG)
  add_definitions(-DDEBUG)
  add_definitions(-DVSXU_DEBUG)
endif(VSXU_DEBUG)

add_definitions(-DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DVSXU_BIN_DIR="${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_BIN_DIR}")

set(SOURCES
  main.cpp
  mainwindow.cpp
)

file(GLOB_RECURSE HEADER_FILES *.h)

set(UI_FILES mainwindow.ui)

qt5_wrap_cpp(MOC_SOURCES ${HEADER_FILES})
qt5_wrap_ui(UI_SOURCES ${UI_FILES})

add_executable(${module_id} ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES})
target_link_libraries(${module_id} Qt5::Widgets)

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT launcher)
