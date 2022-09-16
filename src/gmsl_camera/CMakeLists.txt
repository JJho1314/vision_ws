cmake_minimum_required(VERSION 2.4)
project(gmsl_camera)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(LIB_PANO_MAJOR 1)
set(LIB_PANO_MINOR 0)
set(LIB_PANO_PATCH 0)

#add -no-pie to generate executable files, otherwise generate shared library
# set(CMAKE_CXX_FLAGS "-no-pie -g -Wall")
#-------------------------------------------------------------------------------
# CUDA
#-------------------------------------------------------------------------------
find_package(CUDA REQUIRED)
SET(CUDA_SEPARABLE_COMPILATION ON)
message(STATUS "CUDA版本: ${CUDA_VERSION}")
message(STATUS "    头文件目录：${CUDA_INCLUDE_DIRS}")
message(STATUS "    库文件列表：${CUDA_LIBRARIES}")
message(STATUS "  CMAKE_CURRENT_SOURCE_DIR：${CMAKE_CURRENT_SOURCE_DIR}")

set(CUDA_NVCC_FLAGS  ${CUDA_NVCC_FLAGS} --maxrregcount=32; -G;-g;-std=c++11) # nvcc flags

set(CUDIR /usr/src/jetson_multimedia_api/samples/common/algorithm/cuda)
set(COMMONCLS /usr/src/jetson_multimedia_api/samples/common/classes)

# add_subdirectory(cudasrc)
# include_directories(cudasrc)

set(OpenCV_DIR "/usr/local/share/OpenCV/OpenCVConfig.cmake")
find_package(OpenCV 3 REQUIRED )
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)


message(STATUS "OpenCV版本: ${OpenCV_VERSION}")
message(STATUS "    头文件目录：${OpenCV_INCLUDE_DIRS}")
message(STATUS "    库文件目录：${OpenCV_LIB_DIR}")
message(STATUS "    库文件列表：${OpenCV_LIBS}")
message(STATUS "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
message(STATUS "OpenGL lib: ${OPENGL_LIBRARY}")
message(STATUS "GLUT_LIBRARY: ${GLUT_LIBRARY}")

message(STATUS "CUDA_LIBRARIES:::${CUDA_LIBRARIES}")
message(STATUS "CUDA_CUBLAS_LIBRARIES:::${CUDA_CUBLAS_LIBRARIES}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR:::${CMAKE_CURRENT_SOURCE_DIR}")

# add_compile_options(-std=c++14 -g )
add_compile_options(-std=c++14 )
# add_definitions(-DDEBUG)
add_definitions(-DCAM_IMX390)

cuda_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CUDA_INCLUDE_DIRS} )

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  image_transport 
  cv_bridge
  camera_info_manager
  nodelet
)


catkin_package(
  INCLUDE_DIRS include
  LIBRARIES gmsl_camera
  CATKIN_DEPENDS image_transport roscpp cv_bridge sensor_msgs camera_info_manager
  DEPENDS system_lib
)

set(COMMONCLSFILES
    ${COMMONCLS}/NvBuffer.cpp
    ${COMMONCLS}/NvElement.cpp
    ${COMMONCLS}/NvElementProfiler.cpp
    ${COMMONCLS}/NvEglRenderer.cpp
    ${COMMONCLS}/NvLogging.cpp
    ${COMMONCLS}/NvV4l2Element.cpp
    ${COMMONCLS}/NvV4l2ElementPlane.cpp
    ${COMMONCLS}/NvVideoEncoder.cpp
    ${COMMONCLS}/NvVideoDecoder.cpp
    ${COMMONCLS}/NvLogging.cpp)

include_directories(
    ${CUDA_INCLUDE_DIRS}
    include
    include/nvencoder 
    /usr/src/jetson_multimedia_api/include
    /usr/src/jetson_multimedia_api/samples/common/algorithm/cuda/
    ${catkin_INCLUDE_DIRS}
    )

#-------------------------------------------------------------------------------
# Build Node 
#-------------------------------------------------------------------------------


add_executable(${PROJECT_NAME}_node src/master.cpp src/stitcherglobal.cpp  ${COMMONCLSFILES})

target_link_libraries(${PROJECT_NAME}_node  -lyaml-cpp
    -L/usr/lib/aarch64-linux-gnu/tegra/ -lnvbuf_utils  
    -L/usr/lib/aarch64-linux-gnu/ -lv4l2 -lEGL -lGLESv2 -lX11 
    ${CUDA_LIBRARIES} ${GLUT_LIBRARY} ${OpenCV_LIBS} ${catkin_LIBRARIES}
    -L${CMAKE_CURRENT_SOURCE_DIR}/cfg/ -ltkDNN
)

