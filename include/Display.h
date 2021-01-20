//
// Created by redwan on 1/16/21.
//

#ifndef TAMP_CPP_DISPLAY_H
#define TAMP_CPP_DISPLAY_H
#include <iostream>
#include <string>
#include <matplot/matplot.h>
#include <thread>
#include <chrono>
#include <array>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>
#include <list>
#include <cassert>
#include <yaml-cpp/yaml.h>
#include "PathPlanning.h"
#define MAX_HISTORY (20)

using namespace std;
using namespace matplot;
#define DEBUG_INFO(x) cout << "\033[1;36m" << x << "\033[0m"


namespace visualization
{
    enum Color{
        RED,
        GREEN,
        BLUE,
        YELLOW,
        CYAN
    };
    inline cv::Scalar get_cv_color(const Color& color)
    {
        switch (color) {
            case RED: return cv::Scalar( 0, 0, 255 );
            case GREEN: return cv::Scalar( 0, 255, 0 );
            case BLUE: return cv::Scalar( 255, 0, 0 );
            case CYAN: return cv::Scalar( 255, 255, 0 );
            case YELLOW: return cv::Scalar( 0, 255, 255 );
            default:
                return cv::Scalar( 0, 0, 0 );
        }
    }
    class Robot: public enable_shared_from_this<Robot>, public PathPlanning{
    public:
        using point = pair<double, double>;
        Robot(double x, double y, Color color, const string& config);

        void update(double x, double y);
        shared_ptr<Robot> get_ptr();
        bool isMoved();
        void update_map(cv::Mat& background);
        void add_path(const vector<cv::Point>& path);

    private:
        point position_;
        Color color_;
        list<point> traj_;
        vector<cv::Point> path_;
        bool updated_;
    protected:
        cv::Scalar _get_color();
    };
    class Display {
    public:
        Display(const string& mapname);
        void show();
        void add_robot(shared_ptr<Robot>robot);
        void set_env(const string& yamlfile);
        virtual ~Display();

    private:
        cv::Mat background_;
        thread view_;
        vector<shared_ptr<Robot>> robots_;

    protected:
        cv::Mat update_robots(cv::Mat background);
    };
}
typedef std::shared_ptr<visualization::Robot> RobotPtr;


#endif //TAMP_CPP_DISPLAY_H
