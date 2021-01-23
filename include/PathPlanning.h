//
// Created by redwan on 1/19/21.
//

#ifndef TAMP_CPP_PATHPLANNING_H
#define TAMP_CPP_PATHPLANNING_H
#include <ompl/base/SpaceInformation.h>
#include <ompl/base/spaces/SE3StateSpace.h>
#include <ompl/base/spaces/SE2StateSpace.h>
#include <ompl/geometric/planners/rrt/RRTConnect.h>
#include <ompl/geometric/SimpleSetup.h>
#include <functional>
#include <ompl/config.h>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iterator>
#include <cassert>

namespace ob = ompl::base;
namespace og = ompl::geometric;
using namespace std;
class PathPlanning {
public:
    PathPlanning(const string& yamlfile);
    bool isStateValid(const ob::State *state);
    vector<cv::Point> move(const vector<double>& start, const vector<double>& goal);
    vector<double> get_target(int table_id, int block_id);
private:
    cv::Mat _img;
    vector<vector<double>> _top_blocks, _bottom_blocks;
};


#endif //TAMP_CPP_PATHPLANNING_H
