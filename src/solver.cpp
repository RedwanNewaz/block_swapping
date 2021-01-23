//
// Created by redwan on 1/16/21.
//
#include <algorithm>
#include <map>
#include <future>
#include <chrono>
#include <thread>
#include "../include/Table.h"
#include "../include/Task.h"
#define DEBUG_INFO(x) cout << "\033[1;36m" << x << "\033[0m"
#define RECURSION_LIMIT (10000)
using namespace std;
using namespace std::literals;
typedef pair<int, Task> SOL;




namespace recursive_solver
{
    typedef pair<size_t, size_t> table_block_pair;
    inline void available_actions(weak_ptr<Table> table, bool operation, vector<table_block_pair>&result)
    {
        auto subject = table.lock();
        for (int i = 0; i < subject->blocks.size(); ++i) {
            if (!subject->status[i] ^ operation)
                result.push_back(make_pair(subject->get_id(), i));
        }
    }
    template <typename T>
    inline bool winning_state(TableList<T>& task)
    {
        bool result = true;
        for(auto t : task)
        {
            for (auto block: t->blocks)
            {
                bool isValidColor = (block.color != t->default_color) || (block.color == BLACK);
                result = result && isValidColor;
            }
        }
        return result;
    }
    template <typename T>
    inline void move(const table_block_pair& a1, const table_block_pair& a2, TableList<T>& task)

    {
        int i = task.find_index(a1.first);
        int j = task.find_index(a2.first);
        std::swap(task[i]->blocks[a1.second].color, task[j]->blocks[a2.second].color);
        swap(task[i]->status[a1.second], task[j]->status[a2.second]);
    }

    template <typename T>
    T choice(vector<T>& data)
    {
        vector<T> samples_out;
        std::sample(data.begin(), data.end(), std::back_inserter(samples_out),
                    1, std::mt19937{std::random_device{}()});
        return samples_out.at(0);
    }


    template <typename T>
    inline int solver(TableList<T>& task, int num_robots, Task& assignments, int recursion_count = 0) {
        if (winning_state(task)) {
//            DEBUG_INFO ("[Info]: Found winning states  "<< task.get_hash() << endl);
            return 0;
        }
        else if (recursion_count > RECURSION_LIMIT)
        {
//            cerr << "[Info]: Not Found winning states" << endl;
            return -1;
        }
        // get available actions in terms of picks and places
        vector<table_block_pair> blocks, places;

        bool pick = true;
        bool drop = false;
        for (int i = 0; i < task.size(); ++i) {
            available_actions(task[i]->get_ptr(), pick, blocks);
            available_actions(task[i]->get_ptr(), drop, places);
        }

        for (int j = 0; j < num_robots; ++j) {
            auto sample_pick = choice(blocks);
            auto sample_drop = choice(places);
            assignments.add_task(j, sample_pick.second, sample_drop.second);
            assignments.add_table(j, sample_pick.first, sample_drop.first);
            move(sample_pick, sample_drop, task);
        }
        assignments.add_state(task.get_hash());

        return solver(task, num_robots, assignments, ++recursion_count) + 1;

    }

}



inline SOL synthesis(int num_places, int num_blocks, int num_robots, const vector<bool>&status1, const vector<bool>&status2)
{
    auto table1 = make_shared<Table>(num_places,num_blocks,BLUE, 0, status1);
    auto table2 = make_shared<Table>(num_places,num_blocks,RED, 1, status2);
    int search_len;

    TableList<shared_ptr<Table>> task;
    task.emplace_back(table1->copy());
    task.emplace_back(table2->copy());
    Task assignments(num_robots);
    search_len = recursive_solver::solver(task, num_robots, assignments);

    return make_pair(search_len, assignments);
}




inline vector<Task> parallel_solver(int num_places, int num_blocks, int num_robots, int num_samples)
{

    random_device randomDevice;
    mt19937 gt(randomDevice());
    vector<bool> input1(num_places, false);
    vector<bool> input2(num_places, false);
    for (int i = 0; i < num_blocks; ++i) {
        input1[i] = input2[i] = true;
    }
    shuffle(input1.begin(), input1.end(), gt);
    shuffle(input2.begin(), input2.end(), gt);

    DEBUG_INFO("[Info]: number of samples " << num_samples << endl);
    vector<future<SOL>>parallel;
    parallel.reserve(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        parallel.emplace_back(std::async(std::launch::async, synthesis, num_places, num_blocks, num_robots, input1, input2));
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
    vector<Task> solution_pool;
    for (int i = 0; i < num_samples; ++i)
    {
        auto solution = parallel[i].get();
        solution_pool.emplace_back(solution.second);
    }
    terminated = true;
    thr.join();
    return solution_pool;
}
