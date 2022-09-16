#include "imageProcess.h"

tk::dnn::Yolo3Detection detNN;

int n_batch = 1;
std::string net ="/home/nvidia/model/yolov4/yolo4_berkeley_fp16.rt" ; //yolo4_fp16.rt(jsmb)/yolo4_320_fp16.rt //yolo4_berkeley_fp16.rt（44ms, double detect）, yolo4_berkeley_fp16.rt(30ms),  kitti_yolo4_int8.rt 


cv::Mat imageProcessor::ImageDetect(cv::Mat img, std::vector<int> &detret)
{
    cv::Mat tmp_Img=img.clone();
    std::vector<cv::Mat> batch_frame;
    std::vector<cv::Mat> batch_dnn_input;
    std::vector<std::string> classnames;

    batch_dnn_input.clear();
    batch_frame.clear();

    batch_frame.push_back(tmp_Img);
    // this will be resized to the net format
    batch_dnn_input.push_back(tmp_Img.clone());

    auto start = std::chrono::steady_clock::now();

    detNN.update(batch_dnn_input, n_batch);
    detret.clear();
    // detNN.draw(batch_frame);
    detNN.draw(batch_frame, detret, classnames);
    // printf("detNN draw_boxes okkkkk\n");
    // printf("detret size:%d\n",  detret.size());   //x,y,w,h,class,probality

    // if(detret_all.size()>=6 ){
    //     for(int i=0;i<detret_all.size()/6;i++){
    //         sendData.target_id[i]=i;
    //         sendData.target_x[i]=detret_all[6*i];
    //         sendData.target_y[i]=detret_all[6*i+1];
    //         sendData.target_w[i]=detret_all[6*i+2];
    //         sendData.target_h[i]=detret_all[6*i+3];
    //         sendData.target_class[i]=detret_all[6*i+4]; 
    //         sendData.target_prob[i]=detret_all[6*i+5];
    //     }
    // }

    return batch_frame.back();
}



//Init here
imageProcessor::imageProcessor() {

    int n_classes = 80;
    float conf_thresh=0.8;
    detNN.init(net, n_classes, n_batch, conf_thresh);

   std::cout<<"detNN init okkkkk"<<std::endl;
}
