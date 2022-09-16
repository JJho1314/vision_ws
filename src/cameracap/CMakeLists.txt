cmake_minimum_required(VERSION 2.8.3)
project(cameracap)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# set(OpenCV_DIR "/usr/lib/aarch64-linux-gnu/cmake/opencv4/OpenCVConfig.cmake")
find_package(OpenCV 3 REQUIRED)

if(OpenCV_FOUND)
    message(STATUS "find opencv")
    set(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -DOPENCV")
    set(CMAKE_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} ")
endif()

message(STATUS "CMAKE_MODULE_PATH:::${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_SYSTEM_NAME:::${CMAKE_SYSTEM_NAME}")

#-------------------------------------------------------------------------------
# CUDA
#-------------------------------------------------------------------------------
find_package(CUDA 10.2 REQUIRED)
SET(CUDA_SEPARABLE_COMPILATION ON)
#set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -arch=sm_30 --compiler-options '-fPIC'")
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} --maxrregcount=32)

find_package(CUDNN REQUIRED)
include_directories(${CUDNN_INCLUDE_DIR})


cuda_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CUDA_INCLUDE_DIRS} ${CUDNN_INCLUDE_DIRS})
# cuda_add_library(kernels SHARED ${tkdnn_CUSRC})


message(STATUS "CUDA_LIBRARIES:::${CUDA_LIBRARIES}")
message(STATUS "CUDA_CUBLAS_LIBRARIES:::${CUDA_CUBLAS_LIBRARIES}")
message(STATUS "CUDNN_LIBRARIES:::${CUDNN_LIBRARIES}")
message(STATUS "OpenCV_LIBS:::${OpenCV_LIBS}")
message(STATUS "OPENCV_INCLUDE_DIRS:::${OpenCV_INCLUDE_DIRS}")
message(STATUS "OPENCV version:::${OpenCV_VERSION}")
message(STATUS "OPENCV dir:::${OpenCV_DIR}")
message(STATUS "catkin_LIBRARIES:::${catkin_LIBRARIES}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR:::${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CUDA_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIRS} ${NVINFER_INCLUDES})
#add_library(tkDNN SHARED ${tkdnn_SRC})
#target_link_libraries(tkDNN ${tkdnn_LIBS})

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  image_transport 
  cv_bridge
  autoware_msgs
)

#add_definitions(-DOBJ_DETECT)

# set(HKCAM_SOURCES src/hikcamera.cpp)




catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES cameracap
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

include_directories(
  ${CUDA_INCLUDE_DIRS}
  include
  include/tkDNN
  ${catkin_INCLUDE_DIRS}
)



#-------------------------------------------------------------------------------
# Build Node 
#-------------------------------------------------------------------------------

add_executable(${PROJECT_NAME}_node src/cameracap_node.cpp  src/imageProcess.cpp${NVENCODER_SOURCE})
target_link_libraries(${PROJECT_NAME}_node
  ${tkdnn_LIBS}
  -L${CMAKE_CURRENT_SOURCE_DIR}/cfg/ -ltkDNN
  ${OpenCV_LIBS}
  ${catkin_LIBRARIES}
  #MvCameraControl.so
  #darknet
  #pthread
  v4l2
  #nvbuf_utils
    
 )

 target_link_libraries(${PROJECT_NAME}_node "/usr/local/cuda/lib64/libcudart.so")

 target_link_libraries(${PROJECT_NAME}_node "/usr/local/cuda/lib64/libnvrtc.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/tegra/libcuda.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/tegra/libnvbuf_utils.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/tegra/libnvjpeg.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/tegra/libnvosd.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/tegra/libdrm.so.2")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/libnvparsers.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/libnvinfer.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/libX11.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/libEGL.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/libGLESv2.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/libv4l2.so")
 
 target_link_libraries(${PROJECT_NAME}_node "/usr/lib/aarch64-linux-gnu/libpthread.so")
 