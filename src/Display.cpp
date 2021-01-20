//
// Created by redwan on 1/16/21.
//

#include "../include/Display.h"
using namespace visualization;
Display::Display(const string &mapname):view_() {
    set_env(mapname);
    view_ = thread(&Display::show, this);

}

void Display::show() {
    auto img = background_.clone();
    do {
        // show robot on top
        img = update_robots(img);
        cv::imshow("Frame", img);
    } while (cv::waitKey(33) != 27);

}


Display::~Display() {
    if (view_.joinable())
    {
        view_.join();
        DEBUG_INFO("[Info]: Display Thread joined");
    }
}

void Display::add_robot(shared_ptr<Robot> robot) {
    robots_.push_back(robot);
}

cv::Mat Display::update_robots(cv::Mat background) {
    auto original = background_.clone();
    bool isUpdated = false;
    for(auto& robot: robots_)
    {
        if(robot->isMoved())
        {
            robot->update_map(original);
            isUpdated = true;
        }
    }
    if(isUpdated)
        std::swap(original, background);
    return background;
}

void Display::set_env(const string& yamlfile) {
    YAML::Node config = YAML::LoadFile(yamlfile);
    auto mapname = config["mapname"].as<string>();

    DEBUG_INFO(mapname<< "\n");
    background_= cv::imread(mapname);
    auto draw_ellipse = [&](const cv::Point& p, const cv::Scalar& color)
    {
        int thickness = 2;
        int lineType = 8;
        cv::Point pt1 = cv::Point(p.x - 40, p.y + 20);
        cv::Point pt2 = cv::Point(p.x + 40, p.y - 20);
        cv::rectangle (background_, pt1, pt2,
                       color, thickness, lineType);

    };
    assert(config["table1"]["places"].size() == config["table2"]["places"].size());
    for (int j = 0; j < 2; ++j) {
        auto table_index = "table"+to_string(j+1);
        auto color = get_cv_color(static_cast<Color>(config[table_index]["color"].as<int>()));
        DEBUG_INFO(table_index<< endl);
        for (int i = 0; i < config[table_index]["places"].size(); ++i) {
            DEBUG_INFO(i << " "<<config[table_index]["places"][to_string(i)] << endl);
            auto loc = config[table_index]["places"][to_string(i)] .as<vector<int>>();
            cv::Point p(loc[0], loc[1]);
            draw_ellipse(p, color);
        }
    }

}

void Robot::add_path(const vector<cv::Point> &path) {
    path_.clear();
    std::copy(path.begin(), path.end(), back_inserter(path_));
}

Robot::Robot(double x, double y, Color color, const string& config):
color_(color), PathPlanning(config) {
    updated_ = false;
    position_ = make_pair(x, y);
    traj_.push_back(position_);
}

void Robot::update(double x, double y) {
    position_.first = x;
    position_.second = y;
    traj_.push_back(position_);
    if (traj_.size() > MAX_HISTORY)
        traj_.pop_front();
    updated_ = true;
}

shared_ptr<Robot> Robot::get_ptr() {
    return shared_from_this();
}

bool Robot::isMoved() {
    return updated_;
}

void Robot::update_map(cv::Mat &background) {
    cv::Point center;
    center.x = position_.first;
//    center.y = HEIGHT - position_.second;
    center.y = position_.second;
    cv::circle( background,
            center,
            400/32,
                _get_color(),
            cv::FILLED,
            cv::LINE_8 );

    auto add_line = [&](cv::Point start_point, cv::Point end_point)
    {
        int thickness = 4;
        int lineType = cv::LINE_8;
        line( background,
              start_point,
              end_point,
              _get_color(),
              thickness,
              lineType );
    };
    // show path first
    if(path_.size()>1)
    {
        for (int i = 1; i < path_.size(); ++i) {
            add_line(path_[i-1], path_[i]);
        }
    }


    updated_ = false;
}



cv::Scalar Robot::_get_color() {
    return get_cv_color(color_);
}
