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





std::mutex g_stitcherMtx[2];
std::condition_variable stitcherCon[2];

cv::Mat inputFrame;
std::string stitchercfgpath = "/home/nvidia/workspace/camera/cfg/stitcher-imx390cfg.yaml";



int main(int argc, char *argv[])
{

    YAML::Node config = YAML::LoadFile(stitchercfgpath);
    camSrcWidth = config["camsrcwidth"].as<int>();
    camSrcHeight = config["camsrcheight"].as<int>();
    undistorWidth = config["undistorWidth"].as<int>();
    undistorHeight = config["undistorHeight"].as<int>();
    stitcherinputWidth = config["stitcherinputWidth"].as<int>();
    stitcherinputHeight = config["stitcherinputHeight"].as<int>();
    USED_CAMERA_NUM = config["USED_CAMERA_NUM"].as<int>();
    std::string net = config["netpath"].as<string>();
    std::string cfgpath = config["camcfgpath"].as<string>();
    std::string canname = config["canname"].as<string>();
    undistor = config["undistor"].as<bool>();

    stCamCfg camcfgs[CAMERA_NUM] = {stCamCfg{camSrcWidth,camSrcHeight,undistorWidth,undistorHeight,stitcherinputWidth,stitcherinputHeight,undistor,1,"/dev/video0"}};


    static std::shared_ptr<nvCam> cameras[CAMERA_NUM];
    for(int i=0;i<USED_CAMERA_NUM;i++)
        cameras[i].reset(new nvCam(camcfgs[i]));
    
    while(1)
    {

        auto t = cv::getTickCount();
        auto all = cv::getTickCount();

        cameras[0]->read_frame();
        inputFrame = cameras[0]->m_ret;
        cv::imshow("img", inputFrame);
        printf("read takes : %.3f ms", ((getTickCount() - t) / getTickFrequency()) * 1000);
        cv::waitKey(1);
    }
    return 0;
}
