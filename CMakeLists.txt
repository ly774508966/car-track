cmake_minimum_required (VERSION 2.8.11)
set(PROJECT_EXE car-track)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLOG REQUIRED)
find_package(X11 REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(
    ${GLM_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${OpenGL_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${SDL_MIXER_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIRS}
)

include_directories(include)

file(GLOB_RECURSE SRC_FILES src/*.cpp include/*.hpp include/*.h)
add_executable(${PROJECT_EXE} ${SRC_FILES})

add_subdirectory(lib)

target_link_libraries(
    ${PROJECT_EXE}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY}
    ${GLOG_LIBRARIES}
    ${OpenCV_LIBS}
    ${X11_LIBRARIES}
    ${GLFW_STATIC_LIBRARIES}
    IMGUI_LIBRARY
    ${SDL_MIXER_LIBRARIES}
    ${SDL_LIBRARY}
)

message(WARNING ${SDL_MIXER_LIBRARIES})

FOREACH(item ${GLFW_STATIC_LIBRARIES})
    MESSAGE(STATUS "  using lib: " ${item})
ENDFOREACH()
