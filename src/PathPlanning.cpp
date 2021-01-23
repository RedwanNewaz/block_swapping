//
// Created by redwan on 1/19/21.
//

#include "../include/PathPlanning.h"
using namespace cv;
PathPlanning::PathPlanning(const string &yamlfile) {
    YAML::Node config = YAML::LoadFile(yamlfile);
    auto mapname = config["occupancy_map"].as<string>();
    _img = cv::imread(mapname);
    cv::cvtColor(_img, _img, cv::COLOR_BGR2GRAY);

    assert(config["table1"]["places"].size() == config["table2"]["places"].size());
    for (int j = 0; j < 2; ++j) {
        auto table_index = "table"+to_string(j+1);
        int N = config[table_index]["places"].size();
        if(j==0)_top_blocks.resize(N);
        else _bottom_blocks.resize(N);
        for (int i = 0; i < N; ++i) {
            auto loc = config[table_index]["places"][to_string(i)] .as<vector<double>>();
            if (j == 0 )
            {
                _top_blocks[i].resize(loc.size());
                _top_blocks[i] = loc;
            }
            else
            {
                _bottom_blocks[i].resize(loc.size());
                _bottom_blocks[i] = loc;
            }
        }
    }

}

bool PathPlanning::isStateValid(const ob::State *state)
{
    // cast the abstract state type to the type we expect
    const auto *se2state = state->as<ob::SE2StateSpace::StateType>();
    int x = (int)se2state->getX();
    int y = (int)se2state->getY();
    auto size = _img.size;

    if (x<size[1] && y<size[0])
    {
        return (_img.at<uint8_t>(y, x) >= 255) ;
    }
    // return a value that is always true but uses the two variables we define, so we avoid compiler warnings
    return false;
}


vector<cv::Point> PathPlanning::move( const vector<double>& vec_start, const vector<double>& vec_goal) {

    //TODO sanity check
    assert(vec_start.size() == 2);
    assert(vec_goal.size() == 2);

    // construct the state space we are planning in
    auto space(std::make_shared<ob::SE2StateSpace>());

    auto size = _img.size;
//    cout << size[0] << " "<< size[1] << endl;
    int img_bound = max(size[0], size[1]);
    // set the bounds for the R^2 part of SE(2)
    ob::RealVectorBounds bounds(2);
    bounds.setLow(0);
    bounds.setHigh(img_bound);

    space->setBounds(bounds);

    // define a simple setup class
    og::SimpleSetup ss(space);

    // set state validity checking for this space
    auto validity = std::bind(&PathPlanning::isStateValid,this, std::placeholders::_1);
    ss.setStateValidityChecker(validity);


    // create a random start state
    ob::ScopedState<ob::SE2StateSpace> start(space);
//    start.random();
    start->setX(vec_start[0]);
    start->setY(vec_start[1]);
    // create a random goal state
    ob::ScopedState<ob::SE2StateSpace> goal(space);
    goal->setX(vec_goal[0]);
    goal->setY(vec_goal[1]);

    // set the start and goal states
    ss.setStartAndGoalStates(start, goal);
    // this call is optional, but we put it in to get more output information
    ss.setup();
//    ss.print();

    // attempt to solve the problem within one second of planning time
    ob::PlannerStatus solved = ss.solve(1.0);
    vector<cv::Point> result;
    if (solved)
    {
        std::cout << "Found solution:" << std::endl;
        // print the path to screen
        ss.simplifySolution();
        auto path = ss.getSolutionPath();
        path.interpolate();
        for (auto state:path.getStates())
        {
            const auto *se2state = state->as<ob::SE2StateSpace::StateType>();
//            cout<< se2state->getX() << " " << se2state->getY() << endl;
            cv::Point p(se2state->getX(), se2state->getY());
            result.emplace_back(p);
        }
    }
    else
        std::cout << "No solution found" << std::endl;

    return result;
}

vector<double> PathPlanning::get_target(int table_id, int block_id) {
    return (table_id == 0)? _bottom_blocks[block_id]:_top_blocks[block_id];
}

