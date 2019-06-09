#include "opencv_helper.h"

#include <cstdio>


#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

int i = 0;



struct Data {
    std::vector<cv::Mat> masks;
};

void* init_data() {
    Data* data = new Data;
    data->masks.push_back(cv::imread("nbcBetterMask.png", cv::IMREAD_UNCHANGED));
    data->masks.push_back(cv::imread("nbcOtherMask.png", cv::IMREAD_UNCHANGED));
    data->masks.push_back(cv::imread("nbcOther2Mask.png", cv::IMREAD_UNCHANGED));
    data->masks.push_back(cv::imread("nbcOther3Mask.png", cv::IMREAD_UNCHANGED));
    
    return (void*) data;
}

float helper(unsigned char* rgb_data, void* info) {
    Data* data = (Data*) info;

    i += 1;

    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    cv::Mat image(720, 1280, CV_8UC3, rgb_data);

    cv::Mat grey_image;
    cv::cvtColor(image, grey_image, CV_BGR2GRAY);

    cv::Mat grad_x, grad_y, grad;
    cv::Mat abs_grad_x, abs_grad_y;

    /// Gradient X
    cv::Sobel( grey_image, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    cv::convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    cv::Sobel( grey_image, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
    cv::convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    cv::Mat threshold_grad;
    cv::adaptiveThreshold(grad, threshold_grad, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, -3);

    std::vector<float> num(data->masks.size()); 
    std::vector<float> num_incorrect(data->masks.size()); 

    for (int y = 0; y < 720; y++) {
        for (int x = 0; x < 1280; x++) {

            int current_value = threshold_grad.at<unsigned char>(y, x);

            for (int i = 0; i < data->masks.size(); i++) {
                int a = data->masks[i].at<cv::Vec4b>(y,x)[3];
                int value = data->masks[i].at<cv::Vec4b>(y,x)[0];

                if (a == 255) {
                    num[i] += 1;
                    if (value != current_value) {
                        num_incorrect[i] += 1;
                    }
                } 
            }
        }
    }

    if (i % 100 == 0) {
        printf("I am helping\n");

        cv::imwrite("hope.png", threshold_grad );    
    }

    float min_incorrect = 1.0;

    for (int i = 0; i < data->masks.size(); i++) {
        min_incorrect = std::min(min_incorrect,  num_incorrect[i] / num[i]);
    }

    return min_incorrect;
}