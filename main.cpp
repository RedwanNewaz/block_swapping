#include <iostream>
#include <boost/program_options.hpp>
#include <map>
#include "src/solver.cpp"
#include <future>
#include <chrono>
#include <thread>
#include "include/Display.h"

#include <yaml-cpp/yaml.h>
#define SIMULATION

using namespace boost::program_options;
using namespace std;
using namespace std::literals;


void analysis(const vector<int>& len_solutions, int num_samples)
{
    // gather result and compute pdf
    unordered_map<int, int> prob_tracker;
    int min_sol_len = numeric_limits<int>::max();
    int max_sol_len = -numeric_limits<int>::max();
    float average_len;
    int i = 0;
    for (auto current_len: len_solutions) {
        min_sol_len = std::min(min_sol_len,current_len);
        max_sol_len = std::max(max_sol_len,current_len);
        if(prob_tracker.find(current_len) != prob_tracker.end())
            prob_tracker[current_len] += 1;
        else
            prob_tracker[current_len] = 1;

        if(i==0)
            average_len = (float)current_len;
        else
            average_len += (float)current_len;
        ++i;
    }
    average_len/=float(num_samples);

    DEBUG_INFO("[RESULT]: min_sol_len  "<< min_sol_len << endl);
    DEBUG_INFO("[RESULT]: max_sol_len  "<< max_sol_len << endl);
    DEBUG_INFO("[RESULT]: average_len  "<< average_len << endl);

    // compute standard deviation
    double numerator;
    bool first = true;
    for (auto it = prob_tracker.begin(); it!=prob_tracker.end(); ++it)
    {
        if(first)
        {
            numerator = pow(it->first - average_len, 2);
            first = false;
        }
        else
            numerator += pow(it->first - average_len, 2);
    }
    double std_dev = sqrt(numerator/float(num_samples - 1));
    DEBUG_INFO("[RESULT] std_dev  "<< std_dev << endl);

}


void solver(int num_places,  int num_blocks, int num_robots, int num_samples)
{

    auto synthesis = [&]() {
        auto table1 = make_shared<Table>(num_places,num_blocks,BLUE);
        auto table2 = make_shared<Table>(num_places,num_blocks,RED);
        int search_len;
        size_t start, goal;
        map<size_t, size_t> solution;
        int solution_len = RECURSION_LIMIT;
            TableList<shared_ptr<Table>> task;
            // get hash value for each table
            task.emplace_back(table1->copy());
            task.emplace_back(table2->copy());
            solution.clear();
//            start = task.get_hash();
            search_len = recursive_solver::solver(task, num_robots, solution);
//            goal = task.get_hash();
//            solution_len = recursive_solver::solution_length(start, goal, solution, false);
//            if (search_len < RECURSION_LIMIT)
//                DEBUG_INFO("[Info]: start state " << start << "| win after searching " <<
//                                                  search_len << "| solution length " << solution_len << endl);

        return make_pair(search_len, solution);
    };

    DEBUG_INFO("[Info]: number of samples " << num_samples << endl);
    using SOL = pair<int, map<size_t, size_t>>;
    vector<future<SOL>>parallel;
    parallel.reserve(num_samples);
    for (int i = 0; i < num_samples; ++i) {
            parallel.emplace_back(std::async(std::launch::async, synthesis));
        }
    DEBUG_INFO("[Info]: parallel solvers started "<<endl);

    auto start_time = std::chrono::steady_clock::now();
    bool terminated = false;
    auto time_display = [&]()
    {
        while (!terminated)
        {
            auto end_time = std::chrono::steady_clock::now();
            std::cout << "..("
                      << std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count()
                      << " s)..\r" << std::flush;
            this_thread::sleep_for(1s);
        }
    };
    std::thread thr{time_display};
    vector<int> len_sols;
    for (int i = 0; i < num_samples; ++i)
        len_sols.push_back(parallel[i].get().first);
    terminated = true;
    thr.join();
    analysis(len_sols, num_samples);
}

size_t num_states(int num_places, int num_blocks, int num_robots)
{
    const size_t num_tables = 2;
    auto temp =  num_tables * (num_places - num_blocks);
    auto num_transitions = pow(temp, 3);
    auto total_num_states = pow(2, num_transitions);
    DEBUG_INFO("[Info]: each transition has " << num_transitions << endl);
    DEBUG_INFO("[Info]: total number of transitions "<<total_num_states << endl);
    return total_num_states;
}

void show_plan(const RobotPtr& robot, int from, int to, bool dir)
{
    auto plan = robot->move(from, to, dir);
    robot->add_path(plan);
    for(auto& p:plan)
    {
        robot->update(p.x, p.y);
        this_thread::sleep_for(100ms);
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
    num_states( num_places,  num_blocks,  num_robots);
    solver( num_places,  num_blocks,  num_robots, num_samples);
#ifdef SIMULATION
    string config_file = "../examples/test1.yaml";
    PathPlanning path(config_file);

    visualization::Display disp(config_file);
    auto robot1 = make_shared<visualization::Robot>(200, 200, visualization::RED, config_file);
    auto robot2 = make_shared<visualization::Robot>(100, 50, visualization::CYAN, config_file);
    disp.add_robot(robot1->get_ptr());
    disp.add_robot(robot2->get_ptr());

    thread robo2 = thread(show_plan, robot2->get_ptr(), 3, 1, true);
    thread robo1 = thread(show_plan, robot1->get_ptr(), 1, 3, false);

    robo1.join();
    robo2.join();


#endif




    return 0;
}