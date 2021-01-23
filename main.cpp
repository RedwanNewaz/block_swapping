#include <iostream>
#include <boost/program_options.hpp>
#include "src/solver.cpp"
#include "include/Display.h"
#define SIMULATION

using namespace boost::program_options;


void execute_plan(const RobotPtr& robot,  Task task,  int robot_id)
{
    auto plan = task[robot_id];
    for (int i = 1; i < plan.size() - 1; ++i) {
        auto pick = plan[i-1];
        auto drop = plan[i];
        auto from = task.get_table(robot_id, pick);
        auto to = task.get_table(robot_id, drop);
        auto start = robot->get_target(from, pick);
        auto goal = robot->get_target(to, drop);
        auto path = robot->move(start, goal);
        robot->add_path(path);
        for(auto& point : path)
        {
            robot->update(point.x, point.y);
            this_thread::sleep_for(50ms);
        }

    }

}

int main(int argc, char* argv[]) {
    DEBUG_INFO("[Info]: Block Transfer" << std::endl);
    srand (1234); // random seed

    options_description desc("Options");
    int num_places, num_blocks, num_robots, num_samples;
    desc.add_options()
            ("help,h", "Help screen")
            ("num_places", value<int>()->default_value(4), "#places per table")
            ("num_robots", value<int>()->default_value(2), "total number of robots")
            ("num_blocks", value<int>()->default_value(3), "#blocks per table")
            ("num_samples", value<int>()->default_value(1000), "#samples for solutions");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    num_places = vm["num_places"].as<int>();
    num_blocks = vm["num_blocks"].as<int>();
    num_robots = vm["num_robots"].as<int>();
    num_samples = vm["num_samples"].as<int>();

    vector<Task> solution_pool = parallel_solver(num_places, num_blocks, num_robots, num_samples);
    vector<size_t> solution_lengths(solution_pool.size());
    std::transform(solution_pool.begin(), solution_pool.end(), solution_lengths.begin(), [&](const Task& t){return t.size();});
    auto it = std::min_element(solution_lengths.begin(), solution_lengths.end());
    auto index = std::distance(solution_lengths.begin(), it);
    DEBUG_INFO(solution_pool[index]);



#ifdef SIMULATION
    string config_file = "../examples/test1.yaml";
    PathPlanning path(config_file);

    visualization::Display disp(config_file);
    auto robot1 = make_shared<visualization::Robot>(200, 200, visualization::RED, config_file);
    auto robot2 = make_shared<visualization::Robot>(100, 50, visualization::CYAN, config_file);
    disp.add_robot(robot1->get_ptr());
    disp.add_robot(robot2->get_ptr());

    thread robo2 = thread(execute_plan, robot2->get_ptr(), solution_pool[index], 1);
    thread robo1 = thread(execute_plan, robot1->get_ptr(), solution_pool[index], 0);
    robo1.join();
    robo2.join();

#endif




    return 0;
}