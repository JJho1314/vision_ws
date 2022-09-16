#include <ros/ros.h>
#include <std_msgs/Empty.h>
#include <sensor_msgs/NavSatFix.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <autoware_msgs/DetectedObject.h>
#include <autoware_msgs/DetectedObjectArray.h>

#include <fstream>

#include "imageProcess.h"

using namespace cv;

 
cv::Mat  recievedImg;

void ImgCallback(const sensor_msgs::Image &msg)
{
    ROS_INFO_STREAM("yoloDetCallback");
    cv_bridge::CvImagePtr cv_ptr;
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    recievedImg = cv_ptr->image.clone();
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "camcap_node");
    ros::NodeHandle n;
    image_transport::ImageTransport it(n);
    image_transport::Publisher pub = it.advertise("/image_rects", 1);
    ros::Publisher obj_pub = n.advertise<autoware_msgs::DetectedObjectArray>("/detection/image_detector/objects", 1);
    //ros::Subscriber Img_sub = n.subscribe("/zed_node/left_raw/image_raw_color", 1, ImgCallback);
    ros::Subscriber Img_sub = n.subscribe("/gmsl_cam/image_rect_color", 1, ImgCallback);  //image_raw  
    ros::Rate loop_rate(30);
    // cv::Mat img_jsmb = cv::imread("/home/nvidia/1.jpeg");
    imageProcessor nvProcessor;     //图像处理类
    cv::Mat result;
    autoware_msgs::DetectedObjectArray out_message;
    while(ros::ok()){

        ros::spinOnce();
        auto yoloret = recievedImg.clone();
        // auto yoloret = img_jsmb.clone();  //modify 20220720
        if(yoloret.empty())
            continue;
        // cv::resize(yoloret, yoloret, Size(640, 480)); //modify 20220720
        std::vector<int> detret;
        detret.clear();
        auto start = std::chrono::steady_clock::now();

        result = nvProcessor.ImageDetect(yoloret,detret);

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> spent = end - start;
        std::cout << " Time: " << spent.count() << " sec \n";
        
        sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", result).toImageMsg();
        pub.publish(msg);
        
        out_message.objects.clear();
        if(detret.size()>=6)
        {
            for (unsigned int i = 0; i < detret.size()/6; ++i)
            {
                    autoware_msgs::DetectedObject obj;

                    obj.x = int(detret[6*i]);
                    obj.y = int(detret[6*i+1]);
                    obj.width = int(detret[6*i+2]);
                    obj.height = int(detret[6*i+3]);
                    if (obj.x < 0)
                        obj.x = 0;
                    if (obj.y < 0)
                        obj.y = 0;
                    if (obj.width < 0)
                        obj.width = 0;
                    if (obj.width < 0)
                        obj.height = 0;


                    if(detret[6*i+4]==0){
                        obj.label = "person";
                    }else if(detret[6*i+4]==1){
                        obj.label = "car";
                    }else if(detret[6*i+4]==2){
                        obj.label = "cyclist";
                    }else{
                        obj.label = "unknow";
                    }
                    obj.score = detret[6*i+5];
                    obj.valid = true;
                    std::cout<<obj.x<<", "<<obj.y<<", "<<obj.width<<", "<<obj.label<<std::endl;
                    out_message.objects.push_back(obj);
            }
        }
        obj_pub.publish(out_message);
        loop_rate.sleep();
        // cv::imshow("ret", result);
        // cv::waitKey(30);
        // continue;
    }
    return 0;
}
