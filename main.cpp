#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <boost/program_options.hpp>
#include "include/Table.h"

#define DEBUG_INFO(x) cout << "\033[1;36m" << x << "\033[0m"

#define RECURSION_LIMIT (10000)
typedef pair<int, int> table_block_pair;
using namespace boost::program_options;

void available_actions(weak_ptr<Table> table, bool operation, vector<table_block_pair>&result)
{
    auto subject = table.lock();
    for (int i = 0; i < subject->blocks.size(); ++i) {
        if (!subject->status[i] ^ operation)
            result.push_back(make_pair(subject->get_id(), i));
    }
}

template <typename T>
bool winning_state(TableList<T>& task)
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
void move(const table_block_pair& a1, const table_block_pair& a2, TableList<T>& task)

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

size_t last_table_index = numeric_limits<size_t>::max();
template <typename T>
int solver(TableList<T>& task, int num_robots, unordered_map<size_t, TableList<T>> caches={}, int recursion_count = 0) {
    if (winning_state(task)) {
        DEBUG_INFO ("[Info]: Found winning states" << endl);
        return 0;
    }
    else if (recursion_count > RECURSION_LIMIT)
    {
        cerr << "[Info]: Not Found winning states" << endl;
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
        move(sample_pick, sample_drop, task);
    }
    auto task_id = task.get_hash();
    if(caches.find(task_id) == caches.end())
        caches[task_id] = task;
    TableList<T> new_task;
    std::copy(task.begin(), task.end(), back_inserter(new_task));
    return solver(new_task, num_robots, caches, ++recursion_count) + 1;

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


int main(int argc, char* argv[]) {
    DEBUG_INFO("[Info]: Block Transfer" << std::endl);
    srand (1234); // random seed

    options_description desc("Options");
    int num_places, num_blocks, num_robots;
    desc.add_options()
            ("help,h", "Help screen")
            ("num_places", value<int>()->default_value(5), "#places per table")
            ("num_robots", value<int>()->default_value(2), "total number of robots")
            ("num_blocks", value<int>()->default_value(3), "#blocks per table");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    num_places = vm["num_places"].as<int>();
    num_blocks = vm["num_blocks"].as<int>();
    num_robots = vm["num_robots"].as<int>();
    num_states( num_places,  num_blocks,  num_robots);



    auto table1 = make_shared<Table>(num_places,num_blocks,BLUE);
    auto table2 = make_shared<Table>(num_places,num_blocks,RED);
    int search_len;
    do{
        TableList<shared_ptr<Table>> task;
        // get hash value for each table
        task.emplace_back(table1->get_ptr());
        task.emplace_back(table2->get_ptr());

        search_len = solver(task, num_robots);

        if(search_len < RECURSION_LIMIT)
        DEBUG_INFO("[Info]: winning state " <<task.get_hash() << "| win after searching " <<
             search_len << endl);

    }while (search_len>=RECURSION_LIMIT);


    return 0;
}