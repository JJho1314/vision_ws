#include <ros/ros.h>
#include <std_msgs/Empty.h>
#include <sensor_msgs/NavSatFix.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/CameraInfo.h>
#include <camera_info_manager/camera_info_manager.h>

#include <string>
#include <thread>
#include <memory>
#include <opencv2/core/utility.hpp>
#include "yaml-cpp/yaml.h"
#include "nvcam.hpp"
// #include "config.h"


// #define CAMERA_NUM 8
// #define USED_CAMERA_NUM 6
// #define BUF_LEN 65540 

using namespace cv;

// std::string stitchercfgpath = "/home/nvidia/workspace/vision_ws/src/gmsl_camera/cfg/stitcher-imx390cfg.yaml";
bool rescale_camera_info_;

sensor_msgs::CameraInfo info_;






void rescaleCameraInfo(int width, int height)
{
  double width_coeff = static_cast<double>(width) / info_.width;
  double height_coeff = static_cast<double>(height) / info_.height;
  info_.width = width;
  info_.height = height;

  // See http://docs.ros.org/api/sensor_msgs/html/msg/CameraInfo.html for clarification
  info_.K[0] *= width_coeff;
  info_.K[2] *= width_coeff;
  info_.K[4] *= height_coeff;
  info_.K[5] *= height_coeff;

  info_.P[0] *= width_coeff;
  info_.P[2] *= width_coeff;
  info_.P[5] *= height_coeff;
  info_.P[6] *= height_coeff;
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "gmsl_camera");
    ros::NodeHandle node_;
    image_transport::ImageTransport it(node_);   
    image_transport::CameraPublisher pub = it.advertiseCamera("/gmsl_cam/image_raw", 1);
    
    ros::Rate loop_rate(30);
    std::string imx390_cfg;
    node_.getParam("/imx390_cfg_url", imx390_cfg);

    YAML::Node config = YAML::LoadFile(imx390_cfg);
    vendor = config["vendor"].as<int>();
    camSrcWidth = config["camsrcwidth"].as<int>();
    camSrcHeight = config["camsrcheight"].as<int>();
    undistorWidth = config["undistorWidth"].as<int>();
    undistorHeight = config["undistorHeight"].as<int>();
    stitcherinputWidth = config["stitcherinputWidth"].as<int>();
    stitcherinputHeight = config["stitcherinputHeight"].as<int>();
    USED_CAMERA_NUM = config["USED_CAMERA_NUM"].as<int>();
    undistor = config["undistor"].as<bool>();

    stCamCfg camcfgs[CAMERA_NUM] ={stCamCfg{camSrcWidth,camSrcHeight,distorWidth,distorHeight,undistorWidth,undistorHeight,stitcherinputWidth,stitcherinputHeight,undistor,1,"/dev/video0", vendor}};

    
    static std::shared_ptr<nvCam> cameras[CAMERA_NUM];
    for(int i=0;i<USED_CAMERA_NUM;i++)
        cameras[i].reset(new nvCam(camcfgs[i]));

    cv::Mat inputFrame;

    //**get camera_info start
    camera_info_manager::CameraInfoManager info_manager_(node_, "camera", "");

    std::string url;
    if (node_.getParam("/camera_info_url", url))
    {
        if (info_manager_.validateURL(url))
        {
        info_manager_.loadCameraInfo(url);
        }
    }

    rescale_camera_info_ = node_.param<bool>("/rescale_camera_info", false);

    for (int i = 0;; ++i)
    {
        int code = 0;
        double value = 0.0;
        std::stringstream stream;
        stream << "property_" << i << "_code";
        const std::string param_for_code = stream.str();
        stream.str("");
        stream << "property_" << i << "_value";
        const std::string param_for_value = stream.str();
        if (!node_.getParam(param_for_code, code) || !node_.getParam(param_for_value, value))
        {
            break;
        }
    }
    //**get camera_info end

    while(ros::ok()){
        auto t = cv::getTickCount();
        auto all = cv::getTickCount();

        cameras[0]->read_frame();
        inputFrame = cameras[0]->m_ret;
        // printf("read takes : %.3f ms\n", ((getTickCount() - t) / getTickFrequency()) * 1000);


        info_ = info_manager_.getCameraInfo();
        info_.header.frame_id = "camera";
        info_.header.stamp = ros::Time::now();

        sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", inputFrame).toImageMsg();
        msg->header = info_.header;
        pub.publish(*msg, info_);

        ros::spinOnce();
        loop_rate.sleep();
        // cv::imshow("img", cameras[0]->m_ret);
        // cv::waitKey(5);
    }
    return 0;
}
