#include <iostream>
#include <algorithm>
#include "include/Table.h"

typedef pair<int, int> table_block_pair;

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


template <typename T>
void solver(TableList<T>& task, int num_robots = 2) {
    if (winning_state(task)) {
        cout << "[Info]: Found winning states" << endl;
        return;
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

    solver(task, num_robots);
}
int main() {
    std::cout << "Hello, World!" << std::endl;
    srand (1234); // random seed
    auto table1 = make_shared<Table>(5,3,BLUE);
    auto table2 = make_shared<Table>(5,3,RED);
    TableList<shared_ptr<Table>> task;

    // get hash value for each table
    task.emplace_back(table1);
    task.emplace_back(table2);

    solver(task);

    cout << task.get_hash() << "| win " <<
    winning_state(task) << endl;

    return 0;
}