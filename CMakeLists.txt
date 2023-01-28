cmake_minimum_required(VERSION 3.20.0)
project(checkers-ai)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)

include(GNUInstallDirs)
find_package(wxWidgets REQUIRED)

add_executable(checkers-ai)
# target_link_libraries(checkers-ai PRIVATE wxWidgets)

include(${wxWidgets_USE_FILE})
target_link_libraries(checkers-ai ${wxWidgets_LIBRARIES})

target_include_directories(checkers-ai PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(checkers-ai PRIVATE CMAKE_CURRENT_BINARY_DIR)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(resources)
