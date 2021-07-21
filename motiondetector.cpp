#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdlib>
#include <cmath>

int main() {
    cv::Mat image;
    int width, height;
    cv::VideoCapture cap;
    std::vector<cv::Mat> planes;
    cv::Mat hist;
    int nbins = 64;
    float range[] = { 0, 255 };
    const float* histrange = { range };
    bool uniform = true;
    bool acummulate = false;
    int key;
    cap.open(1);

    if (!cap.isOpened()) {
        std::cout << "cameras indisponiveis";
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    std::cout << "largura = " << width << std::endl;
    std::cout << "altura  = " << height << std::endl;

    int histw = nbins, histh = nbins / 2;
    cv::Mat histImg(histh, histw, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat led(histh, histw, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat hist2;
    cap >> image;

    cv::split(image, planes);
    cv::calcHist(&planes[0], 1, 0, cv::Mat(), hist2, 1, &nbins, &histrange, uniform, acummulate);
    cv::normalize(hist2, hist2, 0, histImg.rows, cv::NORM_MINMAX, -1, cv::Mat());
    

    while (1) {
        cap >> image;
        cv::split(image, planes);
        cv::calcHist(&planes[0], 1, 0, cv::Mat(), hist, 1, &nbins, &histrange, uniform, acummulate);
        cv::normalize(hist, hist, 0, histImg.rows, cv::NORM_MINMAX, -1, cv::Mat());
        histImg.setTo(cv::Scalar(0));

        led.setTo(cv::Scalar(0, 0, 255));
        led.copyTo(image(cv::Rect(0, histh, nbins, histh)));

        double dif = cv::compareHist(hist, hist2, 0);
        std::cout << dif << std::endl;

        if (dif < 0.995) {
            led.setTo(cv::Scalar(0, 255, 0));
            led.copyTo(image(cv::Rect(0, histh, nbins, histh)));
            std::cout << abs(dif) << std::endl;
        }

        cv::calcHist(&planes[0], 1, 0, cv::Mat(), hist2, 1, &nbins, &histrange, uniform, acummulate);
        cv::normalize(hist2, hist2, 0, histImg.rows, cv::NORM_MINMAX, -1, cv::Mat());

        for (int i = 0; i < nbins; i++) {
            cv::line(histImg,
                cv::Point(i, histh),
                cv::Point(i, histh - cvRound(hist.at<float>(i))),
                cv::Scalar(255, 0, 0), 1, 8, 0);
        }
        histImg.copyTo(image(cv::Rect(0, 0, nbins, histh)));
        cv::imshow("image", image);
        key = cv::waitKey(30);
        if (key == 27) break;
    }
    return 0;
}